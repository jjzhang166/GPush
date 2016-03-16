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

void *testfun(void *arg)
{
	int ret = 0;
	string svreqbuf;
	SOCKET sockfd;
	string sendbuf;
	head rh;
	head h2;

	Json::Value jvresp;
	string recvbuf;
	sockfd = tcpConnect(ip.data(), port, NULL, 0);
	if (sockfd <= 0) {
		return NULL;
	}

	Json::Value jvreq;
	jvreq["to_id"] = id;
	jvreq["sn"] = sn;
	jvreq["payload"] = payload;
	jvreq["type"] = type;
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
}
	
int main(int argc, char **argv)
{
	if(argc < 8){
		cout << "PushTest <ip> <port> <client_id> <loop_count> <type> <sn> <payload>\n";
		return 0;
	}


	ip = argv[1];
	port = atoi(argv[2]);
	id = argv[3];
	int cnt = atoi(argv[4]);
	type = atoi(argv[5]);
	sn = argv[6];
	payload = argv[7];	

	for(int i = 0; i < cnt; ++i){
		testfun(NULL);
	}
	return 0;
}
