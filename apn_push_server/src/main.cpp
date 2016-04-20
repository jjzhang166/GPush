#include "net/ef_server.h"
#include "net/ef_net_log.h"
#include "base/ef_deamonize.h"
#include "base/ef_statistic.h"
#include "log_init.h"
#include <stdlib.h>
#include <iostream>
#include <signal.h>
#include <getopt.h>
#include "apn_push_server.h"
#include "settings.h"

using namespace ef;
using namespace gim;

Server* g_pdb = NULL;
int	g_run = 1;

int system_shutdown( void )
{
	printf("apn_push_server_shutdown!\n");

	g_run = false;
	printf("now cleanup...\n");

	return 0;
}


static void signal_handler(int sig)
{

	switch(sig) {
	case SIGHUP:
	case SIGTERM:
		printf("terminate signal catched, now exiting...");		
		system_shutdown();
		break;
	}

}

static int output_statistic(void* p, const std::string& l){
	logError("ApnStatistic") << l;	
	return 0;
}

static void printHelpInfo(){
	std::cout << 	"-h				help\n"
			"-d				daemon\n"
			"-c	<config>		config\n"
	
		<< std::endl;

}



int main(int argc, char* const* argv){
	
	const char* short_options = "hdc:s:";

	const struct option long_options[] = {
		{  "help",      0,   NULL,   'h'  },
		{  "daemon",      0,   NULL,   'd'  },
		{  "config",    1,   NULL,   'c'  },
		{  NULL,      0,    NULL,   0  }
	};

	int c;

	std::string type;
	std::string config;
	std::string spath;
	bool dm = false;

	while((c = getopt_long (argc, argv, short_options, long_options, NULL)) != -1){
		switch(c){
		case 'h':
			printHelpInfo();
			return 0;

		case 'd':
			dm = true;
			break;

		case 'c':
			config = optarg;
			break;		
		
		}			
	}

	if(!config.size()){
		printHelpInfo();
		return -1;
	}
	


	Settings *pSettings = Singleton<Settings>::instance();

	if (!pSettings->load(config.data())) {
		std::cout << "load setting file:" << config 
			<< ", fail\n";
		return 1;
	}	

	if(dm){
		ef::daemonize();
	}

	signal(SIGPIPE, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGTSTP, SIG_IGN); 
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGHUP,  SIG_IGN);
	signal(SIGQUIT,  SIG_IGN);
	signal(SIGURG,  SIG_IGN);
	signal(SIGTERM, signal_handler);

	if(pSettings->Daemon){
		std::cout << "Daemon!\n";
		daemonize();
	}

	setNetLogName("ApnNet");	
	setNetLogLevel(pSettings->NLogLevel);
	setNetLogPath(pSettings->NLogPath);
	logInit(pSettings->LogConfig);
	initStatistic(output_statistic, NULL);
	ApnClient::initApnClientModule();	
	Server s;
	s.setEventLoopCount(pSettings->ThreadCount);
	int32 connectionAliveTime = 10000;
	s.init();
	if(s.startListen(pSettings->ListenPort, new GConnectionFactory(&s, 
		connectionAliveTime)) < 0){
		std::cout << "Listen fail, port:" << pSettings->ListenPort
			<< std::endl;
		return 1;
	}
	s.run();

	while(g_run){
		printf("g_run....\n");
		sleep(1);
	}

	s.clear();

	return 0;
}
