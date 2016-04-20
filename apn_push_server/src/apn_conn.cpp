/**
* @file db_conn.cpp
* @author zhangyongqiang 
* @date 2014/04/18
* @brief
*/

#include "settings.h"
#include "base/ef_tsd_ptr.h"
#include "base/ef_log.h"
#include "apn_conn.h"
#include "apn_client.h"
#include <stdexcept>


namespace gim{

TSDPtr<ApnClient> g_ApnConn;

APNConn::APNConn()
{
}

APNConn::~APNConn()
{
}

ApnClient*
APNConn::get()
{
	ApnClient* pDBC = g_ApnConn.get();

	if (!pDBC) {
		Settings *pSettings = Singleton<Settings>::instance();
		pDBC = new ApnClient();
		pDBC->init(pSettings->Id, pSettings->APNSCert,
			pSettings->APNSKey, pSettings->APNSPass);
		pDBC->connectServer(pSettings->APNSAddr, pSettings->APNSPort);
		g_ApnConn.set(pDBC);
	}

	return pDBC;

}

}

