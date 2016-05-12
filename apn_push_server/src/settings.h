/**
* @file settings.h
* @brief
*/

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "base/ef_singleton.h"
#include <string>


namespace gim{

class Settings
{
public:
	Settings();
	int init();
	int load(const char *filename);

	void print() const;
public:
	std::string ListenIp;
	int ListenPort;

	int ThreadCount;

	int Daemon;
	int Id;

	int NLogLevel;
	std::string NLogPath;
	std::string LogConfig;

	int ReconnectSpan;
	std::string APNSAddr;
	int APNSPort;
	std::string APNSCert;
	std::string APNSKey;
	std::string APNSPass;
	
};

}

#endif

