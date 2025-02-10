// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <strsafe.h>
#include <UserEnv.h>
#include "DebugUtil.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <tlhelp32.h>
#include <ShlObj.h>
#include <WtsApi32.h>
#include <Psapi.h>
#include <tchar.h>
#pragma comment(lib, "wtsapi32.lib")
#include "CPackageName.h"
#include <memory>

#define USER_INFO_REG_ROOT HKEY_LOCAL_MACHINE
#define USER_INFO_SID_REG_PATH					L"Software\\Lenovo\\VantageService\\AddinData\\LenovoGamingAddin\\Data\\UserInfo"
#define USER_INFO_SID_REG_VALUE					L"UserSID"

class CUserPrivilegeUtil
{
public:

	CUserPrivilegeUtil(void)
	{
		m_exploerpid = 0;
		m_currentsid = L"";
		m_username = L"";
		m_bpowerdown = FALSE;
	}

	~CUserPrivilegeUtil(void)
	{
	}

public:

	BOOL IsProcessvalid(ULONG pid)
	{
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, pid);
		if (hProcess)
		{
			CloseHandle(hProcess);
			return TRUE;
		}

		return FALSE;
	}
	DWORD GetProcessPid(wstring procname)
	{
		PROCESSENTRY32 proc_entry;
		DWORD pid = 0;

		HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snap == INVALID_HANDLE_VALUE) {
			return 0;
		}
		SecureZeroMemory(&proc_entry, sizeof(proc_entry));
		proc_entry.dwSize = sizeof(PROCESSENTRY32);
		if (!Process32First(snap, &proc_entry)) {
			CloseHandle(snap);
			return 0;
		}
		do
		{
			if (!_wcsicmp(proc_entry.szExeFile, procname.c_str()))
			{
				pid = proc_entry.th32ProcessID;
				break;
			}

		} while (Process32Next(snap, &proc_entry));


		CloseHandle(snap);
		return pid;
	}

	BOOL GetProcessStartTime(wstring procname, ULONGLONG &ulTime)
	{
		DWORD pid = GetProcessPid(procname);
		if (pid <= 0)
			return FALSE;

		HANDLE  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, pid);
		if (!hProcess)
			return FALSE;

		FILETIME ftCreationTime;
		FILETIME ftExitTime;
		FILETIME ftKernelTime;
		FILETIME ftUserTime;
		BOOL nRet = GetProcessTimes(hProcess, &ftCreationTime, &ftExitTime, &ftKernelTime, &ftUserTime);
		if (!nRet)
			return FALSE;

		ULARGE_INTEGER ularge;
		ularge.LowPart = ftCreationTime.dwLowDateTime;
		ularge.HighPart = ftCreationTime.dwHighDateTime;
		ulTime = ularge.QuadPart;

		CloseHandle(hProcess);
		return TRUE;
	}

	BOOL DosPathToNtPath(LPCTSTR pszDosPath, LPTSTR pszNtPath)
	{
		TCHAR			szDriveStr[500] = { 0 };
		TCHAR			szDrive[3];
		TCHAR			szDevName[100];
		INT				cchDevName;
		INT				i;

		if (!pszDosPath || !pszNtPath)
			return FALSE;

		if (GetLogicalDriveStrings(500, szDriveStr))
		{
			for (i = 0; szDriveStr[i]; i += 4)
			{
				if (!lstrcmpi(&(szDriveStr[i]), L"A:\\") || !lstrcmpi(&(szDriveStr[i]), L"B:\\"))
					continue;

				szDrive[0] = szDriveStr[i];
				szDrive[1] = szDriveStr[i + 1];
				szDrive[2] = '\0';
				if (!QueryDosDevice(szDrive, szDevName, 100))
					return FALSE;

				CString strTemp(pszDosPath);
				cchDevName = (int)wcsnlen_s(szDevName, 100);
				int npos = strTemp.Find(L"\\", cchDevName);
				if (npos > 0 && _tcsnicmp(pszDosPath, szDevName, npos) == 0)
				{
					wcscpy_s(pszNtPath, MAX_PATH, szDrive);
					wcscat_s(pszNtPath, MAX_PATH, pszDosPath + cchDevName);
					return TRUE;
				}
			}
		}
	
		StringCchCopy(pszNtPath, MAX_PATH, pszDosPath);

		return FALSE;
	}

	BOOL GetProcessHandleByName(HANDLE &hHandle, const wstring &procname, wstring &proPath)
	{
		BOOL           bRet = FALSE;
		PROCESSENTRY32 proc_entry;
		DWORD session_id = WTSGetActiveConsoleSessionId();

		HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snap == INVALID_HANDLE_VALUE) {
			return 0;
		}
		SecureZeroMemory(&proc_entry, sizeof(proc_entry));
		proc_entry.dwSize = sizeof(PROCESSENTRY32);
		if (!Process32First(snap, &proc_entry)) {
			CloseHandle(snap);
			return 0;
		}
		do
		{
			ULONG explorer_session_id = 0;

			if (!_wcsicmp(proc_entry.szExeFile, procname.c_str()))
			{
				if (!ProcessIdToSessionId(proc_entry.th32ProcessID, &explorer_session_id))
				{
					break;
				}

				if (explorer_session_id != session_id)
					continue;

				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,
					FALSE, proc_entry.th32ProcessID);
				if (hProcess)
				{
					hHandle = hProcess;

					TCHAR		szImagePath[MAX_PATH] = { 0 };
					TCHAR pszFullPath[MAX_PATH] = { 0 };
					GetProcessImageFileName(hProcess, szImagePath, MAX_PATH);

					DosPathToNtPath(szImagePath, pszFullPath);

					proPath = pszFullPath;

					bRet = TRUE;
				}
				break;
			}

		} while (Process32Next(snap, &proc_entry));


		CloseHandle(snap);
		return (bRet);
	}

	// not be called.
	BOOL IsVantageFromPackageName(HANDLE hHandle)
	{
		BOOL bIsVantage = FALSE;
		wstring strPackName;
		PACKAGE_ID * packageInfo = GetProcessPackageId(hHandle, strPackName);
		if (packageInfo != nullptr && strPackName != L"no")
		{
			CString strName = packageInfo->name;
			strName.MakeLower();
			if (strName.Find(L"e046963f") >= 0 && strName.Find(L"lenovo") >= 0)
				bIsVantage = TRUE;
		}

		if (packageInfo != nullptr)
			free(packageInfo);

		return bIsVantage;
	}

	// not be called
	CString GetVantageLowerPath()
	{
		wchar_t wcDefaultDir[MAX_PATH] = { 0 };
		if (SHGetSpecialFolderPath(NULL, wcDefaultDir, CSIDL_PROGRAM_FILES, false))
		{
			CString strProgramPath = wcDefaultDir;
			strProgramPath.MakeLower();
			strProgramPath += L"\\windowsapps\\e046963f.";
			return strProgramPath;
		}
		
		return L"";
	}

	// not be called
	BOOL FindModule(DWORD dwId)
	{
		BOOL bFind = FALSE;
		MODULEENTRY32 lpme;
		lpme.dwSize = sizeof(MODULEENTRY32);
		HANDLE hModuleSnap = NULL;
		hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwId);
		if (hModuleSnap == INVALID_HANDLE_VALUE || hModuleSnap == NULL) {
			return bFind;
		}

		CString strVantagePath = GetVantageLowerPath();
		if (strVantagePath != L"")
		{
			BOOL bRet = ::Module32First(hModuleSnap, &lpme);
			while (bRet)
			{
				CString strPath = lpme.szExePath;
				strPath.MakeLower();

				if (strPath.Find(strVantagePath) == 0)
				{
					bFind = TRUE;
					break;
				}
				bRet = ::Module32Next(hModuleSnap, &lpme);
			}
			::CloseHandle(hModuleSnap);
		}
					
		return bFind;
	}

	void ShowProcessApplicationUserModelId(HANDLE process, CString & UWPID)
	{
		UINT32 length = 0;
		LONG rc = GetApplicationUserModelId(process, &length, NULL);
		if (rc != ERROR_INSUFFICIENT_BUFFER)
		{
			return;
		}

		PWSTR fullName = nullptr;
		size_t nLen = length * sizeof(WCHAR);
		fullName = (PWSTR)malloc(nLen);
		if (fullName == NULL)
		{
			DBG_PRINTF_FL((L"ShowProcessApplicationUserModelId fullName is NULL\n"));
			return;
		}

		rc = GetApplicationUserModelId(process, &length, fullName);
		if (rc != ERROR_SUCCESS)
			DBG_PRINTF_FL((L"ShowProcessApplicationUserModelId error=%d\n", rc));
		else
			UWPID = fullName;

		free(fullName);
	}

	bool is_vantage_pid(unsigned int pid)
	{

		std::unique_ptr < HANDLE,void(*)(HANDLE*)> hProcess(new HANDLE(OpenProcess(PROCESS_ALL_ACCESS,
			FALSE, pid)),
			[](HANDLE* handle)
			{
				if (*handle != nullptr) 
					CloseHandle(*handle);
			});

		if (*hProcess == nullptr)
			return false;

		CString UWPName;
		ShowProcessApplicationUserModelId(*hProcess, UWPName);
		if (UWPName.GetLength() > 0)
		{
			DBG_PRINTF_FL((L"ShowProcessApplicationUserModelId UWPName=%s\n", UWPName.GetBuffer()));
			UWPName.MakeLower();
			if (UWPName.Find(L"e046963f") >= 0 && UWPName.Find(L"lenovo") >= 0)
			{
				return true;
			}
		}

		return false;
	}

	BOOL IsVantageRunning(HANDLE &hHandle)
	{
#ifdef UNIT_TEST
		return TRUE;
#else
		BOOL           bRet = FALSE;
		PROCESSENTRY32 proc_entry;

		HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snap == INVALID_HANDLE_VALUE) {
			return 0;
		}

		SecureZeroMemory(&proc_entry, sizeof(proc_entry));
		proc_entry.dwSize = sizeof(PROCESSENTRY32);
		if (!Process32First(snap, &proc_entry)) {
			CloseHandle(snap);
			return 0;
		}
		do
		{
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,
				FALSE, proc_entry.th32ProcessID);
			if (hProcess == NULL)
				continue;

			CString UWPName;
			ShowProcessApplicationUserModelId( hProcess, UWPName);
			if (UWPName.GetLength() > 0)
			{
				DBG_PRINTF_FL((L"ShowProcessApplicationUserModelId UWPName=%s\n", UWPName.GetBuffer()));
				UWPName.MakeLower();
				if (UWPName.Find(L"e046963f") >= 0 && UWPName.Find(L"lenovo") >= 0)
				{
					hHandle = hProcess;
					bRet = TRUE;
					break;
				}
			}
			CloseHandle(hProcess);
		} while (Process32Next(snap, &proc_entry));

		CloseHandle(snap);
		return (bRet);
