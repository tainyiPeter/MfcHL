#include "UtilsTools.h"
#include "UtilsString.h"
#include <functional>
#include <sstream>
#include <shlwapi.h>
#include <array>
#include <set>
#include <time.h>
#include <lm.h>

#pragma comment(lib, "Netapi32.lib")
#pragma comment(lib, "Rpcrt4.lib")


uint64_t UtilsTools::GetTimestampMs()
{
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);  // 获取 UTC 时间

	// 将 FILETIME 转换为 100 纳秒单位的时间
	ULARGE_INTEGER uli;
	uli.LowPart = ft.dwLowDateTime;
	uli.HighPart = ft.dwHighDateTime;

	// 转换为毫秒（从 1601-01-01 调整为 1970-01-01）
	const long long EPOCH_OFFSET = 116444736000000000LL;  // 单位: 100 纳秒
	return (uli.QuadPart - EPOCH_OFFSET) / 10000;
}

std::string UtilsTools::GetUUID()
{
	UUID uuid;
	RPC_STATUS status = UuidCreate(&uuid);
	if (status != RPC_S_OK) return "";

	// 将 UUID 转换为 RPC_CSTR 字符串
	RPC_CSTR rpc_str = nullptr;
	status = UuidToStringA(&uuid, &rpc_str);
	if (status != RPC_S_OK || !rpc_str) return "";

	// 将 RPC_CSTR 转换为 std::string
	std::string uuid_str(reinterpret_cast<const char*>(rpc_str));

	// 释放 RPC_CSTR 内存
	RpcStringFreeA(&rpc_str);

	return uuid_str;
}


unsigned char ToHex(unsigned char x)
{
	return  x > 9 ? x + 55 : x + 48;
}

unsigned char FromHex(unsigned char x)
{
	unsigned char y;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	return y;
}

std::string UtilsTools::UrlEncode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (isalnum((unsigned char)str[i]) ||
			(str[i] == '-') ||
			(str[i] == '_') ||
			(str[i] == '.') ||
			(str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ')
			strTemp += "+";
		else
		{
			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] % 16);
		}
	}
	return strTemp;
}

std::string UtilsTools::UrlDecode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (str[i] == '+') strTemp += ' ';
		else if (str[i] == '%')
		{
			unsigned char high = FromHex((unsigned char)str[++i]);
			unsigned char low = FromHex((unsigned char)str[++i]);
			strTemp += high * 16 + low;
		}
		else strTemp += str[i];
	}
	return strTemp;
}