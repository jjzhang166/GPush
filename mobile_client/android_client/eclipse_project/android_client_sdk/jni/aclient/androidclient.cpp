#include "androidclient.h"
#include <jni.h>
#include <android/log.h>
#include "err_no.h"
#include "client_log.h"
#include <android/log.h>
#include <client_log.h>

namespace gim {
int AndroidClient::loglevel = 2;
void logprint(LogLevel lvl, const char* logbuf) {
	if (logbuf) {
		int aloglvl =
				(LOG_LEVEL_ERROR == lvl) ? ANDROID_LOG_ERROR : ANDROID_LOG_INFO;
		if (AndroidClient::loglevel == 1) // print all
				{
			__android_log_print(aloglvl, "clientsdk", "%s", logbuf);
		} else if (AndroidClient::loglevel == 2 && (LOG_LEVEL_ERROR == lvl)) {
			__android_log_print(aloglvl, "clientsdk", "%s", logbuf);
		}
	}
}

AndroidClient::AndroidClient() :
		m_jvm(NULL),
		m_env(NULL),
		m_invokeEnv(NULL),
		m_jcLstn(NULL),
		m_jobLstn(NULL),
		m_pGarbage(NULL)
{
	setLogFp(logprint);
}
int AndroidClient::handleMessage(const std::string& json) {
	std::string msg = json;
	jstring jstr = jniInvokeEnv()->NewStringUTF(msg.c_str());
	jniInvokeEnv()->CallVoidMethod(m_jobLstn, m_hmidHandleMessage, jstr);
	if (jniInvokeEnv()->ExceptionCheck()) {
		//SDK_LOG(LOG_LEVEL_TRACE, "ExceptionCheck");
		jniInvokeEnv()->ExceptionDescribe();
		jniInvokeEnv()->ExceptionClear();
	}
	jniInvokeEnv()->DeleteLocalRef(jstr);
	detachJavaThrd();
	return 0;
}

int AndroidClient::initJniEnv(JNIEnv* env, jobject objLstn) {
	m_env = env;
	m_pGarbage = new JObjGarbage(m_env, false);

	m_jobLstn = env->NewGlobalRef(objLstn);
	m_pGarbage->push(m_jobLstn);

	m_env->GetJavaVM(&m_jvm);

	m_jcLstn = m_env->FindClass(JNI_LISTENER_PATH);
	if (NULL == m_jcLstn) {
		SDK_LOG(LOG_LEVEL_ERROR,
				"handlePushMessageReq find class RTClistener failed!");
		return GIM_JNI_ERROR;
	}
	m_pGarbage->push(m_jcLstn);

	m_hmidHandleMessage = m_env->GetMethodID(m_jcLstn, "handleMessage",
			"(Ljava/lang/String;)V");
	if (NULL == m_hmidHandleMessage) {
		SDK_LOG(LOG_LEVEL_ERROR, "get jni method handleMessage fail!");
		return GIM_JNI_ERROR;
	}

	return 0;
}
}
;

