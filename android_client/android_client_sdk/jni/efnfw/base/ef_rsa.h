#ifndef __EF_RSA_H__ 
#define __EF_RSA_H__ 

#include <string>

struct rsa_st;
typedef struct rsa_st RSA;

namespace ef {


enum{
	STATUS_OK = 0,
	OPEN_FILE_FAIL = -1,
	READ_PUBLIC_KEY_FAIL = -2,
	PUBLIC_ENC_FAIL = -3,
	PRIVATE_DEC_FAIL = -4,
	PRIVATE_ENC_FAIL = -5,
	PUBLIC_DEC_FAIL = -6,
};

class RSAEnt{
public:
	RSAEnt():m_pubrsa(NULL),m_prirsa(NULL){}
	~RSAEnt();

	int initPublicKey(const std::string& pubkeyfile);
	int initPrivateKey(const std::string& prikeyfile);
	int encryptByPublicKey(const std::string &text, std::string &enctext);
	int decryptByPrivateKey(const std::string &enctext, std::string &text);
	int encryptByPrivateKey(const std::string &text, std::string &enctext);
	int decryptByPublicKey(const std::string &enctext, std::string &text);
private:
	RSA* m_pubrsa;
	RSA* m_prirsa;
};

};
#endif
