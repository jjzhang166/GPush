#include "client.h"
#include "err_no.h"
#include <vector>

namespace gim {
    typedef int (*CALLBACKTYPE)(std::string); //回调函数
    extern "C" void setCallBackFun(CALLBACKTYPE getDataByType);
class IosClient: public Client {
public:
	static int loglevel; //0 no, 1=all, 2 error
public:
	IosClient();
	~IosClient() {
	}
    int setCallBackFun(CALLBACKTYPE getDataByType);
	virtual int handleMessage(const std::string& msg);
private:
    CALLBACKTYPE m_getDataByType;
};

}
;

