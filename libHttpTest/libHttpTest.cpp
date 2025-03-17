//#define WIN32_LEAN_AND_MEAN 
//#include <winsock2.h>        // Include Winsock2 before Windows.h
//#include <windows.h>         // Include Windows.h
//#include "curl/curl.h"

#include "CurlHelper.h"
#include "UtilsString.h"
#include "UtilsTools.h"
#include "OpenSSLHelp.h"

#include <iostream>
#include <string>
using namespace std;

static const std::string appId = "1593389727517312";

#pragma comment(lib, "SElibcurl.lib")

#ifdef _DEBUG
#pragma comment(lib, "libcurld.lib")
#pragma comment(lib, "libUtility.lib")
#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "libssl.lib")

#else
#pragma comment(lib, "libcurl.lib")
#endif

//std::string CreateLzSign(const std::vector<std::tuple<std::string, std::string>>& vecData)
//{
//	std::string strContent;
//	std::sort(vecData.begin(), vecData.end(), [](auto first, auto& second)
//		{
//			return std::get<0>(first).compare(std::get<0>(second)) < 0;
//		});
//
//	uint32_t idx = 0;
//	for (auto iter : vecData)
//	{
//		strContent += std::get<1>(iter);
//		++idx;
//		if (idx < vecData.size())
//		{
//			strContent += "&";
//		}
//	}
//
//	return strContent;
//}

bool SyncHttpRequestLz(const std::wstring& strUrl, std::string& strResponse)
{
	//std::string reqUrl = "https://cloud-pay.mbgtest.lenovomm.com/cloud-legionzone/api/v1/getClassifyList";

	//std::vector<std::tuple<std::string, std::string>> vecParam =
	//{
	//	std::make_tuple("appId", appId),
	//	std::make_tuple("nonce", UtilsTools::GetUUID()),
	//	std::make_tuple("sign_type", "RSA2"),
	//	std::make_tuple("timestamp", UtilsString::ConvertUint64ToString(UtilsTools::GetTimestampMs())),
	//};

	//std::string strSign = CreateLzSign(vecParam);
	//strSign = UtilsTools::UrlEncode(strSign);
	//vecParam.push_back(std::make_tuple("sign", strSign));

	//reqUrl += "?";
	//size_t size = vecParam.size();
	//int idx = 0;
	//for (auto iter : vecParam)
	//{
	//	reqUrl += std::get<0>(iter);
	//	reqUrl += "=";
	//	reqUrl += std::get<1>(iter);
	//	if (++idx < size)
	//	{
	//		reqUrl += "&";
	//	}
	//}
	////LOG_INFO(L"url123:%s", CString(reqUrl.c_str()));
	//std::string strRequestBody;
	//std::vector<CString> vecHeaders;
	////if (!CLibHttpHelper::HttpRequest(reqUrl.c_str(), strResponse, true, "application/x-www-form-urlencoded"))
	////{
	////	//LOG_ERROR(L"request failed, url:%s", reqUrl.c_str());
	////	return false;
	////}

	return true;
}

void test_sha()
{
	// 输入数据
	const char* data = "abcdefg";
	size_t data_len = strlen(data);

	std::string sign = OpenSSLHelp::PrikeySign(data, false);

	cout << sign.c_str() << endl;

	sign = OpenSSLHelp::PrikeySign(data, true);

	cout << sign.c_str() << endl;

	return;

	// 私钥路径
	const char* private_key_path = "D:/mywork/MfcHL/libProject/UtilityLib/private.pem";

	// 签名缓冲区
	unsigned char signature[256];
	size_t signature_len = sizeof(signature);

	// 计算签名
	if (OpenSSLHelp::sha256_rsa_sign(private_key_path, (unsigned char*)data, data_len, signature, &signature_len)) 
	{
		printf("Signature generated successfully!\n");
		printf("Signature (hex): ");
		for (size_t i = 0; i < signature_len; i++) {
			printf("%02x", signature[i]);
		}
		printf("\n");
	}
	else {
		fprintf(stderr, "Failed to generate signature\n");
	}
}

void test_dlfile()
{

	std::string response;
	long sCode = 0;
	CString localPath = "d:\\tmp123";
	//CString url = "https://cloud-pay.mbgtest.lenovomm.com/cloud-auth/oauth/token";
	//int32_t ret = CCurlHelper::Test(url);

	//CString url_get = "https://cloud-pay.mbgtest.lenovomm.com/cloud-legionzone/api/v1/getClassifyDatas?appId=1593389727517312&nonce=64d0d0bb-f739-4e5f-b7c3-29ab5cd61769&sign_type=RSA2&timestamp=1742115996000&classifyId=304&page=1&pageSize=10&sign=MWT4eICeVnaqblgA%2BlSVifv7eEOOOfwq4slgxMMoa69T112i133cXsVGCXmGnpLGo5Sqn%2FM%2B5VeMXHJA17%2Ffl5tKpE9oLV3szjKL%2B6TEnKFxX9hxMkgwKch3aevNUMPm3IBrfS95Rfkp6MQImm4w5mp9GRfchUirRMmMpd9%2FMn0%3D";
	//int32_t ret = CCurlHelper::HttpGet(url_get, response, sCode);

	//cout << "rsp:" << response.c_str() << endl;
	//cout << " code:" << sCode << endl;

	//下载测试
	CString url = "https://smartdl.lenovo.com.cn/lzupdate/forhome/LZLiteSetup.exe";
	url = "https://guanjia.lenovo.com.cn/download/lenovopcmanager_apps.exe";
	//int32_t ret = CCurlHelper::DownloadFile(url, localPath);

	int32_t ret = CCurlHelper::DownloadFile(url, localPath, TRUE, [](int64_t done, int64_t total, double speed)
		{
			cout << "done:" << done << " total:" << total << " speed:" << speed << endl;
		});

	cout << "finish ..." << endl;

	//int32_t ret = CCurlHelper::TestDLFile(url, localPath);
}

