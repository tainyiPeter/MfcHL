#include "OpenSSLHelp.h"
#include "openssl/evp.h"
#include "openssl/ossl_typ.h"
#include "openssl/rsa.h"
#include "openssl/pem.h"
#include "openssl/err.h"

#include <windows.h>
#include <algorithm>

//#pragma comment(lib,"ws2_32.lib")
#if _MSC_VER >= 1400 // VC++ 8.0
#pragma warning( disable : 4996 )   // disable warning about strdup being deprecated.
#endif


static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

// 打印错误信息
void print_errors() {
	unsigned long err;
	while ((err = ERR_get_error())) {
		char* str = ERR_error_string(err, NULL);
		fprintf(stderr, "Error: %s\n", str);
	}
}


// SHA256WithRSA 签名
int OpenSSLHelp::sha256_rsa_sign(const char* private_key_path, const unsigned char* data, size_t data_len, unsigned char* signature, size_t* signature_len)
{
	OpenSSL_add_all_algorithms();
	ERR_load_crypto_strings();

	FILE* fp = fopen(private_key_path, "rb");
	if (!fp) {
		fprintf(stderr, "Failed to open private key file\n");
		return 0;
	}

	// 读取私钥
	EVP_PKEY* pkey = PEM_read_PrivateKey(fp, NULL, NULL, NULL);
//	fclose(fp);
	if (!pkey) {
		fprintf(stderr, "Failed to read private key\n");
		print_errors();
		return 0;
	}

	// 创建签名上下文
	EVP_MD_CTX* ctx = EVP_MD_CTX_new();
	if (!ctx) {
		fprintf(stderr, "Failed to create context\n");
		print_errors();
		EVP_PKEY_free(pkey);
		return 0;
	}

	// 初始化签名操作
	if (EVP_DigestSignInit(ctx, NULL, EVP_sha256(), NULL, pkey) <= 0) {
		fprintf(stderr, "Failed to initialize signing\n");
		print_errors();
		EVP_MD_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		return 0;
	}

	// 更新数据
	if (EVP_DigestSignUpdate(ctx, data, data_len) <= 0) {
		fprintf(stderr, "Failed to update data\n");
		print_errors();
		EVP_MD_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		return 0;
	}

	// 计算签名
	if (EVP_DigestSignFinal(ctx, signature, signature_len) <= 0) {
		fprintf(stderr, "Failed to finalize signing\n");
		print_errors();
		EVP_MD_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		return 0;
	}

	// 释放资源
	EVP_MD_CTX_free(ctx);
	EVP_PKEY_free(pkey);

	return 1;
}

static std::string GetStrPrivateKeyTest()
{
	std::string strPrivateKey = "";
	// 测试  私钥
	strPrivateKey.append("-----BEGIN RSA PRIVATE KEY-----\n");
	//strPrivateKey.append("MIICeAIBADANBgkqhkiG9w0BAQEFAASCAmIwggJeAgEAAoGBAK7mzrWJ3r/gwPLg5oP3nN/9MgT/XW6NOsp/cXzugmeZysC6TJDUl5YMEbRxMzu33Se7Z9prrQEGzzYuQ7zxOVIBHp6Hqdfey1tla0hoZtJF3ctQZJKN8Xnc3OhnInc57J4t+K7PO8Xyp+156hE6VTgcXtIf1mzzrVBvvtI8mku5AgMBAAECgYBmA/Y56Srl3GBuykNqYfk8pOCSDakfnHzYNDGr3yps50Vd2MxkBbxNJskE6KorgPkb/bF7syZ73M7sGVwlTsZRWVPh4tXlFODnhdSrGBLSHBjH7+OGVRxojTlk+hMAzuOFeRUvRBoA/qUr3euyTikvkp1BHBbTp+4voOHHy5NyAQJBAOBsqiioZNNA5c2nOZX2bSUY8qcwLz9h+8LCpa3rawBbiw9TMcZkFhBYM5pCbkr3FPTI6NzuVBkaldbIqjRdGmECQQDHgmyodcARpgxguoBztCQKUIhK1X1hezde3ix3V+W7LbBH+drkKUNBD0VFdzqcJAJQVou/qLRydmPdMdXq8CBZAkEAuDHXsskK/6wrMJBInkGEMUd5mSjeDnqAJMiXLNikmMWJq8lPeBIbXczWhYxyc4q5o3LmVLYzRethAcjukU284QJBAIoTtPneDKDqR9TGHtJ1ESBmXf1UxeKqcUbiAupR1ff2hfM6dAjGmfo0BGAwRUr9DPDnBO8PZlt1nmXe8MsZh3kCQQC64eBD4ulHymMhhLxRIOo8N7luaw7t6MCAkfDcjVkEmmTDvauEc2QqtnlsYL1CWXDjWloNO89ppjDMbuYnWvin\n");
	strPrivateKey.append("MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBAKCRD8imb30x15xojnxkr3KKAX6aVBHBRUDMJ21znWSkpVYiGIoqdBlwHXmgyK1n6FCteduCgERjDfgOMY+hf5LBIJSCWh35gvhm0tg83TqWQF4J5zO9cim7vmZ03UeiMZ5tI9od8/wGFXsrwxMY895kt3N7EClVW4IzacAk/73pAgMBAAECgYEAmgFo3TBd4PvMbPWQLNm5uieKNW860WjJztPBwWZSAXU2K/46AVOk1b1GWCGSOVhfZ/pr1dI+ytkjTdf3S2dAXupusjJ+nDy/wFmtJ3GPvToC1beoq1WGLv0b7uCrx3FQAIMblnaEEISQ+FhrezWM3CUVcIPmOY82hmMbi6grxoECQQD3kXPSLq/NJenpwM3npKsEPbEHREcSS9mUioESEWKuRkD8l6mNgW0PVQz51Wuyh+LD5NSudG7zSp0CqfGhx1aZAkEApgkK7W0wEZLtuxgeym8T3a3in8jXKhYt7gkts2QDjt45MWfPlCnkCslznj+H9qgveQTN+qE8hogiewegyVhD0QJATCyIC6Lr2t6Qk6kMcfr71+k3gDnk+F84Snk3b5zD1UWul0n/FJg/O1l7DaA6bAtdndLaEfenb9qEZx5tR87y+QJAGfH1zC4wwb5E+eSGAzoiT5S5giB/RutaWAUFKDArwiqFo2vGay+0m97o+0SQOYIEkROoIE8LxzYEtVb20B/D0QJBAL7ya2yRBOU6Ei5Qe0vaDSiociECjv0qwoTU50w7UK6vIUAdRt7ol6DLfbqAiimSS+p+c8aHZcENqMvzJkGqXYs=\n");
	strPrivateKey.append("-----END RSA PRIVATE KEY-----\n");

	return strPrivateKey;
}

