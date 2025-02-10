// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once

#include <windows.h>
#include <functional>
#include "RegUtil.h"
#include "DebugUtil.h"
#include "ScopeGuard.h"
#include "../define/GZDefine.h"

class CServiceHelper
{
public:

	CServiceHelper()
	{
		mEnvironmentIndex = 0;
	}

	~CServiceHelper()
	{
	}

	static BOOL GetIsXTUInstalled(LPCTSTR serviceName)
	{
		SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);

		if (hSCM != NULL)
		{
			SC_HANDLE hService = ::OpenService(hSCM, serviceName, SERVICE_QUERY_CONFIG);
			if (hService != NULL)
			{
				::CloseServiceHandle(hService);
				::CloseServiceHandle(hSCM);
				return TRUE;
			}
			::CloseServiceHandle(hSCM);
		}
		return FALSE;
	}

	static bool GetServiceInstallPath(LPCTSTR serviceName, std::string &service_path_name)
	{
		SC_HANDLE schSCManager;
		SC_HANDLE schService;
		bool result = false;
		// Get a handle to the SCM database. 

		schSCManager = OpenSCManager(
			NULL,                    // local computer
			NULL,                    // servicesActive database 
			SC_MANAGER_ENUMERATE_SERVICE | SC_MANAGER_CONNECT);  // full access rights 

		if (NULL == schSCManager)
		{
			DBG_PRINTF_FL((L"CServiceHelper::CheckSrvStatus OpenSCManager failed (%d)\n", GetLastError()));
			return false;
		}

		// Get a handle to the service.
		__try
		{
			schService = OpenService(
				schSCManager,         // SCM database 
				serviceName/*L"XTU3SERVICE"*/,            // name of service 
				SERVICE_QUERY_CONFIG);  // full access 

			if (schService == NULL)
			{
				DBG_PRINTF_FL((L"CServiceHelper::CheckSrvStatus OpenService failed (%d)\n", GetLastError()));
				CloseServiceHandle(schSCManager);
				return false;
			}

			//if service is disable, skip cpu oc. 
			DWORD nNeedSize = 0;
			DWORD     nResumeHandle = 0;
			LPQUERY_SERVICE_CONFIGA ServicesInfo = NULL;
			::QueryServiceConfigA(schService, NULL, 0, &nNeedSize);
			if (nNeedSize > 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				ServicesInfo = (LPQUERY_SERVICE_CONFIGA)malloc(nNeedSize);
				if (ServicesInfo && QueryServiceConfigA(schService, ServicesInfo, nNeedSize, &nResumeHandle))
				{
					service_path_name = ServicesInfo->lpBinaryPathName;
					DBG_PRINTF_FL((L"CServiceHelper service path name (%S)\n", service_path_name.c_str()));
					free(ServicesInfo);
					result = true;
				}
			}

			CloseServiceHandle(schService);
		}
		__except (1)
		{
		}

		CloseServiceHandle(schSCManager);

		return result;
	}

	static bool CheckSrvStatus(LPCTSTR serviceName,UINT &stats, bool bstartsvc = FALSE)
	{
		SC_HANDLE schSCManager;
		SC_HANDLE schService;
		bool result = false;
		// Get a handle to the SCM database. 

		schSCManager = OpenSCManager(
			NULL,                    // local computer
			NULL,                    // servicesActive database 
			SC_MANAGER_ENUMERATE_SERVICE | SC_MANAGER_CONNECT);  // full access rights 

		if (NULL == schSCManager)
		{
			DBG_PRINTF_FL((L"CServiceHelper::CheckSrvStatus OpenSCManager failed (%d)\n", GetLastError()));
			return false;
		}

		// Get a handle to the service.
		__try
		{
			schService = OpenService(
				schSCManager,         // SCM database 
				serviceName/*L"XTU3SERVICE"*/,            // name of service 
				bstartsvc ? SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_QUERY_CONFIG : SERVICE_QUERY_STATUS | SERVICE_QUERY_CONFIG);  // full access 

			if (schService == NULL)
			{
				DBG_PRINTF_FL((L"CServiceHelper::CheckSrvStatus OpenService failed (%d)\n", GetLastError()));
				CloseServiceHandle(schSCManager);
				return false;
			}

			SERVICE_STATUS status;
			if (!bstartsvc) {
				if (QueryServiceStatus(schService, &status)) {
					stats = status.dwCurrentState;
					result = true;
				}
			}
			else {
				if (TRUE == QueryServiceStatus(schService, &status))
				{
						if (status.dwCurrentState == SERVICE_STOPPED) {

						//if service is disable, skip cpu oc. 
						DWORD nNeedSize = 0;
						DWORD     nResumeHandle = 0;
						DWORD   dwStartType = SERVICE_DISABLED;
						LPQUERY_SERVICE_CONFIG ServicesInfo = NULL;
						::QueryServiceConfig(schService, NULL, 0, &nNeedSize);
						if (nNeedSize > 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
						{
							ServicesInfo = (LPQUERY_SERVICE_CONFIG)malloc(nNeedSize);
							if (ServicesInfo&&QueryServiceConfig(schService, ServicesInfo, nNeedSize, &nResumeHandle))
							{
								dwStartType = ServicesInfo->dwStartType;
								DBG_PRINTF_FL((L"CServiceHelper::CheckSrvStatus service status(%d)\n", dwStartType));
								free(ServicesInfo);
							}
						}
						if (dwStartType == SERVICE_DISABLED)
						{
							DBG_PRINTF_FL((L"CServiceHelper::CheckSrvStatus service is disable!!!\n"));
							CloseServiceHandle(schService);
							CloseServiceHandle(schSCManager);
							return false;
						}
						//SERVICE_STATUS status;
						//if (TRUE == QueryServiceStatus(schService, &status))
						//{
							//if (/*status.dwCurrentState == SERVICE_STOPPED &&*/ bstartsvc)
							//{
								if (StartService(schService, 0, NULL))
									QueryServiceStatus(schService, &status);
							//}
						
					//}
					}
					stats = status.dwCurrentState;

					result = true;
				}
			}

			CloseServiceHandle(schService);
		}
		__except (1)
		{
		}

		CloseServiceHandle(schSCManager);

		return result;
	}

	bool HasEnvironmentInPath(LPCTSTR serviceName, LPCTSTR environmentLower)
	{
		if (nullptr == serviceName || nullptr == environmentLower)
		{
			return false;
		}

		CServiceHelper::QuerySvcPath(serviceName, mErrorServicePath);
		mErrorServicePath = mErrorServicePath.MakeLower();
		mEnvironmentIndex = mErrorServicePath.Find(environmentLower);
		if (mEnvironmentIndex >= 0)
		{
			mEnvironmentIndex += (int)wcsnlen_s(environmentLower, MAX_PATH);

			//ignore %
			int index = mErrorServicePath.Find(L"\\", mEnvironmentIndex);
			mEnvironmentIndex = index;

			return true;
		}
		//doesn't have environment in path
		if (!mErrorServicePath.IsEmpty())
		{
			mCorrectServicePath = mErrorServicePath;
			mErrorServicePath = L"";
		}

		return false;
	}

	// not be called
	bool ReplaceEnvironmentInPath(LPCTSTR serviceRegPath, LPCTSTR key, 
		LPCTSTR environmentLower, int folderType)
	{
		if (mErrorServicePath.IsEmpty())
			return false;

		wchar_t wcDefaultDir[MAX_PATH] = { 0 };
		if (SHGetSpecialFolderPath(NULL, wcDefaultDir, folderType, false))
		{
			mErrorServicePath = mErrorServicePath.Mid(mEnvironmentIndex, mErrorServicePath.GetLength());
			mErrorServicePath = wcDefaultDir + mErrorServicePath;
			mErrorServicePath = mErrorServicePath.MakeLower();

			if (PathFileExists(mErrorServicePath.GetBuffer()))
			{
				CRegUtil().SetKeyValue(HKEY_LOCAL_MACHINE,
					serviceRegPath,
					key,
					(LPVOID)mErrorServicePath.GetBuffer(),
					mErrorServicePath.GetLength() * sizeof(WCHAR),
					REG_SZ,
					TRUE);

				mCorrectServicePath = mErrorServicePath;
				mErrorServicePath = L"";
				return true;
			}
		}

		return false;
	}

	bool QuerySvcPath(LPCTSTR serviceName, CString &servicePath)
	{
		SC_HANDLE schSCManager = NULL;
		SC_HANDLE schService = NULL;
		LPQUERY_SERVICE_CONFIG lpsc = NULL;
		DWORD dwBytesNeeded=0, cbBufSize=0, dwError=0;

		schSCManager = OpenSCManager(
			NULL,                   
			NULL,                    
			SC_MANAGER_ALL_ACCESS); 

		if (NULL == schSCManager)
		{
			DBG_PRINTF_FL((L"CServiceHelper::QuerySvcPath OpenSCManager failed (%d)\n", GetLastError()));
			return false;
		}

		schService = OpenService(
			schSCManager,       
			serviceName,
			SERVICE_QUERY_CONFIG); 

		if (schService == NULL)
		{
			DBG_PRINTF_FL((L"CServiceHelper::QuerySvcPath OpenService failed (%d)\n", GetLastError()));
			CloseServiceHandle(schSCManager);
			return false;
		}

		if (!QueryServiceConfig(
			schService,
			NULL,
			0,
			&dwBytesNeeded))
		{
			dwError = GetLastError();
			if (ERROR_INSUFFICIENT_BUFFER == dwError)
			{
				cbBufSize = dwBytesNeeded;
				lpsc = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LMEM_FIXED, cbBufSize);

				if (!QueryServiceConfig(
					schService,
					lpsc,
					cbBufSize,
					&dwBytesNeeded))
				{
					DBG_PRINTF_FL((L"CServiceHelper::QuerySvcPath QueryServiceConfig failed (%d)\n", GetLastError()));
				}
				else
					servicePath = lpsc->lpBinaryPathName;
			}
			else
			{
				DBG_PRINTF_FL((L"CServiceHelper::QuerySvcPath QueryServiceConfig failed (%d)\n", GetLastError()));
			}
		}	

		if (lpsc != NULL)
			LocalFree(lpsc);
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		return true;
	}

	private:
		CString mErrorServicePath;
		CString mCorrectServicePath;
		int mEnvironmentIndex;
};

// not be called
class CXTUServiceHelper
{
public:
	static bool StartXtuByChangePath(UINT &stats)
	{
		//After PBR, XTU service can't be started in RS6.
		//The reason is the service path displaying in the ServiceManager turned into "systemroot\wowsystem32\..."
		// from "c:\windows\wowsystem64\..." after PBR. Plugin can change the path in the registry to fix it.
		CServiceHelper svrHelper;
		if (svrHelper.HasEnvironmentInPath(XTUSERVICENAME, L"systemroot"))
		{
			if (svrHelper.ReplaceEnvironmentInPath(L"SYSTEM\\CurrentControlSet\\Services\\XTU3SERVICE",
				L"ImagePath",
				L"systemroot", CSIDL_WINDOWS))
			{
				CServiceHelper::CheckSrvStatus(XTUSERVICENAME, stats, true);
				if (stats != SERVICE_STOPPED)
					return true;
			}
		}

		return false;
	}
};
