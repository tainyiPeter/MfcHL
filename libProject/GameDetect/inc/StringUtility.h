#pragma once
#include <windows.h>
#include <string>
#include <algorithm>
#include <tchar.h>
using namespace std;


inline wstring TransformCase(wstring _String, bool lower = true)
{
	if (lower)
		std::transform(_String.begin(), _String.end(), _String.begin(), ::towlower);
	else
		std::transform(_String.begin(), _String.end(), _String.begin(), ::towupper);
	return _String;
}

inline string TransformCase(string _String, bool lower = true)
{
	if (lower)
		std::transform(_String.begin(), _String.end(), _String.begin(), ::tolower);
	else
		std::transform(_String.begin(), _String.end(), _String.begin(), ::toupper);
	return _String;
}

inline bool FindWithoutCase(const wchar_t * _String1, const wchar_t * _String2)
{
	if (TransformCase(_String1).find(TransformCase(_String2)) != wstring::npos)
		return true;
	return false;
}

inline bool FindWithoutCase(const char * _String1, const char * _String2)
{
	if (TransformCase(_String1).find(TransformCase(_String2)) != string::npos)
		return true;
	return false;
}

inline std::string UnicodeToUTF8(const std::wstring& wstr)
{
	std::string ret;
	BOOL UsedDefaultChar = FALSE;
	DWORD dwNum = WideCharToMultiByte(CP_UTF8, NULL, wstr.c_str(), -1, NULL, 0, NULL, &UsedDefaultChar);
	if (0 == dwNum)
	{
		return ret;
	}

	char* psText = new char[dwNum + 1];
	SecureZeroMemory(psText, sizeof(char) * (dwNum + 1));
	WideCharToMultiByte(CP_UTF8, NULL, wstr.c_str(), -1, psText, dwNum, NULL, &UsedDefaultChar);

	if (nullptr != psText)
	{
		ret = psText;
		delete[] psText;
		psText = nullptr;
	}

	return ret;
}

inline std::wstring UTF8ToUnicode(const std::string& str)
{
	std::wstring ret;

	int len = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)str.c_str(), -1, NULL, 0);
	if (0 == len)
	{
		return ret;
	}

	wchar_t* wszUtf8 = new wchar_t[len + 1];
	SecureZeroMemory(wszUtf8, sizeof(wchar_t) * (len + 1));
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)str.c_str(), -1, (LPWSTR)wszUtf8, len);

	if (nullptr != wszUtf8)
	{
		ret = wszUtf8;
		delete[] wszUtf8;
		wszUtf8 = nullptr;
	}

	return ret;
}


inline bool ContainPath(const wstring & entirePath, const wstring & subPath)
{
	wstring findPath = subPath;
	if (findPath.at(0) != L'\\')
		findPath = L"\\" + findPath;

	size_t index = entirePath.rfind(findPath);
	if (index != wstring::npos && index == (entirePath.size() - findPath.size()))
		return true;

	return false;
}

template<typename Set>
bool set_compare(Set const &lhs, Set const &rhs) {
	return lhs.size() == rhs.size()
		&& equal(lhs.begin(), lhs.end(), rhs.begin());
}

//get the size of the string
template <typename T>
struct array_size
{
	static constexpr size_t value = 0;
};

template <typename T, size_t Size>
struct array_size<T const(&)[Size]>
{
	static constexpr size_t value = Size;
};

template <typename T>
static size_t TCHAR_BYTES(const T& P)
{
	return wcsnlen_s(P, MAX_PATH) * sizeof(T);
};

template <typename T>
static size_t TCHAR_LEN(const T& P)
{
	return  wcsnlen_s(P, MAX_PATH);
}

// wchar_t to string
static void Wchar_tToString(const wchar_t *wchar, std::string& szDst)
{
	const wchar_t * wText = wchar;
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);
	char *psText;
	psText = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);
	szDst = psText;
	delete[]psText;
}

//to utf8
static void Wchar_tToUtf8(const wchar_t *wchar, std::string& szDst)
{
	const wchar_t * wText = wchar;
	DWORD dwNum = WideCharToMultiByte(CP_UTF8, NULL, wText, -1, NULL, 0, NULL, FALSE);
	char *psText = new char[dwNum];
	WideCharToMultiByte(CP_UTF8, NULL, wText, -1, psText, dwNum, NULL, FALSE);
	szDst = psText;
	delete[]psText;
}

//string to wstring
static void StringToWstring(std::string str, std::wstring& szDst)
{
	int len = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)str.c_str(), -1, NULL, 0);
	wchar_t * wszUtf8 = new wchar_t[len + 1];
	SecureZeroMemory(wszUtf8, len * 2 + 2);
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)str.c_str(), -1, (LPWSTR)wszUtf8, len);
	szDst = wszUtf8;
	delete[] wszUtf8;
}

static std::wstring _A2W_(std::string str)
{
	std::wstring szDst;
	StringToWstring(std::move(str), szDst);
	return szDst;
}

static std::string _W2A_(std::wstring str)
{
	std::string szDst;
	Wchar_tToString(str.c_str(), szDst);
	return szDst;
}

static BOOL MByteToWCharEx(LPCSTR _lpcszStr, LPWSTR _lpwszStr, DWORD _dwSize)
{
	// Get the required size of the buffer that receives the Unicode
	// string.
	DWORD dwMinSize;
	dwMinSize = MultiByteToWideChar(CP_ACP, 0, _lpcszStr, -1, NULL, 0);

	if (_dwSize < dwMinSize)
	{
		return FALSE;
	}
	// Convert headers from ASCII to Unicode.
	MultiByteToWideChar(CP_ACP, 0, _lpcszStr, -1, _lpwszStr, dwMinSize);
	return TRUE;
}

// not be called, this function can't work. the following code trigger a assert.
// wstrDst = Fmt(L"%d", 123);
// assert(wstrDst == L"123");
static wstring Fmt(const WCHAR* format, ...)
{
	va_list argList;
	va_start(argList, format);

	size_t length = _vsctprintf(format, argList) + 1;
	wstring str(length, L'\0');

	_vsntprintf_s(&str[0], length, _TRUNCATE, format, argList);
	va_end(argList);

	return str;
}

inline std::vector<std::wstring> SplitString(std::wstring s, std::wstring delimiter, std::wstring startWord = L"") {
	size_t start = 0;
	size_t end = s.find_first_of(delimiter);

	std::vector<std::wstring> output;

	while (true)
	{
		if (end > 0)
		{
			wstring subString = s.substr(start, end - start);
			if (!subString.empty())
			{
				if (output.empty() && !startWord.empty())
				{
					if (startWord == subString)
						output.emplace_back(std::move(subString));
				}
				else
					output.emplace_back(std::move(subString));
			}
		}

		if (end == std::wstring::npos)
			break;

		start = end + 1;
		end = s.find_first_of(delimiter, start);
	}

	return output;
}