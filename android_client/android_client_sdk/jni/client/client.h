#ifndef _CLIENT_H_
#define  _CLIENT_H_

#include <string>

namespace gim
{
	class EventLoop;
	class Client
	{
	public:
		Client();
		~Client();

		int init();
		void setKeepaliveTimeout(int second);
		void setPublicKeyFile(const std::string& keyfile);
		int login(const std::string& srvip, int srvport, const std::string& cid, const std::string& version, const std::string& token);
		int stop();
		int disconnect(const std::string& cid);
	public:
		virtual int handleMessage(const std::string& msg);
	private:
		static int eventLoopMsgRoutine(void* cli, const std::string& msg);
		EventLoop* m_evlp;
		int m_keepalive_timeout;
		std::string m_pubkeyfile;
	};
}

#endif //_CLIENT_H_
