#include "apn_push_server.h"
#include "json/json.h"
#include "base/ef_base64.h"
#include "base/ef_hex.h"
#include "base/ef_log.h"
#include "base/ef_utility.h"
#include "base/ef_statistic.h"
#include "apn_conn.h"
#include <sstream>

namespace gim{

using namespace std;
using namespace Json;
using namespace ef;


static int32 constructPacket(const head& h,
	const std::string& body, std::string& respbuf){
	head rh;
	respbuf.reserve(sizeof(h) + body.size());
	rh.cmd = htonl(h.cmd + 1);
	rh.magic = h.magic;
	rh.len = htonl(sizeof(rh) + body.size());
	respbuf.append((char*)&rh, sizeof(rh));
	respbuf.append(body);
	return 0;
}

int GConnection::handlePacket(const string& req){

	cout << "GConnection::handlePacket" << std::endl;

	int ret = 0;

	if(m_type == BIN_TYPE){
		head h;
		h = *(head*)req.data();
		h.cmd = htonl(h.cmd);
		h.len = htonl(h.len);
		h.magic = htonl(h.magic);

		switch(h.cmd){
		case JSON_PUSH_REQ:
			ret = handleCommand(req.substr(sizeof(h)));
			break;
		default:
			break;	
		}
	}else{
		ret = handleCommand(req);
	}

	return ret;
}

int GConnection::sendJsonResponse(const Value& v){

	string msg;
	string body = v.toStyledString();
	
	if(m_type == BIN_TYPE){
		head resph;
		resph.cmd = JSON_PUSH_RESP;
		resph.magic = MAGIC_NUMBER;

		constructPacket(resph, body, msg);
	}else{

		string heads = m_httph.VERSION + " 200 OK\r\n"
			"Content-Length:" + itostr(body.size()) +"\r\n"
			"Content-Type:text/plain\r\n\r\n";
		msg = heads + body;	

	}

	return sendMessage(msg);
		
}

int GConnection::handleCommand(const string& req){

	int ret = 0;

	Value v;

	Reader r;
	Value respv;

	respv["status"] = 0;

	if(!r.parse(req, v)){
		respv["status"] = -1;
		respv["message"] = "parse req fail";
		sendJsonResponse(respv);
		logInfo("ApnPush") << "<action:handleCommand> <req:"<< req 
			<< "> <resp:" << respv.toStyledString() << ">";
		return -1;
	}

	const string& cmd = v["cmd"].asString();

	if(cmd == "push_request"){
		ret = handlePushRequest(v, respv);
	}else{
		respv["status"] = -2;
		respv["message"] = "unknow command";
	}


	sendJsonResponse(respv);
	logInfo("ApnPush") << "<action:sendJsonResponse> <req:" << req << "> <resp:" 
		<< respv.toStyledString() << ">";
	return 0;
}


int32 GConnection::handlePushRequest(const Value& reqv, Value& respv){
	int32 ret = 0;

	if(objectValue != reqv.type()){
		logInfo("ApnPush") << "invalid type, INPUT_FORMAT_ERROR";
		ret = INPUT_FORMAT_ERROR;
		return ret;
	}

	std::string token;
	Value tkv = reqv.get("token", "");
	if(tkv.type() == stringValue){
		token = tkv.asString();
	}

	int32 expriy = 0;
	Value expv = reqv.get("expiry", 0);
	if(expv.type() == intValue || expv.type() == uintValue){
		expriy = expv.asInt();
	}else if(expv.type() == stringValue){
		expriy = atoi(expv.asString().data());
	}
	std::string payloadb64; 
	Value dtv = reqv.get("payload", "");
	if(dtv.type() == stringValue){
		payloadb64 = dtv.asString();
	}
	std::string payload = base64Decode(payloadb64);

	std::string sn;
	Value snv = reqv.get("sn", "");
	if (snv.type() == stringValue) {
		sn = snv.asString();
	}
	ApnClient* c = APNConn::get();
	if(c){
		ret = c->pushMessage(token, sn, expriy, payload);
		if(ret > 0){
			logError("ApnPush") << "<thread:" << hex << this 
			<< "> <addr:" << getIp() << "> <port:" << dec 
			<< getPort() << "> <action:send> <status:success> <token:" 
			<< token << "> <sn:" << sn << "> <expiry:" 
			<< expriy << "> <payload:" << payload << ">"; 
			ret = 0;
		}else{
			logError("ApnPush") << "<thread:" << hex << this 
			<< "> <addr:" << getIp() << "> <port:" << dec 
			<< getPort() << "> <action:send> <status:fail> <token:" 
			<< token << "> <sn:" << sn << "> <expiry:" 
			<< expriy << "> <payload:" << payload << ">"; 
			ret = SEND_FAIL; 	
			respv["status"] = SEND_FAIL;
			respv["message"] = "send fail!";
		}
	}else{
		logError("ApnPush") << "GET_PUSH_CLIENT_FAIL";
		respv["status"] = GET_PUSH_CLIENT_FAIL;
		respv["message"] = "GET_PUSH_CLIENT_FAIL!";
	}
	
	return ret;
}

int32   GConnection::onCreate(EventLoop* l){
	l->addNotify(this, READ_EVENT);
	if(m_alive_ms){
		startTimer(CHECK_TIMER, m_alive_ms);
	}
	return 0;
}


int32   GConnection::handleTimer(EventLoop* l, int32 id){
	time_t n = time(NULL);
	switch(id){
	case CHECK_TIMER:

		logTrace("ApnPush") << "con:" << getId() << " check timer!";
		if(n - m_last_time > m_alive_ms/1000){
			recycle();
			return 0;
		}
		startTimer(CHECK_TIMER, m_alive_ms);
		break;
	}

	return  0;
}





int GConnection::parseBinHead(){
	int ret = 0;
	head h;
	peekBuf((char*)&h, sizeof(head));
	h.magic = htonl(h.magic);
	h.len = htonl(h.len);

	if(h.len < (int)sizeof(h) 
		|| h.len > 1024 * 1024){
		ret = -1;
	} else if(h.len <= bufLen()){
		ret = h.len;
	}

	if(ret <= 0 && bufLen() > 0){
		logError("ApnPush") << "<action:client_check_pack> " 
			"<event_loop:" << getEventLoop() << "> <conid:" 
			<< getId() << "> <buflen:" << bufLen() 
			<< "> <status:packet_not_full>";
	}

	return 0;

}

int GConnection::parseHttpHead(){
	size_t pos = m_httph.head.find("\r\n\r\n");

	if(pos == string::npos){

		int p = searchInBuf("\r\n\r\n", 4);

		if(p < 0){
			return 0;
		}
	
		//append head		
		string s;
		s.resize(p + 4);
		readBuf((char*)s.data(), s.size());	
		m_httph.head += s;	
	} 

	if(!m_httph.VERSION.size()){
		size_t pos = m_httph.head.find("\r\n");

		string mline = m_httph.head.substr(0, pos);

		pos = mline.find("HTTP");
		if(pos == string::npos){
			return 0;
		}	

		m_httph.VERSION = mline.substr(pos);	

	}

	if(m_bodylen < 0){
		size_t pos = m_httph.head.find("Content-Length:");
		if(pos == string::npos){
			return 0;
		}

		//find after "Content-Length:"
		pos = m_httph.head.find_first_not_of(" \t", pos + 15);	
		
		if(pos == string::npos){
			return 0;
		}

		m_bodylen = atoi(m_httph.head.data() + pos);
	}

	if(m_bodylen <= bufLen()){
		return m_bodylen;	
	}

	//cout << "httphead:" << m_httph.head 
	//	<< ", VERSION:" << m_httph.VERSION 
	//	<< ", bodylen:" << m_bodylen << endl;

	return 0;
		
}

int GConnection::parseHead(){
	int ret = 0;

	if(m_type == UNKNOW_TYPE){
		if(bufLen() < (int)sizeof(head)){
			return 0;
		}
		string hs;
		hs.resize(sizeof(head));
		peekBuf((char*)hs.data(), hs.size());
		if(hs.find("GET") == 0 || hs.find("POST") == 0){
			m_type = HTTP_TYPE;
		}else{
			m_type= BIN_TYPE;
			m_binh = *(head*)hs.data();
			m_binh.magic = htonl(m_binh.magic);
			m_binh.len = htonl(m_binh.len);
			m_binh.cmd = htonl(m_binh.cmd);
			m_bodylen = m_binh.len - sizeof(m_binh);
			return m_binh.len;
		}
	}

	if(m_type == BIN_TYPE){
		ret = parseBinHead();
	}else if(m_type == HTTP_TYPE){
		ret = parseHttpHead();
	}


	return ret;

}

int GConnection::checkPackLen(){

	int ret = parseHead();

	return ret;
}



}

