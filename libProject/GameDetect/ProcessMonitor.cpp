#include "ProcessMonitor.h"
#include "GPUManager.hpp"

#include <tlhelp32.h>
#include <ShlObj.h>
#include <WtsApi32.h>
#include <time.h>

//#include "inc/BiosInfo.h"
//#include "inc/CBinaryFileValidatorProxy.h"
//#include "GameStatusMonitor.hpp"

extern bool g_bunload;
#pragma comment(lib, "wtsapi32.lib")
#define ADK_TASK_NAME L"Axe Restart Task"

#define SIGN_NAME L"SignName"

#define cache_process_count 40


std::vector<std::wstring> g_UncheckCompanyList = {
	L"Autodesk, Inc.",//3d max
	L"Futuremark Corporation", //3d mark
	L"Futuremark, Inc.", //3d mark
	//maya 2014 same as 3d max
	/*
	companyname = L"Autodesk"
	filedesp = L"Maya application file"
	procsignature = L"Autodesk, Inc"
	*/
	//Softimage 2015 same as 3d max
	/*
	companyname = L"Autodesk, Inc."
	*/
	//Cimatron E10
	L"Cimatron Ltd.",//companyname
	//L"EliteLauncher DLL"//filedesp
	L"PassMark Software Pty Ltd"
};

//CString g_UncheckKeyStrings[] = {
//	L"lenovo",
//};

std::vector<std::wstring> g_UncheckProcess = {
	L"LeagueClientUxRender.exe",//filter this process for bios game list
	L"C:\\Windows\\System32\\",
	L"C:\\Windows\\Lenovo\\"
};

CProcessMonitor::CProcessMonitor():wmi_helper_(WMI_CONNET_SPACE),
proc_start_monitor_id_(0),
proc_stop_monitor_id_(0)
{


}


CProcessMonitor::~CProcessMonitor()
{
}

VOID CProcessMonitor::StopMonitor()
{
	if (proc_start_monitor_id_>0)
		wmi_helper_.stop_event(L"SELECT * FROM  Win32_ProcessStartTrace", proc_start_monitor_id_);
	if (proc_stop_monitor_id_>0)
		wmi_helper_.stop_event(L"SELECT * FROM  Win32_ProcessStopTrace", proc_stop_monitor_id_);
}

BOOL CProcessMonitor::CreateProcessCreateMonitor()
{
	proc_start_monitor_id_ = wmi_helper_.register_event(L"SELECT * FROM  Win32_ProcessStartTrace",
	                           [&](std::unordered_map<std::wstring, std::any> result)
	                           {									
									Callback(true, std::move(result));								
									return true;
	                           });
	return TRUE;
}


BOOL CProcessMonitor::CreateProcessDeleteMonitor()
{
	proc_stop_monitor_id_ = wmi_helper_.register_event(L"SELECT * FROM  Win32_ProcessStopTrace",
		[&](std::unordered_map<std::wstring, std::any> result)
		{
			Callback(false, std::move(result));
			return true;
		});

	return TRUE;
}

DWORD CProcessMonitor::_Thread_MonitorProcess(PVOID context)
{
	CProcessMonitor* procmon = (CProcessMonitor*)context;
	if (procmon->m_bstopprocmon)
		return 0;

	////for gamezone 2.0 enable auto gamedetect for all product 20160627 by luxd
	//procmon->CreateProcessMonitorGame();

	//procmon->m_benvready = true;

	//if (procmon->m_bstopprocmon)
	//	return 0;

	if (!procmon->CreateProcessCreateMonitor())
	{
		//DBG_PRINTF_FL((L"Monitor Create Process failed!\n"));
		procmon->StopMonitor();
		return 0;
	}

	if (!procmon->CreateProcessDeleteMonitor())
	{
		//DBG_PRINTF_FL((L"Monitor Delete Process failed!\n"));
		procmon->StopMonitor();
		return 0;
	}

	std::thread([procmon]()
		{
		// DBG_PRINTF_FL((L"Start to monitor game....\n"));
		WaitForSingleObject(procmon->m_killprocessmon, INFINITE);		
		procmon->StopMonitor();
		SetEvent(procmon->m_killprocessmon);
		}).detach();


	return 0;
}

