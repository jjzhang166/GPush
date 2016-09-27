#include "ef_aes.h"
#include "ef_md5.h"
#include "ef_hex.h"
#include "rijndael-api-fst.h"
#include <iostream>

namespace ef{
	int32 aesEncrypt(const std::string& src, 
		const std::string& key, std::string& dst){
		return aesEncrypt(src.data(), src.size(), key, dst);
	}

	int32 aesEncrypt(const char* src, size_t len,
				const std::string& key, std::string& dst){
		int ret = 0;
		keyInstance keyist;
		u8 keymd5[16] = {0};
		u8 keymat[65] = {0};
		MD5(keymd5, (const u8*)key.data(), key.size());
		bytesToHexs((const char*)keymd5, 16, (char*)keymat, 32);
		std::cout << "key:" << keymat << std::endl;
		ret = makeKey(&keyist, DIR_ENCRYPT, 128, keymat);
		if(ret < 0){
			std::cout << "makeKey fail\n";
			return ret;
		}

		cipherInstance cnst;
		ret = cipherInit(&cnst, MODE_ECB, (char*)keymat);
		if(ret < 0){
			std::cout << "cipherInit fail\n";
			return ret;
		}

		int pkcs = 16 - (len % 16);
		std::cout << "mdskey:" << keymat << "pkcs:" << pkcs << std::endl;
		dst.resize(len + 16 - len % 16);
		BYTE* encout = (BYTE*)dst.data();
		ret = padEncrypt(&cnst, &keyist, (BYTE*)src, len, encout);
		if(ret < 0)
			return ret;
		std::cout << "ret:" << ret << std::endl;
		dst.resize(ret);
		return ret;
	}

	int32 aesDecrypt(const char* src, size_t len,
				const std::string& key, std::string& dst){
		int ret = 0;
		keyInstance keyist;
		u8 keymd5[16];
		u8 keymat[65] = {0};
		MD5(keymd5, (const u8*)key.data(), key.size());
		bytesToHexs((const char*)keymd5, 16, (char*)keymat, 32);
		std::cout << "key:" << keymat << std::endl;
		ret = makeKey(&keyist, DIR_DECRYPT, 128, keymat);
		if(ret < 0){
			return ret;
		}

		std::cout << "mdskey:" << keymat << std::endl;
		cipherInstance cnst;
		ret = cipherInit(&cnst, MODE_ECB, (char*)keymat);
		if(ret < 0){
			return ret;
		}
		std::cout << "len:" << len << std::endl;
		if (len < 16 || len % 16) {
			return -10;
		}

		dst.resize(len);
		BYTE* encout = (BYTE*)dst.data();
		ret = padDecrypt(&cnst, &keyist, (BYTE*)src, len, encout);
		if(ret < 0)
			return ret;
		dst.resize(ret);
		return ret;
	}



	int32 aesDecrypt(const std::string& src, 
			const std::string& key, std::string& dst){
		return aesDecrypt(src.data(), src.size(), key, dst);
	}

	
};
