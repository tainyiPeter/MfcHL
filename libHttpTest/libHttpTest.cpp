//#define WIN32_LEAN_AND_MEAN 
//#include <winsock2.h>        // Include Winsock2 before Windows.h
//#include <windows.h>         // Include Windows.h
//#include "curl/curl.h"

#include "CurlHelper.h"
#include "UtilsString.h"

#include <iostream>
#include <string>
using namespace std;

#pragma comment(lib, "SElibcurl.lib")
#ifdef _DEBUG
#pragma comment(lib, "libcurld.lib")
#else
#pragma comment(lib, "libcurl.lib")
#endif


//#ifdef _WIN32
//	#ifdef _DEBUG
//	#pragma comment(lib, "Win32/libcurld.lib")
//	#else
//	#pragma comment(lib, "Win32/libcurl.lib")
//	#endif
//#else  //_WIN64
//	#ifdef _DEBUG
//	#pragma comment(lib, "Win64/libcurld.lib")
//	#else
//	#pragma comment(lib, "Win64/libcurl.lib")
//	#endif
//#endif

int main()
{
	CString url = "https://cloud-pay.mbgtest.lenovomm.com/cloud-auth/oauth/token";
	int32_t ret = CCurlHelper::Test(url);

	std::string strTmp = UtilsString::FormatString("%s===%s", "abc", "def");

	return 0;
}