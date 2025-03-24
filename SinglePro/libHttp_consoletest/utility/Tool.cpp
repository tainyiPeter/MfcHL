//#include "stdafx.h"
#include <atlbase.h>
#include <ShlObj.h>

#include "Tool.h"
#include "Registry.h"
#include <WtsApi32.h>

#pragma comment(lib, "WtsApi32.lib")
using namespace std;
// {C14625DE-069A-4872-9C64-BE79C751DED2}
CLSID CLSID_DeskBand = { 0xc14625de, 0x69a, 0x4872,{ 0x9c, 0x64, 0xbe, 0x79, 0xc7, 0x51, 0xde, 0xd2 } };

CImpersonateUser::CImpersonateUser()
{
	m_bIsImpersonating = false;
}

CImpersonateUser::~CImpersonateUser()
{
	Revert();
}

BOOL CImpersonateUser::FindActiveSessionId(OUT DWORD& dwSessionId)
{
	BOOL bFindActiveSession = FALSE;
	DWORD dwIndex = 0;

	PWTS_SESSION_INFO pWtsSessionInfo = NULL;
	DWORD dwCntWtsSessionInfo = 0;

	do
	{
		dwSessionId = (DWORD)(-1);

		if ((!WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pWtsSessionInfo, &dwCntWtsSessionInfo))
			|| (NULL == pWtsSessionInfo))
		{
			break;
		}

		for (dwIndex = 0; dwIndex < dwCntWtsSessionInfo; dwIndex++)
		{
			if (WTSActive == pWtsSessionInfo[dwIndex].State)
			{
				dwSessionId = pWtsSessionInfo[dwIndex].SessionId;
				bFindActiveSession = TRUE;
				break;
			}
		}

		WTSFreeMemory(pWtsSessionInfo);

		if (!bFindActiveSession)
		{
			break;
		}
	} while (0);

	return bFindActiveSession;
}

bool CImpersonateUser::ImpersonateCurrentUser()
{
	BOOL bSucc = FALSE;
	HANDLE  hToken;
	HANDLE hTokenDuplicated = NULL;
	DWORD dwSessionId;

	FindActiveSessionId(dwSessionId);
	if (::WTSQueryUserToken(dwSessionId, &hToken))
	{
		DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenPrimary, &hTokenDuplicated);
		CloseHandle(hToken);
		bSucc = ::ImpersonateLoggedOnUser(hTokenDuplicated);
		CloseHandle(hTokenDuplicated);
	}

	m_bIsImpersonating = bSucc ? true : false;
	return m_bIsImpersonating;
}

void CImpersonateUser::Revert()
{
	if (m_bIsImpersonating)
	{
		RevertToSelf();
		m_bIsImpersonating = false;
	}
}

typedef LONG(WINAPI* pfnRtlGetVersion)(RTL_OSVERSIONINFOEXW*);
bool Win10GetVersionEx(RTL_OSVERSIONINFOEXW& osverinfo)
{
	osverinfo.dwOSVersionInfoSize = sizeof(osverinfo);
	HMODULE hmodule = LoadLibrary(L"ntdll.dll");
	pfnRtlGetVersion Rtl = (pfnRtlGetVersion)GetProcAddress(hmodule, "RtlGetVersion");
	if (Rtl)
	{
		if (!Rtl(&osverinfo))
		{
			FreeLibrary(hmodule);
			return true;
		}
		else
		{
			FreeLibrary(hmodule);
			return false;
		}
	}
	else
	{
		FreeLibrary(hmodule);
		return false;
	}
}

bool GetOsVersion(OSVERSIONINFOEX& osvi, UINT& bit)
{
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	RTL_OSVERSIONINFOEXW  osInfo = { 0 };
	if (!Win10GetVersionEx(osInfo))
		return false;

	osvi.dwMajorVersion = osInfo.dwMajorVersion;
	osvi.dwMinorVersion = osInfo.dwMinorVersion;

	// Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.

	SYSTEM_INFO si;
	SecureZeroMemory(&si, sizeof(SYSTEM_INFO));

	GetNativeSystemInfo(&si);

	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
		bit = 64; // _T(" 64-bit");
	else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
		bit = 32;  //_T(" 32-bit");
	return true;
}

