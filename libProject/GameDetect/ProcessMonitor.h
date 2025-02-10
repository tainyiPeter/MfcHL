

#pragma once
//#include "GameWhiteListChecker.h"
//#include "helper/gaming_wmi_helper.h"
//#include "inc/gpu_memory_usage.h"

#include "helper/gaming_wmi_helper.h"
#include "inc/LockObject.h"

#include <any>
#include <map>
#include <memory>
#include <thread>
#include <string>
#include <unordered_map>
#include <windows.h>
#include <future>


#define WMI_CONNET_SPACE L"ROOT\\cimv2"


class CProcessMonitor
{
public:
	static CProcessMonitor* getinstance(bool destroy=false)
	{
		static std::unique_ptr<CProcessMonitor> myInstance;
		if (destroy)
		{
			myInstance = nullptr;
			return nullptr;
		}

		if (myInstance == nullptr)
			myInstance.reset(new CProcessMonitor);
		return myInstance.get();
	}

private:
	CProcessMonitor();

public:
	~CProcessMonitor();

	BOOL StartMonitorProcess(ULONG flag = 0);
	BOOL StopMonitorProcess();

	void Callback(bool bCreateProcess, std::unordered_map<std::wstring, std::any> result);

	BOOL IsMonitorStarted() const
	{
		if (proc_start_monitor_id_ > 0)
			return TRUE;
		return FALSE;
	}

	

private:
	
	void CreateProcessMonitorGame();
	static DWORD WINAPI _Thread_MonitorProcess(PVOID context);
	bool m_benvready;
	static DWORD WINAPI _Thread_GameCheck(LPVOID lParam);
	HANDLE m_autodetecthandle = NULL;

	HANDLE m_killprocessmon = NULL;
	BOOL	m_bstopprocmon = FALSE;;

	VOID StopMonitor();
	BOOL CreateProcessCreateMonitor();
	BOOL CreateProcessDeleteMonitor();

	gaming_wmi_helper wmi_helper_;
	std::future<DWORD> m_CheckThread;

	int proc_start_monitor_id_ = 0;
	int proc_stop_monitor_id_ = 0;

	BOOL m_bautogamemode;
	ThreadSafeMap<UINT, ULONG> m_lastproclist;
};