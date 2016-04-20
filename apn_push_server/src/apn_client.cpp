#include "apn_client.h"
#include "net/ef_sock.h"
#include "base/ef_base64.h"
#include "base/ef_hex.h"
#include "base/ef_log.h"
#include "base/ef_statistic.h"
#include <iostream>
#include <sstream>
#include <sys/epoll.h>

namespace gim{

using namespace ef;
using namespace std;

int ApnClient::initApnClientModule(){
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();
	SSL_library_init();
	return 0;
}

ApnClient::ApnClient()
	:m_ctx(NULL),
	m_ssl(NULL),
	m_fd(EF_INVALID_SOCKET),
	m_msg_id(0),
	m_que_size(0),
	m_reconnect_span(DEFAULT_RECONNECT_SPAN){
		
}

ApnClient::~ApnClient(){
	if(m_ssl){
		SSL_shutdown(m_ssl);
		SSL_free(m_ssl);
		m_ssl = NULL;
	}
	if(m_ctx){
		SSL_CTX_free(m_ctx);
		m_ctx = NULL;
	}
}

int ApnClient::init(int id, 
		const string& cert,
		const string& key,
		const string& pass,
		int reconnectspan,
		int max_que_size){
	m_id = id;
	m_max_que_size = max_que_size;
	m_reconnect_span = reconnectspan;
	m_ctx = SSL_CTX_new(SSLv23_client_method());
	if(!m_ctx){
		logError("ApnClient") << id << ": SSL_CTX_new fail";
		return -1;
	}


	SSL_CTX_set_default_passwd_cb_userdata(m_ctx, (void*)pass.data());
	if(!SSL_CTX_use_certificate_file(m_ctx, cert.data(), SSL_FILETYPE_PEM)){
		logError("ApnClient") << id << ": SSL_CTX_use_certificate_file fail, "
			"cert: " << cert 
			<< ", error: "<< ERR_reason_error_string(ERR_get_error())
			;
		return -4;
	}
	
	if(!SSL_CTX_use_PrivateKey_file(m_ctx, key.data(), SSL_FILETYPE_PEM)){
		logError("ApnClient") << id << ": SSL_CTX_use_PrivateKey_file fail, "
			"key: " << key 
			<< ", error: "<< ERR_reason_error_string(ERR_get_error())
			;
		return -5;
	}
	
	

	if(!SSL_CTX_check_private_key(m_ctx)){

		logError("ApnClient") << id << ": SSL_CTX_check_private_key fail, "
			"cert:" << cert << ", key: " << key 
			<< ", error: "<< ERR_reason_error_string(ERR_get_error())
			;
	}
	

	return 0;	
}


int ApnClient::connectServer(const string &addr, int port){
	TimeRecorder t(__FUNCTION__);
	int ret = 0;
	m_addr = addr;
	m_port = port;
	m_fd = tcpConnect(const_cast<char*>(m_addr.c_str()), m_port, NULL, 0);
	if(m_fd < 0){
		logError("ApnClient") << m_id << ": BIO_new_ssl_connect fail, addr:"
			<< m_addr << ", port:" << m_port
			;
		return -1;
	}
	
	// qh add
	setSocketNonblocking(m_fd);
	if (!m_ssl) {
		m_ssl = SSL_new(m_ctx);
		if(!m_ssl){
			logError("ApnClient") << ": SSL_new fail";
			return -3;
		}
	}

	if(!SSL_set_fd(m_ssl, m_fd)){
		logError("ApnClient") << m_id << ": SSL_set_fd fail, addr:"
			<< m_addr << ", port:" << m_port
			<< ", error: "<< ERR_reason_error_string(ERR_get_error()) 
			;
		close(m_fd);
		m_fd = -1;
		return -2;
	}

		
	if (SSL_connect(m_ssl) <= 0) {
		ret = tryConnectServer();
	}
	
	m_last_connect_time = time(NULL);
	logError("ApnClient") << m_id << ": SSL_connect success, addr:"
		<< m_addr << ", port:" << m_port;
	return ret;
}

int ApnClient::tryConnectServer(){
// tring connect to SSL server
	int ret = 0;
	int loop = 5;
	int success = 0;
	
	while(loop-- && !success) {
		int epofd = epoll_create(16);
		if (epofd < 0) {
			continue;
		}
		struct epoll_event evt, evts[16];
		evt.data.fd = m_fd;
		evt.events = EPOLLIN;
		if (epoll_ctl(epofd, EPOLL_CTL_ADD, m_fd, &evt) < 0) {
			close(epofd);
			continue;
		}
		int nfds = epoll_wait(epofd, evts, 16, 1000);
		if (nfds == -1) {
			close(epofd);
			continue;
		}
		for (int i = 0; i < nfds; i++) {
			if (evts[i].data.fd != m_fd) continue;
			if (evts[i].events & EPOLLIN) {
				ret = SSL_connect(m_ssl);
				if (ret <= 0) {		
					int sslerr = SSL_get_error(m_ssl, ret);
					logError("ApnClient") << m_id << " SSL error:" << sslerr << ".retrying...";
				} else {
					success = 1;
				}
			}
		}
		close(epofd);
	}
	if (!success) {
		closeServerConnection();
		logError("ApnClient") << m_id << ": can't connect to SSL server!" << "addr: " <<
			m_addr << ",port: " << m_port;
		return -1;
	}


	return 0;
}

int ApnClient::reconnectServer(){
	closeServerConnection();
	int ret = 0;
	logError("ApnClient") << "addr: " << m_addr << ",port: " << m_port;
	ret = connectServer(m_addr, m_port);
	return ret;
}


int ApnClient::closeServerConnection(){

	if(m_fd >= 0){
		SSL_shutdown(m_ssl);
		close(m_fd);
		m_fd = -1;
		SSL_free(m_ssl);
		m_ssl = NULL;
	}

	return 0;
}

int ApnClient::checkConnection(){
	TimeRecorder t(__FUNCTION__);
	if(m_fd < 0){
		if (reconnectServer() < 0) return -1;
	}
	string buf;
	int rcvnum = 1024; 
	sockSetBlocking(m_fd, 0);
	buf.resize(rcvnum);
	int ret = SSL_read(m_ssl, const_cast<char*>(buf.data()), buf.size());
	sockSetBlocking(m_fd, 1);
	if(ret <= 0 && SSL_get_error(m_ssl, ret) != SSL_ERROR_ZERO_RETURN){
		return 0;
	}
	//connection close
	//get fail start id, clear message before it, 
	//and resend message from it
	int id = *(int*)(buf.data() + 2);
	clearSendedMessage(id, (int)buf[1]);	
	if (reconnectServer() < 0) return -1;
	resendMessages();
	return 0;
}

inline bool idIsOld(int id, int cmp_id){
	uint uid = *(uint*)&id;
	uint ucmp_id = *(uint*)&cmp_id;
	if((uid <= ucmp_id && (ucmp_id - uid) < (0x7FFFFFFF))
		|| (uid > ucmp_id && (uid - ucmp_id) > (0x7FFFFFFF)))
	{
		return true;
	}

	return false;
}


int ApnClient::clearSendedMessage(int endid, int status){
	MsgQue::iterator itor = m_msgs.begin();
	for(; itor != m_msgs.end(); itor = m_msgs.begin()){
		ApnMessage& m = *itor;
		if(m.id == endid){
			logError("ApnClient") << "<action:invalid_msg> <id:" 
				<< endid << "> <token:" << m.token
				<< "> <status:" << status << ">";
		}
		if(idIsOld(m.id, endid)){
			m_msgs.erase(itor);
			--m_que_size;
		}else{
			break;
		}
	}
	
	return 0;
}

int ApnClient::resendMessages()
{
	TimeRecorder t(__FUNCTION__);
	MsgQue::iterator itor = m_msgs.begin();
	for(; itor != m_msgs.end(); ++itor){
		sendMessage(*itor);	
	}
	return 0;
}


int ApnClient::sendMessage(const ApnMessage& msg){
	TimeRecorder t(__FUNCTION__);
	if(m_fd < 0){
		logError("ApnClient") << "<action:send> <status:fail> <token:"
			<< msg.token << "> <sn:" << msg.sn << "> <id:" << msg.id << "> <expiry:"
			<< msg.expiry << "> <error:invalid_socket>";
		return -1;
	}
	
	string buf;
	//see http://www.oschina.net/question/861681_111082
	buf.resize(1 + 4 + 4 + 2 + msg.token.size() + 2 + msg.data.size());		
	char* ptr = const_cast<char*>(buf.data());
	*(ptr) = 1;
	ptr += 1;
	*(int*)ptr = msg.id;
	ptr += 4;
	*(int*)ptr = htonl(msg.expiry);
	ptr += 4;
	string tk;
	hexToBytes(msg.token, tk);
	*(int16*)ptr = htons((int16)tk.size());
	ptr += 2;
	memmove(ptr, msg.token.data(), tk.size());
	ptr += msg.token.size();
	*(int16*)ptr = htons((int16)msg.data.size());
	ptr += 2;
	
	memmove(ptr, msg.data.data(), msg.data.size());
	int ret = SSL_write(m_ssl, buf.data(), buf.size());
	if(ret > 0){
	/*	logError("ApnSn") << "<connection:" << hex << this << "> <addr:" << m_addr 
 			<< "> <port:" << m_port << "> <action:send> <status:success> <token:" 
			<< tk << "> <sn:" << msg.sn << "> <id:" << msg.id << "> <expiry:" 
			<< msg.expiry << "> <data:" << msg.data << ">"; */
	}else{
	/*	logError("ApnSn") << "<connection:" << hex << this << "> <addr:" << m_addr 
 			<< "> <port:" << m_port << "> <action:send> <status:fail> <token:" 
			<< tk << "> <sn:" << msg.sn << "> <id:" << msg.id << "> <expiry:" 
			<< msg.expiry << "> <data:" << msg.data << "> <error:"
			<< ERR_error_string(SSL_get_error(m_ssl, ret), NULL)
			<< ">"; */
		closeServerConnection();
	}
	
	return ret;
}

void ApnClient::constructMessage(const string& token, const string &sn,
		int expiry, const string& msg,
		ApnMessage& m){
	++m_msg_id;
	m.id = m_msg_id;
	m.expiry = expiry;
	m.token = token;
	m.data = msg;
	m.sn = sn;

}

int ApnClient::addMessage(const ApnMessage& m){
	m_msgs.push_back(m);
	++m_que_size;
	if(m_que_size < m_max_que_size){
		m_msgs.pop_front();
	}
	return 0;
}

int ApnClient::pushMessage(const string& token, const string &sn, 
		int expiry, const string& data){
	TimeRecorder t(__FUNCTION__);
	if(token.size() != TOKEN_SIZE || data.size() > MAX_APN_DATA_SIZE){
		logError("ApnClient") << "token:" << token 
			<< ", sn:" << sn << ", PARAMETER_ERROR!"; 
		return PARAMETER_ERROR;
	}

	if (checkConnection() < 0) return -1;
	time_t n = time(NULL);
	if(n - m_last_connect_time >= m_reconnect_span){
		if (reconnectServer() < 0) return -1;
	}
	ApnMessage msg;
	constructMessage(token, sn, expiry, data, msg);
/*
	logError("ApnClient") << "<action:add> <status:success> <token:" 
	<< tk << "> <sn:" << sn << "> <id:" << msg.id << "> <expiry:" 
	<< msg.expiry << ">"; */
	int ret = 0;
	ret = sendMessage(msg);
	if(ret > 0)
		addMessage(msg);
	return ret;
}


};
