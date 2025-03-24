#pragma once

#include <atlstr.h>
#include <string>
#include <vector>

namespace pcutil
{
	class CStringHelper {
	public:
		CString static ConvertUtf8ToLocal(const std::string& strUtf8);
		static CString ConvertUtf8ToLocal(const char* strUtf8);
		static std::string ConvertLocalToUtf8(LPCTSTR lpLocal);

		static CString ToString(int arg, int radix = 10);
		static CString UInt32ToString(unsigned int arg, int radix = 10);
		static CString ToString(__int64 arg, int radix = 10);
		static CString UInt64ToString(unsigned __int64 arg, int radix = 10);

		static BOOL ToBOOL(const CString& str);
		static DWORD TimeDiff(DWORD dwCur, DWORD dwStart);

		static CString URLEncode(const CString &strURL);
		static bool isSafe(char ch);
		static BOOL SplitCStringToArray(const CString &str, const CString &strDim, std::vector<CString> &arrRet, BOOL bAddZeroLengthItem = FALSE);
		static void CStringSplit(const CString& s, const CString& delim, std::vector<CString >& ret);

		//static UUID GenerateUUID();
		//static UUID GenerateUUID(std::string &result);
		//static UUID GenerateUUID(CString &result);


		static CString  ToCString(std::string& str);
		static CString  ToCString(const char* str);
		static std::string  ToString(CString& str);
		static std::string  IntToString(int arg);
		static BOOL  MByteToWChar(LPCSTR lpcszStr, LPWSTR* lpwszStr, DWORD MBytedwSize);


		static void CStringTostring(const CString& src, std::string& des);

		static std::string string_format(const std::string fmt, ...);

	};
}