bool GetOSName(CString& Osname, UINT& bit)
{
	SYSTEM_INFO si;
	SecureZeroMemory(&si, sizeof(SYSTEM_INFO));

	GetNativeSystemInfo(&si);

	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
		bit = 64; // _T(" 64-bit");
	else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
		bit = 32;  //_T(" 32-bit");

	BOOL bSucc = CRegHelper::GetRegistryValue(
		HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"),
		_T("ProductName"),
		Osname);

	return bSucc ? true : false;
}
//copy from msdn
typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

BOOL IsWow64()
{
#if defined(_WIN64)
	return TRUE;
#elif defined(_WIN32)
	BOOL bIsWow64 = FALSE;
	LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
		GetModuleHandle(_T("kernel32")), "IsWow64Process");

	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
		{
			// handle error
		}
	}
	return bIsWow64;
#endif
}

CString GetModulePath()
{
	TCHAR szPath[MAX_PATH] = { 0 };
	if (GetModuleFileName(NULL, szPath, MAX_PATH))
	{
		(_tcsrchr(szPath, _T('\\')))[1] = 0;
	}

	return szPath;
}

void StringSplit(std::string& s, std::string& delim, std::vector< std::string >& ret)
{
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		ret.push_back(s.substr(last, index - last));
		last = index + 1;
		while (delim == s.substr(last, 1))
			last++;

		index = s.find_first_of(delim, last);
	}
	if (index - last > 0)
	{
		ret.push_back(s.substr(last, index - last));
	}
}

void WStringSplit(std::wstring& s, std::wstring& delim, std::vector< std::wstring >& ret)
{
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::wstring::npos)
	{
		ret.push_back(s.substr(last, index - last));
		last = index + 1;
		while (delim == s.substr(last, 1))
			last++;
		index = s.find_first_of(delim, last);
	}
	if (index - last > 0)
	{
		ret.push_back(s.substr(last, index - last));
	}
}

void CStringSplit(const CString& s, const CString& delim, std::vector<CString >& ret)
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

bool IsDeskBandShow()
{
#ifdef LOG_INFO
	LOG_INFO(_T("IsDeskBandShow"));
#endif // DEBUG

	bool DeskBandShow = false;
	ITrayDeskBand* pTrayDeskBand = NULL;
	CoInitialize(NULL);
	HRESULT hr = CoCreateInstance(CLSID_TrayDeskBand, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pTrayDeskBand));
	// Vista and higher operating system
	if (SUCCEEDED(hr))
	{
		int retry = 0;
		while (retry < 3)//ÔÊÐíÊ§°Ü£¬ÖØÊÔ3´Î
		{
			hr = pTrayDeskBand->IsDeskBandShown(CLSID_DeskBand);
#ifdef LOG_INFO
			LOG_INFO(_T("IsDeskBandShown:%d"), hr);
#endif
			if (S_OK == hr)
			{
				DeskBandShow = true;
				break;
			}
			else if (S_FALSE == hr)
			{
				DeskBandShow = false;
				break;
			}
			retry++;
		}
		pTrayDeskBand->Release();
	}
	CoUninitialize();
#ifdef LOG_INFO
	LOG_INFO(_T("DeskBandShow:%d"), DeskBandShow);
#endif // LOG_INFO

	return DeskBandShow;
}

string WStringToUTF8String(LPCWSTR str)
{
	string strTmp;
	strTmp = CT2A(str, CP_UTF8);
	return strTmp;
}

CString Utf8StringToWString(string str)
{
	CString strTmp;
	strTmp = CA2T(str.c_str(), CP_UTF8);
	return strTmp;
}

CString GetCmdLineValueFromKey(const CString& cmdLine, CString key)
{
	CString  name = L"--" + key + L"=";
	int titlePos = cmdLine.Find(name);
	CString value = L"";
	if (titlePos >= 0)
	{
		titlePos = titlePos + name.GetLength();
		int spacePos = cmdLine.Find(L"--", titlePos);
		if (spacePos >= 0)
		{
			value = cmdLine.Mid(titlePos, spacePos - titlePos);
		}
		else
		{
			value = cmdLine.Mid(titlePos);
		}
	}
	return value.Trim();
}