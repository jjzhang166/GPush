#include "client.h"
#include "ops.h"
#include <ef_utility.h>
#include "eventloop.h"

namespace gim
{
	Client::Client()
	:m_keepalive_timeout(KEEPALIVE_TIMEOUT),
	 m_evlp(NULL)
	 {
		 m_evlp = new EventLoop();
	}
	Client::~Client()
	{
		if(m_evlp){
			delete m_evlp;
			m_evlp = NULL;
		}
	}
	void Client::setKeepaliveTimeout(int second){
		m_keepalive_timeout = second;
	}
	
	void Client::setPublicKeyFile(const std::string& keyfile){
		m_pubkeyfile = keyfile;
	}
	int Client::init()
	{
		m_evlp->setMsgCb(eventLoopMsgRoutine, (void*)this);
		m_evlp->startLoop();
		return 0;
	}

	int Client::login(const std::string& srvip, int srvport, const std::string& cid, const std::string& version, const std::string& token)
	{
		LoginOp* op = new LoginOp(cid);
		op->init(srvip, srvport, version, m_keepalive_timeout, m_pubkeyfile, token);
		return m_evlp->asynAddOp((Op*)op);
	}
	int Client::stop()
	{
		m_evlp->asynStop();
		return 0;
	}
	int Client::disconnect(const std::string& cid)
	{
		DisconnectOp* op = new DisconnectOp(cid);
		return m_evlp->asynAddOp((Op*)op);
	}

	int Client::eventLoopMsgRoutine(void* cli, const std::string& msg)
	{
		return cli ? ((Client*)cli)->handleMessage(msg) : -1;
	}
	int Client::handleMessage(const std::string& msg)
	{
		return 0;
	}
}
