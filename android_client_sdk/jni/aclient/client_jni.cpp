#include "client_jni.h"
#include "androidclient.h"

#define C_STR(x) (gim::JstringToString(env, x).c_str())

#ifdef __cplusplus
extern "C" {
#endif
static gim::AndroidClient* s_cli = NULL;
JNIEXPORT jint JNICALL Java_com_gim_client_init(JNIEnv *env, jobject jcli,
	jobject jlstn, jstring pubkey) {
	if (s_cli == NULL) {
		s_cli = new gim::AndroidClient();
		if (s_cli->initJniEnv(env, jlstn) < 0)
			return -1;

		s_cli->init();
		s_cli->setPublicKeyFile(C_STR(pubkey));
		return 0;
	} else {
		return -1;
	}
}
JNIEXPORT jint JNICALL Java_com_gim_client_stop(JNIEnv *, jobject) {
	s_cli->stop();
	delete s_cli;
	s_cli = NULL;
	return 0;
}
JNIEXPORT jint JNICALL Java_com_gim_client_login(JNIEnv* env, jobject job,
	jstring jsrvip, jint srvport, jstring jcliver, jstring jcid, jstring jtoken) {
	return s_cli->login(C_STR(jsrvip), srvport, C_STR(jcid), C_STR(jcliver), C_STR(jtoken));
}
JNIEXPORT jint JNICALL Java_com_gim_client_logout(JNIEnv *env, jobject job,
		jstring jcid) {
	return s_cli->disconnect(C_STR(jcid));
}

JNIEXPORT jint JNICALL Java_com_gim_client_loglevel(JNIEnv *, jobject,
		jint lvl) {
	gim::AndroidClient::loglevel = lvl;
	return 0;
}

#ifdef __cplusplus
}
#endif
