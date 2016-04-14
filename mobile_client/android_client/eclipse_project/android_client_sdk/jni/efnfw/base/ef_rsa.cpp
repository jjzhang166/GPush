#include "ef_rsa.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <arpa/inet.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

namespace ef{

RSAEnt::~RSAEnt(){
	if(m_pubrsa){
		RSA_free(m_pubrsa);
		m_pubrsa = NULL;
	}
	if(m_prirsa){
		RSA_free(m_prirsa);	
		m_prirsa = NULL;
	}
}

int RSAEnt::initPublicKey(const std::string& pubkeyfile){
	FILE* f = fopen(pubkeyfile.data(), "r");
	if(!f){
		return OPEN_FILE_FAIL;
	}
	
	m_pubrsa = PEM_read_RSA_PUBKEY(f, NULL, NULL, NULL);
	if(!m_pubrsa){
		fclose(f);
		return READ_PUBLIC_KEY_FAIL;		
	}


	return 0;
}

int RSAEnt::initPrivateKey(const std::string& pubkeyfile){
	FILE* f = fopen(pubkeyfile.data(), "r");
	if(!f){
		return OPEN_FILE_FAIL;
	}
	
	m_prirsa = PEM_read_RSAPrivateKey(f, NULL, NULL, NULL);
	if(!m_prirsa){
		fclose(f);
		return READ_PUBLIC_KEY_FAIL;		
	}


	return 0;
}

int RSAEnt::encryptByPublicKey(const std::string &text, std::string &enctext){
	int ret = 0;
	int tlen = text.size();
	int pos = 0;

	int l = htonl(tlen);
	enctext.append((char*)&l, sizeof(l));
	
	while(pos < tlen){

		int rsa_len = RSA_size(m_pubrsa);
		enctext.append(rsa_len, 0);
	
		unsigned char* str = (unsigned char*)text.data() + pos;
		unsigned char* p_en = (unsigned char*)enctext.data() + pos + sizeof(tlen);

		ret = RSA_public_encrypt(rsa_len, str, p_en, m_pubrsa, RSA_NO_PADDING);
		if(ret < 0){
			ERR_print_errors_fp(stdout);
			return PUBLIC_ENC_FAIL;	
		}
	
		pos += rsa_len;
	}

	return 0;
}

int RSAEnt::decryptByPrivateKey(const std::string &enctext, std::string &text){

	int tlen = ntohl(*(int*)enctext.data());

	int pos = 0;

	while(pos < tlen){

		int rsa_len = RSA_size(m_prirsa);
		text.append(rsa_len, 0);
		
		unsigned char* str = (unsigned char*)enctext.data() + sizeof(tlen) + pos;
		unsigned char* p_de = (unsigned char*)text.data() + pos;

		int ret = RSA_private_decrypt(rsa_len, str, p_de, m_prirsa, RSA_NO_PADDING);
		if(ret < 0){
			ERR_print_errors_fp(stdout);
			return PRIVATE_DEC_FAIL;	
		}

		pos += rsa_len;
	}
	text.resize(tlen);

	return 0;

}

int RSAEnt::encryptByPrivateKey(const std::string &text, std::string &enctext){
	int ret = 0;
	int tlen = text.size();
	int pos = 0;

	int l = htonl(tlen);
	enctext.append((char*)&l, sizeof(l));
	
	while(pos < tlen){

		int rsa_len = RSA_size(m_prirsa);
		enctext.append(rsa_len, 0);
	
		unsigned char* str = (unsigned char*)text.data() + pos;
		unsigned char* p_en = (unsigned char*)enctext.data() + pos + sizeof(tlen);

		ret = RSA_private_encrypt(rsa_len, str, p_en, m_prirsa, RSA_NO_PADDING);
		if(ret < 0){
			ERR_print_errors_fp(stdout);
			return PRIVATE_ENC_FAIL;	
		}
	
		pos += rsa_len;
	}

	return 0;
}

int RSAEnt::decryptByPublicKey(const std::string &enctext, std::string &text){

	int tlen = ntohl(*(int*)enctext.data());

	int pos = 0;

	while(pos < tlen){

		int rsa_len = RSA_size(m_pubrsa);
		text.append(rsa_len, 0);
		
		unsigned char* str = (unsigned char*)enctext.data() + sizeof(tlen) + pos;
		unsigned char* p_de = (unsigned char*)text.data() + pos;

		int ret = RSA_public_decrypt(rsa_len, str, p_de, m_pubrsa, RSA_NO_PADDING);
		if(ret < 0){
			ERR_print_errors_fp(stdout);
			return PUBLIC_DEC_FAIL;	
		}

		pos += rsa_len;
	}
	text.resize(tlen);

	return 0;

}


}