//测试lz授权
void test_lz_auth()
{
	std::string strResponse;
	long status = 0;
	//授权测试
	CString url = "https://cloud-pay.mbgtest.lenovomm.com/cloud-auth/oauth/token";

	std::vector<std::tuple<std::string, std::string>> vecData =
	{
		std::make_tuple("grant_type", "client_credentials"),
		std::make_tuple("client_id", "1593389727517312"),
		std::make_tuple("client_secret", "d248f803532a4d009c4bdecfb5bcd5cc")
	};
	std::string strBody = UtilsString::BuildHttpBody(vecData);
	std::string strContent = "Content-Type: application/x-www-form-urlencoded";
	int ret = CCurlHelper::HttpPost(url, strResponse, strBody, status, strContent);

	cout << "rsp:" << strResponse.c_str() << endl;
	cout << "ret:" << ret << " status:"<<status << endl;
}

//获取列表
void test_lz_getlist()
{
	std::string url = "https://cloud-pay.mbgtest.lenovomm.com/cloud-legionzone/api/v1/getClassifyList";

	std::vector<std::tuple<std::string, std::string>> vecParam =
	{
		std::make_tuple("appId", appId),
		std::make_tuple("nonce", UtilsTools::GetUUID()),
		std::make_tuple("sign_type", "RSA2"),
		std::make_tuple("timestamp", UtilsString::ConvertUint64ToString(UtilsTools::GetTimestampMs())),
	};

	std::string strSign = OpenSSLHelp::LzParamSign(vecParam);
	strSign = UtilsTools::UrlEncode(strSign);
	vecParam.push_back(std::make_tuple("sign", strSign));

	url += "?";
	size_t size = vecParam.size();
	int idx = 0;
	for (auto iter : vecParam)
	{
		url += std::get<0>(iter);
		url += "=";
		url += std::get<1>(iter);
		if (++idx < size)
		{
			url += "&";
		}
	}
	std::string strResponse;
	long status = 0;
	int ret = CCurlHelper::HttpGet(url.c_str(), strResponse, status);

	cout << "rsp:" << strResponse.c_str() << endl;
	cout << "ret:" << ret << " status:" << status << endl;
}

//获取素材
void test_lz_getdata(uint32_t cID, uint32_t pageIdx, uint32_t pageSize)
{
	std::string url = "https://cloud-pay.mbgtest.lenovomm.com/cloud-legionzone/api/v1/getClassifyDatas";
	std::vector<std::tuple<std::string, std::string>> vecParam =
	{
		std::make_tuple("appId", appId),
		std::make_tuple("nonce", UtilsTools::GetUUID()),
		std::make_tuple("sign_type", "RSA2"),
		std::make_tuple("timestamp", UtilsString::ConvertUint64ToString(UtilsTools::GetTimestampMs())),
		std::make_tuple("classifyId", UtilsString::ConvertUint64ToString(cID)),
		std::make_tuple("page", UtilsString::ConvertUint64ToString(pageIdx)),
		std::make_tuple("pageSize", UtilsString::ConvertUint64ToString(pageSize)),
	};
	std::string strSign = OpenSSLHelp::LzParamSign(vecParam);
	strSign = UtilsTools::UrlEncode(strSign);
	vecParam.push_back(std::make_tuple("sign", strSign));

	url += "?";
	size_t size = vecParam.size();
	int idx = 0;
	for (auto iter : vecParam)
	{
		url += std::get<0>(iter);
		url += "=";
		url += std::get<1>(iter);
		if (++idx < size)
		{
			url += "&";
		}
	}
	std::string strResponse;
	long status = 0;
	int ret = CCurlHelper::HttpGet(url.c_str(), strResponse, status);

	cout << "rsp:" << strResponse.c_str() << endl;
	cout << "ret:" << ret << " status:" << status << endl;
}

int main()
{
	//int CCurlHelper::HttpGet(CString url, string & response, long& statusCode, int timeout)

	//std::string strTmp = UtilsString::FormatString("%s===%s", "abc", "def");
	//test_sha();

	//test_lz_getlist();
	test_lz_getdata(303, 1, 10);

	cout << "main exit" << endl;
	return 0;
}