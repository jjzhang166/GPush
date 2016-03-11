#include "rsa_service.h"


namespace gim{

using namespace std;
using namespace ef;

int RSAService::init(const std::string& publicpem, const std::string& privatepem){
	m_publicpem = publicpem;
	m_privatepem = privatepem;

	return 0;
}

RSAEnt* RSAService::getEntry(){
	RSAEnt* r = m_rsas.get();
	if(r){
		return r;
	}

	RSAEnt* e = new RSAEnt();
	if(e->initPublicKey(m_publicpem) < 0){
		delete e;
		return NULL;
	}

	if(e->initPrivateKey(m_privatepem) < 0){
		delete e;
		return NULL;
	}

	m_rsas.set(e);

	return e;
}


int RSAService::publicKeyEncrypt(const std::string& in, std::string& out){
	int ret = 0;

	RSAEnt* r = getEntry();

	if(!r){
		return -20001;
	}

	ret = r->encryptByPublicKey(in, out);	

	return ret;
}

int RSAService::privateKeyDecrypt(const std::string& in, std::string& out){
	int ret = 0;

	RSAEnt* r = getEntry();

	if(!r){
		return -20001;
	}

	ret = r->decryptByPrivateKey(in, out);	

	return ret;
}

int RSAService::privateKeyEncrypt(const std::string& in, std::string& out){
	int ret = 0;

	RSAEnt* r = getEntry();

	if(!r){
		return -20001;
	}

	ret = r->encryptByPrivateKey(in, out);	

	return ret;
}

int RSAService::publicKeyDecrypt(const std::string& in, std::string& out){
	int ret = 0;

	RSAEnt* r = getEntry();

	if(!r){
		return -20001;
	}

	ret = r->decryptByPublicKey(in, out);	

	return ret;
}

};
