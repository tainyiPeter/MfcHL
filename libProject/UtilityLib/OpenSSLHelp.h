#pragma once

#include <string>
#include <vector>
#include <tuple>

class OpenSSLHelp
{
public:
	static int sha256_rsa_sign(const char* private_key_path, const unsigned char* data, size_t data_len, unsigned char* signature, size_t* signature_len);
	static std::string PrikeySign(const std::string& cipherText, bool urlEncode = false);
	static std::string base64_encode(const char* bytes_to_encode, int in_len);
	static std::string urlsafe_base64_encode(const char* bytes_to_encode, unsigned int in_len);
	/** Legion Zone ²ÎÊıÇ©Ãû
	*/
	static std::string LzParamSign(std::vector<std::tuple<std::string, std::string>>& vecData);
};