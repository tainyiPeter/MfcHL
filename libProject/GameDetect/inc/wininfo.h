// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary


#ifndef _WIN_INFO_H_

#define _WIN_INFO_H_

#pragma once
#include <windows.h>
#include "RegUtil.h"
#include "UserPrivilegeUtil.h"

#define WINDOWS_ANCIENT 0
#define WINDOWS_XP 51
#define WINDOWS_SERVER_2003 52
#define WINDOWS_VISTA 60
#define WINDOWS_7 61
#define WINDOWS_8 62
#define WINDOWS_8_1 63
#define WINDOWS_10 100
#define WINDOWS_NEW MAXLONG
typedef _Return_type_success_(return >= 0) LONG NTSTATUS;
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

typedef
NTSTATUS
(NTAPI* _RtlGetVersion)(
	_Out_ PRTL_OSVERSIONINFOW lpVersionInformation
	);
_RtlGetVersion RtlGetVersion;


RTL_OSVERSIONINFOEXW PhOsVersion;

NTSTATUS NTAPI UT_RtlGetVersion(_Out_ PRTL_OSVERSIONINFOW lpVersionInformation)
{
	return (nullptr != RtlGetVersion) ? RtlGetVersion(lpVersionInformation) : -1;
}

class CWindowsInfo
{
public:

	CWindowsInfo()
	{
	}

	~CWindowsInfo()
	{
	}




	static VOID GetWindowsVersion(ULONG &WindowsVersion)
	{
		RTL_OSVERSIONINFOEXW versionInfo;
		ULONG majorVersion;
		ULONG minorVersion;

		HMODULE            hNtDll = NULL;
		WindowsVersion = WINDOWS_10;
		try
		{
			hNtDll = LoadLibraryEx(L"NtDll.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
			if (hNtDll == NULL)
			{
				return;
			}

			RtlGetVersion = (_RtlGetVersion)GetProcAddress(hNtDll,
				"RtlGetVersion");
			if (RtlGetVersion == NULL)
			{
				FreeLibrary(hNtDll);
				return;
			}

		}
		catch (...)
		{
			return;
		}

		versionInfo.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);

		if (!NT_SUCCESS(UT_RtlGetVersion((PRTL_OSVERSIONINFOW)&versionInfo)))
		{
			//PhShowWarning(
			//    NULL,
			//    L"Unable to determine the Windows version. "
			//    L"Some functionality may not work as expected."
			//    );
			WindowsVersion = WINDOWS_NEW;
			FreeLibrary(hNtDll);
			return;
		}

		memcpy_s(&PhOsVersion, sizeof(RTL_OSVERSIONINFOEXW), &versionInfo, sizeof(RTL_OSVERSIONINFOEXW));
		majorVersion = versionInfo.dwMajorVersion;
		minorVersion = versionInfo.dwMinorVersion;

		if (majorVersion == 5 && minorVersion < 1 || majorVersion < 5)
		{
			WindowsVersion = WINDOWS_ANCIENT;
		}
		/* Windows XP */
		else if (majorVersion == 5 && minorVersion == 1)
		{
			WindowsVersion = WINDOWS_XP;
		}
		/* Windows Server 2003 */
		else if (majorVersion == 5 && minorVersion == 2)
		{
			WindowsVersion = WINDOWS_SERVER_2003;
		}
		/* Windows Vista, Windows Server 2008 */
		else if (majorVersion == 6 && minorVersion == 0)
		{
			WindowsVersion = WINDOWS_VISTA;
		}
		/* Windows 7, Windows Server 2008 R2 */
		else if (majorVersion == 6 && minorVersion == 1)
		{
			WindowsVersion = WINDOWS_7;
		}
		/* Windows 8 */
		else if (majorVersion == 6 && minorVersion == 2)
		{
			WindowsVersion = WINDOWS_8;
		}
		/* Windows 8.1 */
		else if (majorVersion == 6 && minorVersion == 3)
		{
			WindowsVersion = WINDOWS_8_1;
		}
		/* Windows 10 */
		else if (majorVersion == 10 && minorVersion == 0)
		{
			WindowsVersion = WINDOWS_10;
		}
		else if (majorVersion == 10 && minorVersion > 0 || majorVersion > 10)
		{
			WindowsVersion = WINDOWS_NEW;
		}

		FreeLibrary(hNtDll);
	}


#define PARENT_SETUP_KEY_PATH				L"system\\Setup"
#define SETUP_CmdLine_VALUE					L"CmdLine"
#define SETUP_OOBEInProgress_VALUE			L"OOBEInProgress"
#define SETUP_SetupPhase_VALUE				L"SetupPhase"
};

#endif // !WIN_INFO_H