#endif
	}

	BOOL GetTokenByName(HANDLE &hToken, const wstring &procname)
	{
		BOOL           bRet = FALSE;
		PROCESSENTRY32 proc_entry;
		DWORD session_id = WTSGetActiveConsoleSessionId();

		HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snap == INVALID_HANDLE_VALUE) {
			return FALSE;
		}
		SecureZeroMemory(&proc_entry, sizeof(proc_entry));
		proc_entry.dwSize = sizeof(PROCESSENTRY32);
		if (!Process32First(snap, &proc_entry)) {
			CloseHandle(snap);
			return FALSE;
		}
		do
		{
			ULONG explorer_session_id = 0;

			if (!_wcsicmp(proc_entry.szExeFile, procname.c_str()))
			{
				if (!ProcessIdToSessionId(proc_entry.th32ProcessID, &explorer_session_id))
				{
					break;
				}

				if (explorer_session_id != session_id)
					continue;

				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,
					FALSE, proc_entry.th32ProcessID);
				if (hProcess)
				{
					bRet = OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken);
					CloseHandle(hProcess);
				}
				break;
			}

		} while (Process32Next(snap, &proc_entry));


		CloseHandle(snap);
		return (bRet);
	}

	BOOL GetProcessFullPath(DWORD dwPID, TCHAR pszFullPath[MAX_PATH])
	{
		TCHAR		szImagePath[MAX_PATH];
		HANDLE		hProcess;

		if (!pszFullPath)
			return FALSE;

		pszFullPath[0] = '\0';
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, dwPID);
		if (!hProcess)
		{
			return FALSE;
		}

		BOOL bRet = TRUE;
		do 
		{
			if (!GetProcessImageFileName(hProcess, szImagePath, MAX_PATH))
			{
				bRet = FALSE;
				break;
			}

			if (!DosPathToNtPath(szImagePath, pszFullPath))
			{
				bRet = FALSE;
				break;
			}
		} while (false);

		CloseHandle(hProcess);

		return bRet;
	}

	// not be called
	DWORD GetExplorerPid()
	{
		PROCESSENTRY32 proc_entry;
		DWORD explorer_pid = 0;
		DWORD agent_session_id;

		agent_session_id = WTSGetActiveConsoleSessionId();

		HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snap == INVALID_HANDLE_VALUE) {
			return 0;
		}
		SecureZeroMemory(&proc_entry, sizeof(proc_entry));
		proc_entry.dwSize = sizeof(PROCESSENTRY32);
		if (!Process32First(snap, &proc_entry)) {
			CloseHandle(snap);
			return 0;
		}
		do
		{
			if (!_wcsicmp(proc_entry.szExeFile, L"EXPLORER.EXE"))
			{
				DWORD explorer_session_id;
				if (!ProcessIdToSessionId(proc_entry.th32ProcessID, &explorer_session_id)) {
					break;
				}
				//remote connect
				if (5 == agent_session_id)
				{
					explorer_pid = proc_entry.th32ProcessID;
					break;
				}
				else if (explorer_session_id == agent_session_id)
				{
					explorer_pid = proc_entry.th32ProcessID;
					break;
				}
			}

		} while (Process32Next(snap, &proc_entry));


		CloseHandle(snap);
		if (explorer_pid == 0) {
			return 0;
		}
		return explorer_pid;
	}

	// not be called
	void ClearUserSIDInfo()
	{
		HKEY Key = NULL;
		if (RegOpenKeyEx(USER_INFO_REG_ROOT,
			USER_INFO_SID_REG_PATH,
			0,
			KEY_QUERY_VALUE | KEY_SET_VALUE,
			&Key) != ERROR_SUCCESS)
		{
			return;
		}
		RegDeleteValue(Key,
			USER_INFO_SID_REG_VALUE);

		if (Key)
			RegCloseKey(Key);
	}

	const WCHAR* GetCurrentUserSIDString()
	{
		HANDLE hToken = NULL;
		wstring username;
		wstring domainname;

		if (!GetTokenByName(hToken, L"EXPLORER.EXE"))
		{
			return NULL;
		}

		DWORD dwTemp = 0;
		char tagTokenInfoBuf[256] = { 0 };
		PTOKEN_USER tagTokenInfo = (PTOKEN_USER)tagTokenInfoBuf;
		if (!GetTokenInformation(hToken, TokenUser, tagTokenInfoBuf, sizeof(tagTokenInfoBuf),
			&dwTemp)) {
			CloseHandle(hToken);
			return NULL;
		}


		WCHAR *sidstr = NULL;
		typedef BOOL(WINAPI*CONVERT)(PSID, LPWSTR*);
		HMODULE hAdvapi = LoadLibraryEx(L"Advapi32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
		CONVERT sidToString = reinterpret_cast<CONVERT>(GetProcAddress(hAdvapi, "ConvertSidToStringSidW"));

		if (sidToString)
		{
			if (sidToString(tagTokenInfo->User.Sid, (LPWSTR*)&sidstr))
			{
				if (sidstr)
				{
					m_currentsid = sidstr;
					LocalFree(sidstr);
				}
			}
		}
		::FreeLibrary(hAdvapi);
		return m_currentsid.c_str();
	}

	BOOL IsSystemProcess()
	{
		try
		{
			BOOL                     bRet = FALSE;
			ULONG sessionid;
			ProcessIdToSessionId(GetCurrentProcessId(), &sessionid);
			if (0 == sessionid)
			{
				return TRUE;
			}
			//Not reliable in non-english system,such as Russia
			/*TCHAR szUserName[256];
			DWORD dwSize2=256;
			BOOL ret=GetUserName(szUserName,&dwSize2);
			if (ret)
			{
			if (_wcsicmp(szUserName, L"system") == 0
			|| _wcsicmp(szUserName, L"local service") == 0
			|| _wcsicmp(szUserName, L"network service") == 0)
			return TRUE;
			}*/
			{
				HANDLE                   hAccessToken;
				BYTE                     InfoBuffer[256] = { 0 };
				PTOKEN_USER            pTokenUser;
				DWORD                    dwInfoBufferSize;

				if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hAccessToken))
					return bRet;
				bRet = GetTokenInformation(hAccessToken, TokenUser, InfoBuffer, 1024, &dwInfoBufferSize);
				CloseHandle(hAccessToken);
				if (!bRet)
					return bRet;


				bRet = FALSE;
				pTokenUser = (PTOKEN_USER)InfoBuffer;
				WCHAR *sidstr = NULL;
				typedef BOOL(WINAPI*CONVERT)(PSID, LPWSTR*);
				HMODULE hAdvapi = LoadLibraryEx(L"Advapi32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
				CONVERT sidToString = reinterpret_cast<CONVERT>(GetProcAddress(hAdvapi, "ConvertSidToStringSidW"));

				if (sidToString)
				{
					CString sidstring;
					sidToString(pTokenUser->User.Sid, (LPWSTR*)&sidstr);
					sidstring = sidstr;
					if (0 == sidstring.CompareNoCase(L"s-1-5-18")
						|| 0 == sidstring.CompareNoCase(L"s-1-5-19")
						|| 0 == sidstring.CompareNoCase(L"s-1-5-20"))
						bRet = TRUE;
					if (sidstr)
						LocalFree(sidstr);
				}
				::FreeLibrary(hAdvapi);
			}
			return bRet;
		}
		catch (...)
		{
			DBG_PRINTF_FL((L"IsSystemProcess failed : CAtlException\r\n"));
		}

		return FALSE;
	}


