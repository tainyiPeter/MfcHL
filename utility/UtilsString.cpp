#include "UtilsString.h"
#include <sstream>
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

std::wstring UtilsString::GetFileName(const std::wstring& strFileName)
{
	return CFileHelper::GetFileName(strFileName.c_str());
}

std::wstring UtilsString::GetFileNameNoExt(const std::wstring& strFileName)
{
	std::wstring strName = GetFileName(strFileName);
	int pos = strName.find(L".");
	if (std::wstring::npos == pos)
		return strName;

	strName = strName.substr(0, pos);
	return strName;
}

std::string UtilsString::unescape_unicode(const std::string& str)
{
	std::string result;
	for (size_t i = 0; i < str.size();) {
		if (str[i] == '\\' && i + 1 < str.size() && str[i + 1] == 'u') {
			// 提取4位十六进制
			unsigned int code;
			std::stringstream ss;
			ss << std::hex << str.substr(i + 2, 4);
			ss >> code;

			// 转换为UTF-8
			if (code <= 0x7F) {
				result += static_cast<char>(code);
			}
			else if (code <= 0x7FF) {
				result += static_cast<char>(0xC0 | (code >> 6));
				result += static_cast<char>(0x80 | (code & 0x3F));
			}
			else if (code <= 0xFFFF) {
				result += static_cast<char>(0xE0 | (code >> 12));
				result += static_cast<char>(0x80 | ((code >> 6) & 0x3F));
				result += static_cast<char>(0x80 | (code & 0x3F));
			}
			i += 6;
		}
		else {
			result += str[i++];
		}
	}
	return result;
}