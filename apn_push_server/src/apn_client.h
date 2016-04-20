#ifndef APN_CLIENT_H
#define APN_CLIENT_H


#include "base/ef_btype.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include <time.h>
#include <list>
#include <string>

namespace gim{


class ApnClient{
public:
	enum{
		APN_OK = 0,
		TOKEN_SIZE = 64,
		MAX_APN_DATA_SIZE = 256,
		PARAMETER_ERROR = -100,
		DEFAULT_MAX_QUE_SIZE = 128, 
		DEFAULT_RECONNECT_SPAN = 3600,
	};

	ApnClient();	

	~ApnClient();

	static int initApnClientModule();

	//every client has an unique id, for message retry
	int init(int id, 
		const std::string& cert,
		const std::string& key,
		const std::string& pass,
		int reconnetspan = DEFAULT_RECONNECT_SPAN,
		int maxquesize = DEFAULT_MAX_QUE_SIZE
		);

	int connectServer(const std::string &addr, int port);
	int pushMessage(const std::string& token, const std::string &sn, 
		int expire, const std::string& msg);
	int checkConnection();
private:
	int reconnectServer();
	typedef struct{
		int id;
		int expiry;
		std::string token;
		std::string data;
		std::string sn;
	}ApnMessage;

	int sendMessage(const ApnMessage& msg);
	int resendMessages();
	void constructMessage(const std::string& token, const std::string &sn,
		int expiry, const std::string& msg,
		ApnMessage& m);
	int addMessage(const ApnMessage& msg);
	int clearSendedMessage(int endid, int status);
	//keep the message
	int closeServerConnection();

	int tryConnectServer();

	typedef std::list<ApnMessage> MsgQue;
	MsgQue m_msgs;
	SSL_CTX* m_ctx;
	SSL* m_ssl;
	int m_fd;
	int m_id;
	int m_msg_id;
	int m_type;
	int m_que_size;
	int m_max_que_size;
	std::string m_addr;
	int m_port;
	time_t m_last_connect_time;
	int m_reconnect_span;
};

};


#endif/*APN_CLIENT_H*/
