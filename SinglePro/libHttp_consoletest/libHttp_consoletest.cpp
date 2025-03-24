// libHttp_consoletest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "HttpHelper.h"

bool SyncHttpRequest(const std::wstring& strUrl, std::string& strResponse)
{
	if (strUrl.empty()) return false;

	std::vector<CString> vecHeaders;
	std::wstring strAppId = L"devAppId:6686088440bb48cd19f3f245";
	vecHeaders.push_back(strAppId.c_str());
	//std::wstring strAppId = UtilsString::FormatString(L"devAppId:%s", m_strDevAppId.c_str());
	//if (!m_strDevAppId.empty())
	//	vecHeaders.push_back(strAppId.c_str());

	//std::wstring strBuId = UtilsString::FormatString(L"buId:%s", m_strBuId.c_str());
	//if (!m_strBuId.empty())
	//	vecHeaders.push_back(strBuId.c_str());

	//std::wstring strRealm = UtilsString::FormatString(L"realm:%s", m_strRealm.c_str());
	//if (!m_strRealm.empty())
	//	vecHeaders.push_back(strRealm.c_str());

	//std::wstring strToken = UtilsString::FormatString(L"token:%s", m_strToken.c_str());
	//if (!m_strToken.empty())
	//	vecHeaders.push_back(strToken.c_str());

	std::string strRequestBody;
	//only test
	if (!CLibHttpHelper::HttpRequest(strUrl.c_str(), strResponse, true, _T("application/oct-stream"), vecHeaders, strRequestBody))
	{
		//LOG_ERROR(L"request failed, url:%s", strUrl.c_str());
		return false;
	}

	return 0;
}


int main()
{
	std::wstring url = L"https://gms-test.service.lenovo.com/game/api/getClassifyList";
	std::string strRes;
//	SyncHttpRequest(url, strRes);

	url = L"https://gms-test.service.lenovo.com/game/api/getClassifyDatas?classifyId=185&page=1&pageSize=50";
	SyncHttpRequest(url, strRes);

    std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
