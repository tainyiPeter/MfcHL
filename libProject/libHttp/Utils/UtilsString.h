#pragma once

#include "tchar.h"

#include <memory>
#include <string>
#include <set>
#include <vector>

class UtilsString
{

public:
 	template<typename ... Args>
	static std::string FormatString(const std::string& format, Args ... args)
	{
		auto size = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
		std::unique_ptr<char[]> buf(new char[size]);
		std::snprintf(buf.get(), size, format.c_str(), args ...);
		return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
	}

	template<typename ... Args>
	static std::wstring FormatString(const std::wstring& format, Args ... args)
	{
		auto size = std::swprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
		std::unique_ptr<wchar_t[]> buf(new wchar_t[size]);
		std::swprintf(buf.get(), size, format.c_str(), args ...);
		return std::wstring(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
	}

	/** 忽略大小写比较字符串
	*/
	static bool IsEqualNoCase(const std::string& strData1, const std::string& strData2)
	{
		return IsEqualNoCase(strData1.c_str(), strData2.c_str());
	}
	static bool IsEqualNoCase(const char* szData1, const char* szData2)
	{
		if ((szData1 == nullptr) || (szData2 == nullptr))
		{
			return false;
		}

		return (_stricmp(szData1, szData2) == 0);
	}

	static bool IsEqualNoCase(const std::wstring& strData1, const std::wstring& strData2)
	{
		return IsEqualNoCase(strData1.c_str(), strData2.c_str());
	}

	static bool IsEqualNoCase(const TCHAR* szData1, const TCHAR* szData2)
	{
		if ((szData1 == nullptr) ||
			(szData2 == nullptr))
		{
			return false;
		}
		return (_wcsicmp(szData1, szData2) == 0);
	}

	static std::wstring FormatIntToString(int64_t* hlOffsetMs, uint32_t cnt);
	static std::wstring FormatIntToString(uint64_t* hlOffsetMs, uint32_t cnt);

	static bool SplitStr(const std::string& strData, char tag, std::vector<std::string>& vecResult);
	static bool SplitStr(const std::wstring& strData, TCHAR tag, std::vector<std::wstring>& vecResult);

	/** 字符串大小写转换
	*/
	static std::string ConverToUpper(const std::string& src);
	static std::string ConverToLower(const std::string& src);

	static std::wstring ConverToUpper(const std::wstring& src);
	static std::wstring ConverToLower(const std::wstring& src);
};