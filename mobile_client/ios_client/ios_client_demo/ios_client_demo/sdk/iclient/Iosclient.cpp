#include "Iosclient.h"
#include "err_no.h"
#include "client_log.h"

namespace gim {
int IosClient::loglevel = 2;
void logprint(LogLevel lvl, const char* logbuf) {
	if (logbuf) {
//		int aloglvl =
//				(LOG_LEVEL_ERROR == lvl) ? ANDROID_LOG_ERROR : ANDROID_LOG_INFO;
//		if (AndroidClient::loglevel == 1) // print all
//				{
//			__android_log_print(aloglvl, "clientsdk", "%s", logbuf);
//		} else if (AndroidClient::loglevel == 2 && (LOG_LEVEL_ERROR == lvl)) {
//			__android_log_print(aloglvl, "clientsdk", "%s", logbuf);
//		}
	}
}

IosClient::IosClient() :
		m_getDataByType(NULL)
{
	setLogFp(logprint);
}
int IosClient::handleMessage(const std::string& json) {
    m_getDataByType(json);
	return 0;
}

int IosClient::setCallBackFun(CALLBACKTYPE getDataByType) {
	if (NULL == getDataByType) {
		SDK_LOG(LOG_LEVEL_ERROR,
				"handlePushMessageReq find class RTClistener failed!");
		return GIM_JNI_ERROR;
	}
    m_getDataByType=getDataByType;
	return 0;
}
}
;