private:
	// not be called
	BOOL ExePowerDownEx(BOOL bRealDone)
	{
		DWORD dwSessionId, dwExplorerLogonPid;
		HANDLE hProcess, hPToken;

		if (!bRealDone)
			return TRUE;

		//Get the active desktop session id
		dwSessionId = WTSGetActiveConsoleSessionId();

		//We find the explorer process since it will have the user token

		//////////////////////////////////////////
		// Find the explorer process
		////////////////////////////////////////

		PROCESSENTRY32 procEntry;

		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnap == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}

		procEntry.dwSize = sizeof(PROCESSENTRY32);

		if (!Process32First(hSnap, &procEntry))
		{
			return FALSE;
		}

		dwExplorerLogonPid = (DWORD)(-1);
		do
		{
			if (_wcsicmp(procEntry.szExeFile, L"explorer.exe") == 0)
			{
				DWORD dwExplorerSessId = 0;
				if (ProcessIdToSessionId(procEntry.th32ProcessID, &dwExplorerSessId)
					&& dwExplorerSessId == dwSessionId)
				{
					dwExplorerLogonPid = procEntry.th32ProcessID;
					break;
				}
			}

		} while (Process32Next(hSnap, &procEntry));

		if ((DWORD)(-1) == dwExplorerLogonPid)
		{
			return FALSE;
		}

		////////////////////////////////////////////////////////////////////////
		hProcess = OpenProcess(MAXIMUM_ALLOWED, FALSE, dwExplorerLogonPid);

		if (!::OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY
			| TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_ADJUST_SESSIONID
			| TOKEN_READ | TOKEN_WRITE, &hPToken))
		{
			return TRUE; ///< if openProcessToken on here, must return TRUE, else dead loop
			/// if a normal user login, this happen
		}

		BOOL bRc = ImpersonateLoggedOnUser(hPToken);
		if (!bRc)
		{
			DBG_PRINTF_FL((L"failed : ImpersonateLoggedOnUser\r\n"));
		}

		return bRc;
	}

