#ifndef __RSA_SERVICE_H__
#define __RSA_SERVICE_H__

#include "base/ef_rsa.h"
#include "base/ef_tsd_ptr.h"

namespace gim{

class RSAService{
public:
	int init(const std::string& publicpem, const std::string& privatepem);
	int publicKeyEncrypt(const std::string& in, std::string& out);
	int privateKeyDecrypt(const std::string& in, std::string& out);	
	int privateKeyEncrypt(const std::string& in, std::string& out);
	int publicKeyDecrypt(const std::string& in, std::string& out);	
	ef::RSAEnt* getEntry();
private:
	std::string m_publicpem;
	std::string m_privatepem;
	ef::TSDPtr<ef::RSAEnt> m_rsas;	
};

};


#endif/*__RSA_SERVICE_H__*/
