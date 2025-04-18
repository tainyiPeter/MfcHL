﻿#include "UtilsString.h"

#include <algorithm>

std::wstring UtilsString::FormatIntToString(int64_t* hlOffsetMs, uint32_t cnt)
{
	std::wstring strRet;
	if (nullptr == hlOffsetMs || cnt == 0) return strRet;
	for (int i = 0; i < cnt; ++i)
	{
		strRet += std::to_wstring(hlOffsetMs[i]) + L",";
	}

	return strRet;
}

std::wstring UtilsString::FormatIntToString(uint64_t* hlOffsetMs, uint32_t cnt)
{
	std::wstring strRet;
	if (nullptr == hlOffsetMs || cnt == 0) return strRet;
	for (int i = 0; i < cnt; ++i)
	{
		strRet += std::to_wstring(hlOffsetMs[i]) + L",";
	}

	return strRet;
}

bool UtilsString::SplitStr(const std::string& strData, char tag, std::vector<std::string>& vecResult)
{
	if (strData.empty())
	{
		return false;
	}

	std::string subStr;
	for (size_t i = 0; i < strData.length(); i++)
	{
		if (tag == strData[i])
		{
			if (!subStr.empty())
			{
				vecResult.push_back(subStr);
				subStr.clear();
			}
		}
		else
		{
			subStr.push_back(strData[i]);
		}
	}

	if (!subStr.empty()) //剩余的子串作为最后的子字符串
	{
		vecResult.push_back(subStr);
	}

	return !vecResult.empty();
}

bool UtilsString::SplitStr(const std::wstring& strData, TCHAR tag, std::vector<std::wstring>& vecResult)
{
	if (strData.empty())
	{
		return false;
	}

	std::wstring subStr;
	for (size_t i = 0; i < strData.length(); i++)
	{
		if (tag == strData[i])
		{
			if (!subStr.empty())
			{
				vecResult.push_back(subStr);
				subStr.clear();
			}
		}
		else
		{
			subStr.push_back(strData[i]);
		}
	}

	if (!subStr.empty()) //剩余的子串作为最后的子字符串
	{
		vecResult.push_back(subStr);
	}

	return !vecResult.empty();
}

std::string UtilsString::ConverToUpper(const std::string& src)
{
	if (src.empty()) return "";

	std::string ret = src;
	std::transform(src.begin(), src.end(), ret.begin(), ::toupper);

	return ret;
}

std::string UtilsString::ConverToLower(const std::string& src)
{
	if (src.empty()) return "";

	std::string ret = src;
	std::transform(src.begin(), src.end(), ret.begin(), ::tolower);

	return ret;
}

std::wstring UtilsString::ConverToUpper(const std::wstring& src)
{
	if (src.empty()) return L"";

	std::wstring ret = src;
	std::transform(src.begin(), src.end(), ret.begin(), ::toupper);

	return ret;
}

std::wstring UtilsString::ConverToLower(const std::wstring& src)
{
	if (src.empty()) return L"";

	std::wstring ret = src;
	std::transform(src.begin(), src.end(), ret.begin(), ::tolower);

	return ret;
}

std::string UtilsString::ConvertUint64ToString(uint64_t n64Value)
{
	char buf[64] = { "" };
	sprintf_s(buf, u8"%I64u", n64Value);
	return buf;
}

std::string UtilsString::BuildHttpBody(std::vector<std::tuple<std::string, std::string>>& vecData)
{
	std::string strBody;
	bool bFirst = true;
	for (auto iter : vecData)
	{
		if (bFirst)
		{
			strBody += FormatString("%s=%s", std::get<0>(iter).c_str(), std::get<1>(iter).c_str());
			bFirst = false;
		}
		else
		{
			strBody += FormatString("&%s=%s", std::get<0>(iter).c_str(), std::get<1>(iter).c_str());
		}
	}

	return strBody;
}
