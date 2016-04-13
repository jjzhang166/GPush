#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include "net/ef_sock.h"
#include "msg_head.h"
#include "json/json.h"
#include "base/ef_utility.h"
#include "base/ef_log.h"

using namespace ef;
using namespace gim;
using namespace std;

std::string ip;
int port = 0;
int type = 0;
std::string sn;
std::string id;
std::string payload;


class Client{
public:

	~Client(){
		if(sockfd >= 0){
			closesocket(sockfd);
			sockfd = -1;
		}
	}

	int connectServer(const std::string& ip, int port){
		sockfd = tcpConnect(ip.data(), port, NULL, 0);
		return sockfd;
	}	

	int doRequest(const Json::Value& jvreq){
		int ret = 0;
		string svreqbuf;
		string sendbuf;
		head rh;
		head h2;

		string recvbuf;

		svreqbuf = jvreq.toStyledString();
		rh.cmd = htonl(JSON_PUSH_REQ);
		rh.magic = 20150624;
		rh.len = htonl(sizeof(head) + svreqbuf.size());
		sendbuf.append((char *)&rh, sizeof(rh));
		sendbuf.append(svreqbuf);
		tcpSend(sockfd, sendbuf.data(), sendbuf.size(), 1000, NULL);
		cout << "send request!" << std::endl;
		char buf1[1024] = {0};
		ret = recv(sockfd, buf1, 1024, 0);
		h2 = *(head *)buf1;
		h2.cmd = htonl(h2.cmd);
		h2.len = htonl(h2.len);
		recvbuf.append(buf1 + sizeof(h2), h2.len - sizeof(h2));
		cout << recvbuf << endl;
		
		return ret;
	}


private:
	SOCKET sockfd;
};

void *testfun(void *arg)
{
}
	
int main(int argc, char **argv)
{
	if(argc < 6){
		cout << "PushTest <ip> <port> <loop_count> push_requst <user_id> <type> <sn> <payload>\n"
			"				user_status_request <user_id>\n";
		return 0;
	}

	Client c;

	ip = argv[1];
	port = atoi(argv[2]);
	int cnt = atoi(argv[3]);
	std::string cmd = argv[4];
	id = argv[5];

	Json::Value jvreq;
	if(cmd == "push_request"){
		type = atoi(argv[6]);
		sn = argv[7];
		payload = argv[8];	
		jvreq["cmd"] = cmd;
		jvreq["to_id"] = id;
                jvreq["sn"] = sn;
                jvreq["payload"] = payload;
                jvreq["type"] = type;
	}else if(cmd == "user_status_request"){
		jvreq["cmd"] = cmd;
		jvreq["user_id"] = id;
	}	


	int ret = c.connectServer(ip, port);

	if(ret < 0){
		cout << "connect push_server fail!" << endl;
		return 0;
	}

	for(int i = 0; i < cnt; ++i){
		c.doRequest(jvreq);
	}

	return 0;
}
