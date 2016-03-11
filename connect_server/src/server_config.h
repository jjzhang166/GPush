#ifndef __SERVER_CONFIG_H__
#define __SERVER_CONFIG_H__

#include <vector>
#include <json/json.h> 
#include "rsa_service.h"
#include "base/ef_auto_ptr.h"

namespace gim{

class CliConfig{
public:
	int Enc;
	int AliveMs;
	int MinType;
	int MaxType;
	int ListenPort;
	int MaxReqQueSize;
	int MaxPackCntPerMin;
	int StartThreadIdx;
	int ThreadCnt;
	ef::RefCntPtr<RSAService> RSAs;

	CliConfig(){
		Enc = 0;
		AliveMs = 0;
		MinType = 0;
		MaxType = 0;
		ListenPort = 0;
		MaxReqQueSize = 0;
		MaxPackCntPerMin = 0;
		StartThreadIdx = 0;
		ThreadCnt = 0;
	}

	CliConfig(const CliConfig& other){
		Enc = other.Enc;
		AliveMs = other.AliveMs;
		MinType = other.MinType;
		MaxType = other.MaxType;
		ListenPort = other.ListenPort;
		MaxReqQueSize = other.MaxReqQueSize;
		MaxPackCntPerMin = other.MaxPackCntPerMin;
		StartThreadIdx = other.StartThreadIdx;
		ThreadCnt = other.ThreadCnt;
		RSAs = other.RSAs;
	}

	CliConfig& operator = (const CliConfig& other){
		Enc = other.Enc;
		AliveMs = other.AliveMs;
		MinType = other.MinType;
		MaxType = other.MaxType;
		ListenPort = other.ListenPort;
		MaxReqQueSize = other.MaxReqQueSize;
		MaxPackCntPerMin = other.MaxPackCntPerMin;
		StartThreadIdx = other.StartThreadIdx;
		ThreadCnt = other.ThreadCnt;
		RSAs = other.RSAs;
		
		return *this;
	}

};


class ServerConfig{
public:
	int ID; 
	int ThreadCount;

	std::string ZkUrl;
	std::string StatusZkPath;

	int NetLogLevel;
	std::string NetLogPath;

	std::string LogConfig;

	Json::Value SessCacheConfig;

	std::string TokenKeyPath;

	std::vector<CliConfig> CliConfigs;

	ServerConfig():ID(0), ThreadCount(0){
	}

	int init(const std::string& f);

	int serializeToJson(Json::Value& v) const;
	int parseFromJson(const Json::Value& v);

	std::string toStyledString() const;

private:
	int loadPortsConfigs(const Json::Value& v);
	
};

}

#endif