DWORD __stdcall CProcessMonitor::_Thread_GameCheck(LPVOID lParam)
{
	CProcessMonitor* pThreadParam = (CProcessMonitor*)lParam;

	if (GPUManager::getInstance()->GetGPUCount() <= 0)
		return 0;

	vector<HANDLE> hList;
	hList.push_back(pThreadParam->m_autodetecthandle);

	HANDLE hGpuEvent = NULL;
	GPUManager::getInstance()->Initialize(hGpuEvent, &(pThreadParam->m_lastproclist));
	if (hGpuEvent != NULL)
		hList.push_back(hGpuEvent);

	if (!pThreadParam->m_bautogamemode)
	{
		DBG_PRINTF_FL((L"Game:loading check pause.\n"));
	}

	do
	{
		bool InGameMode = true;
		GPUManager::getInstance()->StartMonitor(InGameMode);
		DWORD dwRet = WaitForMultipleObjects((DWORD)hList.size(), hList.data(), FALSE, INFINITE);
		DBG_PRINTF_FL((L"Card Num %d\n", dwRet));

		if (pThreadParam->m_bstopprocmon)
		{
			DBG_PRINTF_FL((L"Game:Stop GPU loading check.\n"));
			goto CLEAN;
		}

		//if (pThreadParam->m_bautogamemode == FALSE)
		//{
		//	continue;
		//}

		{
			GPUManager::getInstance()->ChangeCheckTime(CHECK_GPU_LOAD_TIME_S);
		}

		//if (InGameMode)
		//{
		//	continue;
		//}

		if (GPUManager::getInstance()->CheckGPULoading())
		{
			GPUManager::getInstance()->EndMonitor();

			DBG_PRINTF_FL((L"Fit global gpu load standard, start check gpu process\n"));
			map<UINT, ULONG> proclist = pThreadParam->m_lastproclist.Get();
			CUserPrivilegeUtil userpriv;
			WCHAR fileshortname[MAX_PATH] = { 0 };
			WCHAR filefullname[MAX_PATH] = { 0 };

			for (auto pinfo : proclist)
			{
				if (pinfo.second == 0)
					continue;
				BOOL checkresult = FALSE;
				SecureZeroMemory(fileshortname, sizeof(WCHAR) * MAX_PATH);
				SecureZeroMemory(filefullname, sizeof(WCHAR) * MAX_PATH);
				if (FALSE == userpriv.GetProcessFullPath(pinfo.second, filefullname))
				{
					DBG_PRINTF_FL((L"Check process (%d) fullname failed in last list.\n", pinfo.second));
				}

				CFileInfo().SplitFileandPath(filefullname, NULL, fileshortname, TRUE);
				if (pThreadParam->m_bstopprocmon)
					break;

				DBG_PRINTF_FL((L"Check process %s (%d) in last list.\n", fileshortname, pinfo.second));

				//only test				
				//checkresult = pThreadParam->m_gamechecker->CheckProcessIsGame(pinfo.second,
				//	fileshortname,
				//	filefullname,
				//	TRUE);

				if (checkresult)
					break;
			}
		}
	} while (1);
CLEAN:
	GPUManager::getInstance()->EndMonitor();
	return 0;
}

BOOL CProcessMonitor::StartMonitorProcess(ULONG flag)
{
	std::thread t = std::thread(_Thread_MonitorProcess, this);
	t.join();
	return FALSE;
}

BOOL CProcessMonitor::StopMonitorProcess()
{
	//m_bautogamemode = FALSE;

	if (m_autodetecthandle)
	{
		SetEvent(m_autodetecthandle);
		m_CheckThread.wait();
		CloseHandle(m_autodetecthandle);
		m_autodetecthandle = NULL;
	}
	if (m_killprocessmon)
	{
		SetEvent(m_killprocessmon);
		WaitForSingleObject(m_killprocessmon, 2000);
		CloseHandle(m_killprocessmon);
		m_killprocessmon = NULL;
	}

	return TRUE;
}

void CProcessMonitor::Callback(bool bCreateProcess, std::unordered_map<std::wstring, std::any> result)
{
	//HRESULT hres = S_OK;
	//CUserPrivilegeUtil userpriv;

	//WCHAR filefullname[512] = { 0 };

	//if (m_bstopprocmon)
	//	return;
	//if (bCreateProcess && !m_bautogamemode)
	//	return;

	//wstring gamefilepath;
	//ULONG pid = 0;
	//wstring procname = L"";
	//try
	//{
	//	if (result.find(L"ProcessID") != result.end())
	//		pid = std::any_cast<ULONG>(result[L"ProcessID"]);

	//	if (pid==0)
	//		return;

	//	if (result.find(L"ProcessName") != result.end())			
	//		procname = std::any_cast<wstring>(result[L"ProcessName"]);
	//	
	//	if (bCreateProcess)
	//	{
	//		if (FALSE == userpriv.GetProcessFullPath(pid, filefullname))
	//			return;

	//		gamefilepath = filefullname;
	//	}
	//	else
	//	{

	//	}
	//}
	//catch (...)
	//{
	//	DBG_PRINTF_FL((L"fatal error in monitor process!!!\n"));
	//}
}

void CProcessMonitor::CreateProcessMonitorGame()
{
	DWORD ThreadId;
	m_autodetecthandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_autodetecthandle == NULL)
	{
		//DBG_PRINTF_FL((L"Create auto detect handle failed\n"));
		return;
	}

	m_CheckThread = std::async(std::launch::async, &_Thread_GameCheck, this);


}