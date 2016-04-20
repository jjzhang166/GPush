#ifndef __PUSH_CONN_H__ 
#define __PUSH_CONN_H__ 

#include <json/json.h>
#include "net/ef_server.h"
#include "head/msg_head.h"
#include "apn_client.h"

namespace gim{


enum{
	PUSH_OK = 0,
	INPUT_FORMAT_ERROR = ApnClient::PARAMETER_ERROR,
	GET_PUSH_CLIENT_FAIL = INPUT_FORMAT_ERROR + 1,
	SEND_FAIL = GET_PUSH_CLIENT_FAIL + 1,
};

class GConnection:public ef::Connection
{
public:
	enum{
		CHECK_TIMER = 1,
	};

	GConnection(ef::Server* s, int32 alive_ms)
		:m_type(UNKNOW_TYPE), m_bodylen(-1), m_serv(s), 
		m_last_time(0), m_alive_ms(alive_ms){
	}

	~GConnection(){
	}

	virtual int32   onCreate(EventLoop* l);

	virtual int32   handleTimer(EventLoop* l, int32 id);

	virtual int32   handlePacket(const std::string& req);

	// < 0, error, = 0, wait more, > 0, recv whole pack
	virtual int32   checkPackLen();

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


	int handlePushRequest(const Json::Value& req, Json::Value& resp);
	int handleCommand(const std::string& req);
	int sendJsonResponse(const Json::Value& v);
	int parseHead();
	int parseBinHead();
	int parseHttpHead();


	Server* m_serv;
	time_t m_last_time;
	int32 m_alive_ms;
};

class   GConnectionFactory:public ef::ConnectionFactory{
public:
	GConnectionFactory(ef::Server* s, int32 connectionAlivems)
		:m_serv(s), m_con_alive_ms(connectionAlivems){

	}

	~GConnectionFactory(){

	}

	virtual ef::Connection* createConnection(ef::EventLoop*, int32, 
		const std::string&, int32){
		ef::Connection* c = new GConnection(m_serv, m_con_alive_ms);
		return c;
	}

private:
	ef::Server* m_serv;
	int32 m_con_alive_ms;
};



}

#endif/*__PUSH_CONN_H__*/
