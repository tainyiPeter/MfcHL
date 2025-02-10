// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include "inc/xtu_wrapper_helper.hpp"
#include "CThread.h"
#include "AutoLocker.h"
#include "DebugUtil.h"
#include "ServiceHelper.h"

#include "7.9/SdkWrapperForNativeCode/SdkWrapperForNativeCode.h"

using namespace SdkWrapperLibrary;

typedef 	SdkWrapperForNativeCode*(*pfnGetSdkWrapperObj)();


class CCPUInfoFromXTU
{
public:

	CCPUInfoFromXTU()
	{
		m_cpucurrentTemp = 0;
		m_cpureadevent = NULL;		
		m_bxtustop = false;
		m_XTUhelper = NULL;		
		m_bxtuinitdone = false;
		m_xtusdklib = NULL;
		cpuTempSubThread = NULL;
	}

	~CCPUInfoFromXTU()
	{
		if (cpuTempSubThread != NULL)
		{
			WaitForSingleObject(cpuTempSubThread, INFINITE);
		}
	}


	float GetCpuTemp()
	{
		DWORD ThreadId;  //Create a new thread for monitoring
		if (m_cpureadevent == NULL)
		{
			m_cpureadevent = CreateEvent(NULL, FALSE, FALSE, NULL);
			cpuTempSubThread = CreateThread(NULL, 0, _ThreadProc_GetCpuTemp, this, 0, &ThreadId);
		}
		else
			SetEvent(m_cpureadevent);
		return m_cpucurrentTemp;
	}



	BOOL InitXTU(bool bstartsvc, bool bsyncdone)
	{
		CAutoLocker l(m_initxtulocker);
		if (m_bxtuinitdone == true)
		{
			DBG_PRINTF_FL((L"InitXTU:XTU already init!\n"));
			return TRUE;
		}
		BOOL result = FALSE;
		try
		{
			UINT xtusvcstats;
		CHECK:
			if (!CServiceHelper::CheckSrvStatus(XTUSERVICENAME, xtusvcstats, bstartsvc))
			{
				DBG_PRINTF_FL((L"Initialize XTU Service not install!\n"));
				m_bxtustop = true;
				throw 1;
			}
			else
			{
			/*	if (bstartsvc)
				{
					if (xtusvcstats == SERVICE_STOPPED)
					{
						DBG_PRINTF_FL((L"XTU service can't be started.\n"));
						CXTUServiceHelper::StartXtuByChangePath(xtusvcstats);
					}
				}*/

				if (xtusvcstats == SERVICE_RUNNING)
				{
					m_xtusdklib = xtu_wrapper_helper::load_xtu_wrapper();
					if (m_xtusdklib != NULL)
					{
						pfnGetSdkWrapperObj GetXTUSDKObjFn = (pfnGetSdkWrapperObj)::GetProcAddress(m_xtusdklib, "GetSdkWrapperObj");
						if (GetXTUSDKObjFn)
						{
							m_XTUhelper = GetXTUSDKObjFn();
							DBG_PRINTF_FL((L"Get xtu wrapper obj done.\n"));
						}
						else
							DBG_PRINTF_FL((L"Can't get xtu wrapper obj function!\n"));
					}
					else
						DBG_PRINTF_FL((L"Can't load xtu wrapper dll!\n"));
					//m_XTUhelper = new SdkWrapperForNativeCode();

					//XTU initialize
					if (m_XTUhelper == NULL)
						return FALSE;
					m_XTUhelper->Initialize(INIT_XTU_LIB_FLAG_MONITOR);
					if (m_XTUhelper->m_bInitMonitorLib)
					{
						m_bxtuinitdone = true;
						DBG_PRINTF_FL((L"Initialize XTU done.\n"));
						result = TRUE;
					}
					else
					{
						DBG_PRINTF_FL((L"Initialize XTU fatal2 error!\n"));
						//delete m_XTUhelper;
						m_XTUhelper = NULL;
						return FALSE;
					}
				}
				else
				{
					if (bsyncdone)
					{
						Sleep(100);
						goto CHECK;
					}
					else
					{
						CThread<CCPUInfoFromXTU> threadTray(*this, [](PVOID param) -> DWORD {
							CCPUInfoFromXTU *cpuinfo = (CCPUInfoFromXTU *)param;
							cpuinfo->InitXTU(true,false);
							return 1;
						});
					}
				}
			}
		}
		catch (...)
		{
			m_XTUhelper = NULL;
			DBG_PRINTF_FL((L"Initialize XTU fatal error!\n"));
		}

		return result;
	}

private:
	static DWORD WINAPI _ThreadProc_GetCpuTemp(LPVOID lParam)
	{
		CCPUInfoFromXTU *ochelper = (CCPUInfoFromXTU*)(lParam);
		do
		{
			DWORD ret = WaitForSingleObject(ochelper->m_cpureadevent, 5000);
			if (ret == WAIT_TIMEOUT)
			{
				/*if (ochelper->m_bxtuinitdone
					&&ochelper->m_XTUhelper)
				{
					ochelper->m_XTUhelper->StopMonitor();
				}*/
				CloseHandle(ochelper->m_cpureadevent);
				ochelper->m_cpureadevent = NULL;
				break;
			}
			if (ochelper->m_bxtuinitdone
				&&ochelper->m_XTUhelper)
			{
				try
				{
					ochelper->m_cpucurrentTemp = (float)ochelper->m_XTUhelper->GetMonitorValue(7);
					DBG_PRINTF_FL((L"Cpu temperature %f\n", ochelper->m_cpucurrentTemp));
				}
				catch (...)
				{
					ochelper->m_bxtuinitdone = false;
					delete ochelper->m_XTUhelper;
					ochelper->m_XTUhelper = NULL;
					DBG_PRINTF_FL((L"XTU: Fatal error!\n"));
				}

			}
			else
			{
				ochelper->m_cpucurrentTemp = 0;
				DBG_PRINTF_FL((L"Get Cpu frequency failed!\n"));
			}

		} while (1);


		return 0;
	}


	float m_cpucurrentTemp;
	HANDLE m_cpureadevent;
	HANDLE cpuTempSubThread;  //Create a new thread for monitoring
	CriticalSection m_initxtulocker;
	bool m_bxtustop;	
	XTUSdkWrapperObj * m_XTUhelper;	
	HMODULE m_xtusdklib;



public:
	bool m_bxtuinitdone;

};


