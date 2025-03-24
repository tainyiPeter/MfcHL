#pragma once
#include <atlsecurity.h>
#include <string>
#include <vector>
#include <atlstr.h>

class CImpersonateUser
{
public:
	CImpersonateUser();
	virtual  ~CImpersonateUser();
	bool      ImpersonateCurrentUser();
	void		Revert();

	BOOL FindActiveSessionId(OUT DWORD& dwSessionId);
private:
	bool 	m_bIsImpersonating;
};

BOOL IsWow64();
bool GetOsVersion(OSVERSIONINFOEX& osvi, UINT& bit);
bool GetOSName(CString& Osname, UINT& bit);

CString GetModulePath();

void StringSplit(std::string& s, std::string& delim, std::vector< std::string >& ret);
void WStringSplit(std::wstring& s, std::wstring& delim, std::vector< std::wstring >& ret);
void CStringSplit(const CString& s, const CString& delim, std::vector<CString>& ret);
bool IsDeskBandShow();
std::string WStringToUTF8String(LPCWSTR str);
CString Utf8StringToWString(std::string str);

//命令行中获取--key=的内容
CString GetCmdLineValueFromKey(const CString& cmdLine, CString key);