public:
	// not be called
	void ExePowerDown(BOOL bRealDone /*= FALSE*/)
	{
		if (m_bpowerdown)
			return;
		for (int i = 0; i < 5; i++)
		{
			m_bpowerdown = ExePowerDownEx(bRealDone);
			if (m_bpowerdown)
				break;
			Sleep(1000); ///< if Exe Power Down not successful, we can't do other action, the action will failed
			/// if user entry desktop, ExePowerDownEx will successful
		}

		Sleep(200); ///< let Exe Power effect...
	}

	// not be called
	void ExePowerUp(BOOL bRealDone)
	{
		if (!bRealDone)
			return;
		if (m_bpowerdown == FALSE)
			return;
		RevertToSelf();
		m_bpowerdown = FALSE;
	}

	// not be called
	BOOL IsProcessAdminPrivilege()
	{
		BOOL bIsAdmin = FALSE;
		BOOL bRet = FALSE;
		SID_IDENTIFIER_AUTHORITY idetifier = SECURITY_NT_AUTHORITY;
		PSID pAdministratorGroup;
		if (AllocateAndInitializeSid(
			&idetifier,
			2,
			SECURITY_BUILTIN_DOMAIN_RID,
			DOMAIN_ALIAS_RID_ADMINS,
			0, 0, 0, 0, 0, 0,
			&pAdministratorGroup))
		{
			if (!CheckTokenMembership(NULL, pAdministratorGroup, &bRet))
			{
				bIsAdmin = FALSE;
			}
			if (bRet)
			{
				bIsAdmin = TRUE;
			}
			FreeSid(pAdministratorGroup);
		}

		return bIsAdmin;
	}

	// not be called
	bool GetSystemidleState()
	{
		static DWORD PreviousTime = 0;
		LASTINPUTINFO inputinfo;
		inputinfo.cbSize = sizeof(LASTINPUTINFO);
		//	ImpersonateCurrentUser();
		//To get the lastinputinfo the process has to be launched with createprocessasuser, where as this is bieng luanched in system context
		//hence it returns the last input since begining of service start
		//Impersonation does not work
		GetLastInputInfo(&inputinfo); //does not work because dependency installer is called from system context and impersonate here does not help
		//	RevertToSelf();

#pragma warning(suppress: 28159)
		DWORD idleSeconds = (GetTickCount()) / 1000;
		PreviousTime = inputinfo.dwTime / 1000;

		if ((idleSeconds - PreviousTime) > 10)
		{
			DBG_PRINTF_FL((L"System is in IdleState for %d seconds...\n", (idleSeconds - PreviousTime)));
			//Before doing the below taks also see if lenovo settings is suspended or not
			//When system is idle for more than 10 seconds perform the install operation one after another
			//after installing one application check if the pc is stil in idle state
			return true;
		}
		else
		{
			DBG_PRINTF_FL((L"System is not in IdleState, Last input was %d seconds before...\n", (idleSeconds - PreviousTime)));
			return false;
		}
	}

	// not be called
	LPCTSTR GetWindowsDir()
	{
		static	TCHAR	szPath[MAX_PATH] = { 0 };
		if (szPath[0] == 0)
		{
			GetSystemWindowsDirectory(szPath, MAX_PATH);
			//	GetWindowsDirectory(szPath, MAX_PATH);
		}
		return szPath;
	}


public:
	ULONG m_exploerpid;
	wstring m_currentsid;
	wstring m_username;
	BOOL m_bpowerdown;
};


