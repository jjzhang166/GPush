#ifndef __LISTEN_CONN_H__
#define __LISTEN_CONN_H__


#include "logic_common.h"
#include "net/ef_connection.h"
#include "net/ef_acceptor.h"
#include "msg_head.h"
#include <json/json.h>

namespace gim{

class PushServer;
class PushRequest;

class ListenCon:public Connection 
{
public:

	ListenCon():m_type(UNKNOW_TYPE), m_bodylen(-1), m_serv(NULL){
	}

	virtual	~ListenCon();

	int getSvID() const;

	void setPushServer(PushServer* s){
		m_serv = s;
	}
	
	PushServer* getPushServer(){
		return m_serv;
	}

	virtual int handlePacket(const std::string& req);

	// < 0, error, = 0, wait more, > 0, recv whole pack
	virtual int checkPackLen();

	virtual int onCreate(ef::EventLoop* l);
protected:

private:

	struct httpHead{
		std::string METHOD;
		std::string VERSION;
		std::string head;

	};


	enum{
		UNKNOW_TYPE = 0,
		BIN_TYPE = 1,
		HTTP_TYPE = 2,
	};

	int m_type;//0:unknow, 1:bin, 2:http
	httpHead m_httph;
	head m_binh;

	int m_bodylen;

	int handleCommand(const std::string& req);
	int handlePushReqeust(const Json::Value& v, Json::Value& resp);
	int handleUserStatusRequest(const Json::Value& v, Json::Value& resp);
	int handleReqeust(const std::string& req);
	int sendJsonResponse(const Json::Value& v);
	int sendToServer(int svid, const PushRequest& req);
	int parseHead();
	int parseBinHead();
	int parseHttpHead();

	PushServer* m_serv;
};



class LConFactory:public ConnectionFactory{
public:
	LConFactory(PushServer* s = NULL):m_s(s){
	}

	virtual ~LConFactory(){};
	virtual Connection* createConnection(EventLoop* l,
		int32 fd, const std::string& addr,int32 port);

private:
	PushServer* m_s;	
};

}

#endif/*__LISTEN_CONN_H__*/