static inline char b64_to_safe(char c)
{
	switch (c)
	{
	case '+':
		return '-';
		break;
	case '/':
		return '_';
		break;
	default:
		return c;
	}
}


std::string OpenSSLHelp::urlsafe_base64_encode(const char* bytes_to_encode, unsigned int in_len)
{
	std::string b64_encode = base64_encode(bytes_to_encode, in_len);
	std::string::iterator it;
	for (it = b64_encode.begin(); it != b64_encode.end(); it++)
	{
		*it = b64_to_safe(*it);
	}
	return b64_encode;
}

std::string OpenSSLHelp::PrikeySign(const std::string& cipherText, bool urlEncode)
{
	std::string priKey = GetStrPrivateKeyTest();

	std::string strRet = "";
	RSA* rsa = NULL;// RSA_new();
	BIO* keybio;
	keybio = BIO_new_mem_buf((unsigned char*)priKey.c_str(), -1);

	rsa = PEM_read_bio_RSAPrivateKey(keybio, /*&rsa*/NULL, NULL, NULL);
	if (rsa == NULL) {
		return "";
	}


	unsigned char szSha256Data[SHA256_DIGEST_LENGTH + 1] = { 0 };
	ZeroMemory(szSha256Data, sizeof(szSha256Data));
	//memset(szSha256Data, 0, SHA256_DIGEST_LENGTH + 1);
	SHA256_CTX c;
	if (!SHA256_Init(&c))
	{
		return "";
	}
	SHA256_Update(&c, cipherText.c_str(), cipherText.length());
	SHA256_Final(szSha256Data, &c);
	OPENSSL_cleanse(&c, sizeof(c));

	unsigned int len = RSA_size(rsa);
	char* decryptedText = (char*)malloc(len + 1);
	memset(decryptedText, 0, len + 1);


	// 签名
	//int result = RSA_sign(NID_sha256, (BYTE*)cipherText.c_str(), cipherText.length(), (BYTE*)decryptedText, &len, rsa);

	int result = RSA_sign(NID_sha256, szSha256Data, SHA256_DIGEST_LENGTH, (BYTE*)decryptedText, &len, rsa);
	//判断是否需要使用url 安全的编码方式
	if (urlEncode)
	{
		strRet = urlsafe_base64_encode(decryptedText, len);
	}
	else
	{
		strRet = base64_encode(decryptedText, len);
	}

	//strRet = decryptedText;

	

	// 释放内存  
	free(decryptedText);
	BIO_free_all(keybio);
	RSA_free(rsa);

	return strRet;
}


std::string OpenSSLHelp::base64_encode(const char* bytes_to_encode, int in_len)
{
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--)
	{
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3)
		{
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++)
			{
				ret += base64_chars[char_array_4[i]];
			}
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
		{
			char_array_3[j] = '\0';
		}

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

		for (j = 0; (j < i + 1); j++)
		{
			ret += base64_chars[char_array_4[j]];
		}
		while ((i++ < 3))
		{
			ret += '=';
		}
	}

	return ret;
}

std::string OpenSSLHelp::LzParamSign(std::vector<std::tuple<std::string, std::string>>& vecData)
{
	std::string strContent;
	std::sort(vecData.begin(), vecData.end(), [](auto first, auto& second)
		{
			return std::get<0>(first).compare(std::get<0>(second)) < 0;
		});

	uint32_t idx = 0;
	for (auto iter : vecData)
	{
		strContent += std::get<0>(iter);
		strContent += "=";
		strContent += std::get<1>(iter);
		++idx;
		if (idx < vecData.size())
		{
			strContent += "&";
		}
	}

	strContent = PrikeySign(strContent, true);
	return strContent;
}