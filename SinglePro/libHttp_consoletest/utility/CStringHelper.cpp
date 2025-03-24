#pragma once

#include "CStringHelper.h"
//#include <windows.h>
//#include <malloc.h>
//#include <tchar.h>
#include <sstream>
namespace pcutil
{
	CString CStringHelper::ConvertUtf8ToLocal(const std::string& strUtf8)
	{
		return ConvertUtf8ToLocal(strUtf8.c_str());
	}
	CString CStringHelper::ConvertUtf8ToLocal(const char* strUtf8)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, NULL, 0);
		if (len == 0)
			return CString();
		CString ret;
		LPWSTR lpBuffer = ret.GetBuffer(len);
		MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, lpBuffer, len);
		ret.ReleaseBuffer();
		return ret;
	}

	std::string CStringHelper::ConvertLocalToUtf8(LPCTSTR lpLocal)
	{
		// 对输入参数的有效性进行判断，如果输入NULL，返回
		if (!lpLocal || 0 == wcslen(lpLocal))
			return std::string();
			
		//CAutoBuffer szUtf8;
		int len = 0;
		len = WideCharToMultiByte(CP_UTF8, 0, lpLocal, -1, NULL, 0, NULL, NULL);
		std::string text(len - 1, '\0');
		
		//if (!szUtf8.AllocateBuffer(len - 1)) return std::string();
		//WideCharToMultiByte(CP_UTF8, 0, lpLocal, -1, szUtf8.GetBuffer(), len - 1, NULL, NULL);
		WideCharToMultiByte(CP_UTF8, 0, lpLocal, -1, &text[0], len - 1, NULL, NULL);
		//return szUtf8.ToString();
		return text;
	}




	CString CStringHelper::ToString(int arg, int radix)
	{
		TCHAR buf[68] = { _T('\0') };
		_itot_s(arg, buf, radix);
		CString strRet = buf;
		return strRet;
	}

	CString CStringHelper::ToString(__int64 arg, int radix)
	{
		TCHAR buf[68] = { _T('\0') };
		_i64tot_s(arg, buf, 68, radix);
		CString strRet = buf;
		return strRet;
	}


	CString CStringHelper::UInt32ToString(unsigned int arg, int radix)
	{
		CString strRet;
		if (radix == 10)
		{
			strRet.Format(_T("%u"), arg);
		}
		else if (radix == 16)
		{
			strRet.Format(_T("%x"), arg);
		}
		else
		{
			ATLASSERT(FALSE);
		}
		return strRet;
	}


	CString CStringHelper::UInt64ToString(unsigned __int64 arg, int radix)
	{
		TCHAR buf[68] = { _T('\0') };
		_ui64tot_s(arg, buf, 68, radix);
		CString strRet = buf;
		return strRet;
	}

	BOOL CStringHelper::ToBOOL(const CString& str)
	{
		CString strTemp = str;
		strTemp.Trim();
		strTemp.MakeUpper();
		if (strTemp == _T("TRUE") || strTemp == _T("1"))
		{
			return TRUE;
		}
		return FALSE;

	}



	DWORD CStringHelper::TimeDiff(DWORD dwCur, DWORD dwStart)
	{
		if (dwCur >= dwStart)
		{
			return dwCur - dwStart;
		}
		else
		{
			return INFINITE - dwStart + dwCur;
		}
		return 0;
	}


	bool CStringHelper::isSafe(char ch)
	{
		if ((((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z'))) || ((ch >= '0') && (ch <= '9')))
		{
			return true;
		}
		switch (ch)
		{
		case '\'':
		case '(':
		case ')':
		case '*':
		case '-':
		case '.':
		case '_':
		case '!':
			return true;
		}
		return false;
	}
	/*bool CStringHelper::isSafeEx(char ch)
	{
		if ((((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z'))) || ((ch >= '0') && (ch <= '9')))
		{
			return true;
		}
		switch (ch)
		{
		case '-':
		case '.':
		case '_':
		case '~':
			return true;
		}
		return false;
	}*/
	CString CStringHelper::URLEncode(const CString &strURL)
	{
		// 由于此方法没有考虑到unicode转换的问题，所以逻辑上改为URLEncodeEx
		// 在空格的处理上，保留原来URLEncode的方式，省略空格字符
		std::string tt = ConvertLocalToUtf8(strURL);
		std::string dd;
		size_t len = tt.length();
		for (size_t i = 0; i<len; i++)
		{
			if (isSafe((BYTE)tt.at(i)))
			{
				char tempbuff[2] = { 0 };
				sprintf_s(tempbuff, "%c", (BYTE)tt.at(i));
				dd.append(tempbuff);
			}
			else if (isspace((BYTE)tt.at(i)))
			{
				dd.append("%20");
			}
			else
			{
				char tempbuff[4];
				sprintf_s(tempbuff, "%%%X%X", ((BYTE)tt.at(i)) >> 4, ((BYTE)tt.at(i)) % 16);
				dd.append(tempbuff);
			}

		}
		CString result;
		result = CA2T(dd.c_str());
		return result;
	}


	BOOL CStringHelper::SplitCStringToArray(const CString &str,
		const CString &strDim,
		std::vector<CString> &arrRet,
		BOOL bAddZeroLengthItem)
	{
		arrRet.clear();
		//CString resToken;
		//int curPos = 0;
		//resToken= str.Tokenize( strDim ,curPos );
		//while( resToken != _T("") )
		//{
		//   arrRet.push_back( resToken );
		//   resToken = str.Tokenize( strDim, curPos );
		//}   
		//return TRUE;

		int nDimLen = strDim.GetLength();
		if (nDimLen <= 0) return FALSE;
		std::auto_ptr<CString> pString(new CString(str));
		if (pString.get() == NULL) return FALSE;
		CString &strSrc = *pString;
		strSrc += strDim;
		DWORD nLoop = 0;
		do
		{
			int iRet = strSrc.Find(strDim);
			if (iRet >= 0)
			{
				CString strPart = strSrc.Left(iRet);
				if (strPart.GetLength() > 0 || bAddZeroLengthItem)
				{
					arrRet.push_back((LPCTSTR)strPart);
				}

				int iLeft = strSrc.GetLength() - iRet - nDimLen;
				if (iLeft <= 0)
					break;
				strSrc = strSrc.Right(iLeft);
			}
			if (++nLoop > 500000) //add this avoid dead loop
				return FALSE;
		} while (pString->Find(strDim) >= 0);
		return TRUE;
	}

	void CStringHelper::CStringSplit(const CString& s, const CString& delim, std::vector<CString >& ret)
	{
		int last = 0;
		int index = s.Find(delim, last);
		while (index != -1)
		{
			ret.push_back(s.Mid(last, index - last));
			last = index + 1;
			while (delim == s.GetAt(last))
				last++;
			index = s.Find(delim, last);
		}
		if (last < s.GetLength())
		{
			ret.push_back(s.Mid(last, s.GetLength() - last));
		}
	}


	CString CStringHelper::ToCString(std::string& str)
	{
		return ToCString(str.c_str());
	}

	CString CStringHelper::ToCString(const char* str)
	{
		if (str == NULL)
		{
			return _T("");
		}
		CString result;
		LPWSTR buff = NULL;
		if (MByteToWChar(str, &buff, 0))
		{
			result = buff;
			delete[] buff;
			return result;
		}
		result = CA2T(str).m_psz;
		return result;
	}

	BOOL CStringHelper::MByteToWChar(LPCSTR lpcszStr, LPWSTR* lpwszStr, DWORD MBytedwSize)
	{
		// Get the required size of the buffer that receives the Unicode
		// string.
		DWORD dwMinSize;
		dwMinSize = MultiByteToWideChar(CP_ACP, 0, lpcszStr, -1, NULL, 0);

		if (*lpwszStr != NULL)
		{
			delete[](*lpwszStr);
			*lpwszStr = NULL;
		}
		*lpwszStr = new wchar_t[dwMinSize];

		// Convert headers from ASCII to Unicode.
		MultiByteToWideChar(CP_ACP, 0, lpcszStr, -1, *lpwszStr, dwMinSize);
		return TRUE;
	}

	std::string CStringHelper::ToString(CString& str)
	{
		/*char* buff = NULL;
		if( CommonHelper::WCharToMByte( str.GetBuffer(), buff, str.GetLength() ) )
		{
			return buff;
		}*/
		std::string result;
		result = CT2A(str.GetBuffer()).m_psz;
		return result;
	}

	std::string CStringHelper::IntToString(int arg)
	{
		std::stringstream ss;
		std::string str;
		ss << arg;
		ss >> str;

		return str;
	}


	void CStringHelper::CStringTostring(const CString& src, std::string& des)
	{
		CString strSrc = src;
		des = CT2A(strSrc);
		//return;
		//int i;
		//for( i = 0;i < src.GetLength(); i++ )
		//{
		//	char ch;
		//	ch = src.GetAt( i );
		//	des.push_back( ch );
		//}
	}

	std::string CStringHelper::string_format(const std::string fmt, ...) {
		int size = ((int)fmt.size()) * 2 + 50;   // Use a rubric appropriate for your code
		std::string str;
		va_list ap;
		while (1) {     // Maximum two passes on a POSIX system...
			str.resize(size);
			va_start(ap, fmt);
			int n = vsnprintf((char*)str.data(), size, fmt.c_str(), ap);
			va_end(ap);
			if (n > -1 && n < size) {  // Everything worked
				str.resize(n);
				return str;
			}
			if (n > -1)  // Needed size returned
				size = n + 1;   // For null char
			else
				size *= 2;      // Guess at a larger size (OS specific)
		}
		return str;
	}
	// Move the function to CUUID.h
	//UUID CStringHelper::GenerateUUID()
	//{
	//	UUID uuid = { 0 };
	//	if (UuidCreate(&uuid) != RPC_S_OK)
	//	{
	//		ZeroMemory(&uuid, sizeof(UUID));
	//		uuid.Data1 = GetTickCount();
	//	}
	//	return uuid;
	//}

	//UUID CStringHelper::GenerateUUID(std::string &result)
	//{
	//	UUID uuid = { 0 };
	//	if (UuidCreate(&uuid) != RPC_S_OK)
	//	{
	//		ZeroMemory(&uuid, sizeof(UUID));
	//		uuid.Data1 = GetTickCount();
	//	}
	//	unsigned char* str = NULL;
	//	UuidToStringA(&uuid, &str);
	//	if (str != NULL)
	//	{
	//		result = reinterpret_cast<char*>(str);
	//	}
	//	RpcStringFreeA(&str);
	//	return uuid;
	//}

	//UUID CStringHelper::GenerateUUID(CString &result)
	//{
	//	UUID uuid = { 0 };
	//	if (UuidCreate(&uuid) != RPC_S_OK)
	//	{
	//		ZeroMemory(&uuid, sizeof(UUID));
	//		uuid.Data1 = GetTickCount();
	//	}
	//	unsigned short* str = NULL;
	//	UuidToStringW(&uuid, &str);
	//	if (str != NULL)
	//	{
	//		result = reinterpret_cast<wchar_t*>(str);
	//	}
	//	RpcStringFreeW(&str);
	//	return uuid;
	//}

}

//
//char * Unicode2GBK(LPCWSTR lpUnicodeStr)
//{
//	if (lpUnicodeStr == NULL)
//		return "";
//
//
//	int nGBKStrLen = WideCharToMultiByte(CP_ACP, 0, lpUnicodeStr, -1, NULL, NULL, NULL, NULL);
//	char *lpGBKStr = new char[nGBKStrLen + 1];
//	if (lpGBKStr) {
//		WideCharToMultiByte(CP_ACP, 0, lpUnicodeStr, -1, lpGBKStr, nGBKStrLen, NULL, NULL);
//	}
//
//	return lpGBKStr;
//}
//
//WCHAR * GBK2Unicode(const char *lpGBKStr)
//{
//	if (lpGBKStr == NULL)
//		return L"";
//
//
//	int cchWideChar = (int)strlen(lpGBKStr);
//	wchar_t *lpUnicodeStr = new wchar_t[cchWideChar + 1];
//	memset(lpUnicodeStr, '\0', sizeof(wchar_t)*(cchWideChar + 1));
//	if (lpUnicodeStr) {
//		MultiByteToWideChar(CP_ACP, 0, lpGBKStr, -1, lpUnicodeStr, cchWideChar);
//	}
//
//	return lpUnicodeStr;
//}
//
//WCHAR * UTF8toUnicode(const char *lpUTF8Str)
//{
//	if (lpUTF8Str == NULL)
//		return L"";
//
//
//	int cchWideChar = MultiByteToWideChar(CP_UTF8, 0, lpUTF8Str, -1, NULL, NULL);
//	wchar_t *lpUnicodeStr = new wchar_t[cchWideChar + 1];
//	if (lpUnicodeStr) {
//		MultiByteToWideChar(CP_UTF8, 0, lpUTF8Str, -1, lpUnicodeStr, cchWideChar);
//
//	}
//
//	return lpUnicodeStr;
//}
//
//char * Unicode2Utf8(LPCWSTR lpUnicodeStr)
//{
//	if (lpUnicodeStr == NULL)
//		return "";
//
//	int nGBKStrLen = WideCharToMultiByte(CP_UTF8, 0, lpUnicodeStr, -1, NULL, NULL, NULL, NULL);
//	char *lpGBKStr = new char[nGBKStrLen + 1];
//	if (lpGBKStr) {
//		WideCharToMultiByte(CP_UTF8, 0, lpUnicodeStr, -1, lpGBKStr, nGBKStrLen, NULL, NULL);
//	}
//
//	return lpGBKStr;
//}