/**
* @file db_conn.h
* @author zhangyongqiang 
* @date 2013/07/28
* @brief
*/

#ifndef _DB_CONN_H_
#define _DB_CONN_H_
#include "base/ef_no_copy.h"
#include "apn_client.h"
#include <vector>
using namespace ef;

namespace gim{

class APNConn : NoCopy
{
	APNConn();
public:
	~APNConn();

	static ApnClient *get();
private:
};

}

#endif

