/**
* @file settings.cpp
* @author zhangyq
* @date 2014/04/16
* @brief
*/

#include <json/json.h>
#include <fstream>
#include "settings.h"
#include "base/ef_log.h"

namespace gim{

using namespace std;
using namespace ef;

Settings::Settings()
{
	init();
}

bool
Settings::init()
{
	ListenIp = "0.0.0.0";
	ListenPort = 2100;

	ThreadCount = 6;

	Daemon = false;
	
	return true;
}


bool
Settings::load(const char *filename)
{
	fstream ifs;

	ifs.open(filename, ios::in);

	if(!ifs.is_open()){
		return -1;
	}

	Json::Reader reader;
	Json::Value root;

	if (!reader.parse(ifs, root, false)) {
		return -2;
	}

	if (root["ListenIp"].isString()) {
		ListenIp = root["ListenIp"].asString();
	} else {
		return -3;
	}


	if (root["ListenPort"].isInt()) {
		ListenPort = root["ListenPort"].asInt();
	} else {
		return -4;
	}


	if (root["ThreadCount"].isInt()) {
		ThreadCount = root["ThreadCount"].asInt();
	} else {
		return -5;
	}

	if (root["LogConfig"].isString()) {
		LogConfig = root["LogConfig"].asString();
	} else {
		return -6;
	}

	if (root["NetLogLevel"].isString()) {
		NLogLevel = getStrLevel(root["NetLogLevel"].asString().data()); 
	} else {
		return -7;
	}

	if (root["NLogPath"].isString()) {
		NLogPath = root["NLogPath"].asString();
	} else {
		return -8;
	}

	if (root["Id"].isInt()) {
		Id = root["Id"].isInt();
	} else {
		return -10;
	}
	
	if (root["ReconnectSpan"].isInt()) {
		ReconnectSpan = root["ReconnectSpan"].isInt();
	} else {
		return -11;
	}

	if (root["APNSAddr"].isString()) {
		APNSAddr = root["APNSAddr"].asString();
	} else {
		return -12;
	}
		
	if (root["APNSCert"].isString()) {
		APNSCert = root["APNSCert"].asString();
	} else {
		return -13;
	}

	if (root["APNSKey"].isString()) {
		APNSKey = root["APNSKey"].asString();
	} else {
		return -14;
	}

	if (root["APNSPass"].isString()) {
		APNSPass = root["APNSPass"].asString();
	} else {
		return -15;
	}
	
	
	return true;
}

}

