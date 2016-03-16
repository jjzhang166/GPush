#include "err_no.h"
#include "msg_head.h"
#include "listen_conn.h"
#include "push_server.h"
#include "gpush.pb.h"
#include "logic_log.h"
#include "sess_cache.h"
#include "server_conn.h"
#include "server_manager.h"
#include "base/ef_base64.h"
#include <iostream>

namespace gim{

using namespace std;


int genSessID(int svid, int conid,
	const std::string& n, std::string& dn){

	std::string buf;
	buf.resize(8);

	char* p = (char*)buf.data();

	*(int32*)p = svid;
	*(int32*)(p + sizeof(int32)) = conid;

	dn = ef::base64Encode(buf) + n;
	return 0;
}

ListenCon::~ListenCon(){
}


int ListenCon::getSvID() const{
	return m_serv->getConfig().ID;
}

int ListenCon::handlePacket(const string& req){

	std::cout << "ListenCon::handlePacket" << std::endl;

	int ret = 0;

	if(m_type == BIN_TYPE){
		head h;
		h = *(head*)req.data();
		h.cmd = htonl(h.cmd);
		h.len = htonl(h.len);
		h.magic = htonl(h.magic);

		switch(h.cmd){
		case JSON_PUSH_REQ:
			ret = handlePushReqeust(req.substr(sizeof(h)));
			break;
		default:
			break;	
		}
	}else{
		ret = handlePushReqeust(req);
	}

	return ret;
}

int ListenCon::sendJsonResponse(const Json::Value& v){

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

int ListenCon::handlePushReqeust(const string& req){

	Json::Value v;

	Json::Reader r;
	Json::Value respv;

	respv["status"] = 0;

	if(!r.parse(req, v)){
		respv["status"] = -1;
		respv["message"] = "parse req fail";
		sendJsonResponse(respv);
		logicInfo << "<action:handlePushReqeust> <req:"<< req 
			<< "> <resp:" << respv.toStyledString() << ">";
		return -1;
	}

	respv["sn"] = v["sn"];

	PushRequest preq;
	preq.set_type(v["type"].asInt());
	preq.set_payload(v["payload"].asString());
	preq.set_sn(v["sn"].asString());

	SessCache* s = SessCacheService::instance()->getSessCache();
	if (!s) {
		respv["status"] = -2;
		respv["message"] = "get sess cache fail";
		sendJsonResponse(respv);
		logicInfo << "<action:sendJsonResponse> <req:" << req 
			<< "> <resp:" << respv.toStyledString() << ">";
		return -2;
	}

	if(!v["to_id"].isString()){
		respv["status"] = -4;
		respv["message"] = "to_id is empty";
		sendJsonResponse(respv);
		logicInfo << "<action:sendJsonResponse> <req:" << req 
			<< "> <resp:" << respv.toStyledString() << ">";
		return -4;
	}

	std::string id = v["to_id"].asString();

	std::vector<Sess> sv;
	s->getSession(id, sv);
	if (sv.empty()) {
		respv["status"] = -3;
		respv["message"] = "user offline";
		sendJsonResponse(respv);
		logicInfo << "<action:sendJsonResponse> <req:" << req 
			<< "> <resp:" << respv.toStyledString() << ">";
		return -3;
	}

	int count = 0;
	for (unsigned int n = 0; n<sv.size(); n++){
		Sess&  ss = sv[n];
		ss.consvid();

		preq.set_to_sessid(ss.sessid());

		int ret = sendToServer(ss.consvid(), preq);
		if (ret >= 0){
			++count;
		}

		logicInfo << "<action:sendJ2C> <sn:" << preq.sn() << "> <to_sessid:" << ss.sessid() 
			<< "> <id:" << id << "> <type:" << preq.type() << "> <ret:" << ret
			<< "> <req_payload:" << preq.payload() << ">";
	}

	respv["count"] = count;
	sendJsonResponse(respv);
	logicInfo << "<action:sendJsonResponse> <req:" << req << "> <resp:" 
		<< respv.toStyledString() << ">";
	return count;
}

int ListenCon::sendToServer(int svid, const PushRequest& preq){
	Svlist* m = (Svlist*)getEventLoop()->getObj();

	SvCon* s = m->getServer(svid);

	if(!s){
		return -1;
	}

	return s->sendPushRequest(preq);
}

int ListenCon::parseBinHead(){
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
		logicError << "<action:client_check_pack> " 
			"<event_loop:" << getEventLoop() << "> <conid:" 
			<< getId() << "> <buflen:" << bufLen() 
			<< "> <status:packet_not_full>";
	}

	return 0;

}

int ListenCon::parseHttpHead(){
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

	//std::cout << "httphead:" << m_httph.head 
	//	<< ", VERSION:" << m_httph.VERSION 
	//	<< ", bodylen:" << m_bodylen << std::endl;

	return 0;
		
}

int ListenCon::parseHead(){
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

int ListenCon::checkPackLen(){

	int ret = parseHead();

	return ret;
}

int ListenCon::onCreate(ef::EventLoop* l){
	addNotify(l, READ_EVENT);
	return 0;
}

Connection* LConFactory::createConnection(EventLoop* l,
	int32 fd, const std::string& addr,int32 port){
	ListenCon* c = new ListenCon();
	return c;
}

}
