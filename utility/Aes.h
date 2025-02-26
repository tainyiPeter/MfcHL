#pragma once

#include <windows.h>
#include <bcrypt.h>
#include <vector>
#include <iostream>
#include <string>


// 安全释放资源
template<typename T>
void SafeRelease(T*& p) 
{
	if (p != nullptr) 
	{
		BCryptCloseAlgorithmProvider(p, 0);
		p = nullptr;
	}
}

class Aes
{
public:
	static std::vector<BYTE> generate_random_bytes(size_t length);
	// AES-CBC 加密（返回加密后的数据）
	static std::vector<BYTE> encrypt_aes_cbc(const std::vector<BYTE>& plaintext, const std::vector<BYTE>& key, const std::vector<BYTE>& iv);
	// AES-CBC 解密（返回解密后的数据）
	static std::vector<BYTE> decrypt_aes_cbc(const std::vector<BYTE>& ciphertext, const std::vector<BYTE>& key, const std::vector<BYTE>& iv);
	//static std::string Encrypt(const std::string& strData, const std::string& strKey);
	//static std::string Decrypt(const std::string& strData, const std::string& strKey);
};