
#pragma once
#include "windows.h"
#include <vector>
//#include <inc/addin_manager_client.h>

#include "ProcessMonitor.h"
#include "Powerbase.h"
#include "Powrprof.h"
#pragma comment(lib,"Powrprof.lib")

using namespace std;


class CGameStatusMonitor
{
//public:
//	static CGameStatusMonitor* getinstance(bool destroy=false)
//	{
//		static unique_ptr<CGameStatusMonitor> myInstance;
//		if (destroy)
//		{
//			myInstance = nullptr;
//			return nullptr;
//		}
//
//		if (myInstance == nullptr)
//			myInstance.reset(new CGameStatusMonitor);
//		return myInstance.get();
//	}
//private:
//
//	CGameStatusMonitor()
//	{
//		m_bkillnotifythread = false;
//		m_brenoitfy = false;
//		m_lastgamestatus = 0xf;
//		m_gamerunning = NULL;
//		m_gameexit = NULL;
//		m_status = GAME_UNKNOWN;
//	}
//public:
//	~CGameStatusMonitor()
//	{		
//
//	}
//public:
//	void NotifyGameStart()
//	{
//		if (m_gamerunning)
//			SetEvent(m_gamerunning);
//
//
//	}
//
//	void NotifyGameExit()
//	{
//		if (m_gameexit)
//			SetEvent(m_gameexit);
//	}
//
//
//
//	DWORD RenotifyGameStatus(PVOID context)
//	{
//		UNREFERENCED_PARAMETER(context);
//		CAutoLocker l(m_renotifylock);
//		m_brenoitfy = true;
//		DBG_PRINTF_FL((L"re-notify user for wake trigger. \n"));
//
//		Sleep(3000);//wait for resume enable oc, lock tp
//
//
//		if (m_status == GAME_RUNNING) {
//			NotifyGameStart();
//		}
//
//		m_brenoitfy = false;
//		return 0;
//	}
//
//	BOOL StartGameStatusNotify()
//	{
//		m_gamerunning = CreateEvent(NULL, FALSE, FALSE, NULL);
//		m_gameexit = CreateEvent(NULL, FALSE, FALSE, NULL);
//		m_notifythread = std::thread(&CGameStatusMonitor::_Thread_GameStatusNotify,this);
//		return FALSE;
//	}
//
//	VOID StopGameStatusNotify()
//	{
//		m_bkillnotifythread = true;
//		if (m_notifythread.joinable())
//		{
//			SetEvent(m_gamerunning);
//			m_notifythread.join();
//		}
//
//		if (m_gamerunning)
//			CloseHandle(m_gamerunning);
//		if (m_gameexit)
//			CloseHandle(m_gameexit);
//
//	}
//
//private:
//
//	void EventNotify(string eventname, string parameter)
//	{
//		addin_manager_client addinclient;
//		EventReaction eventinfo;
//		eventinfo.EventName = eventname;
//		eventinfo.Parameter = parameter;
//
//		addinclient.fire_custom_event(MakeResult<EventReaction>(eventinfo));
//		addinclient.fire_event(MakeResult<EventReaction>(eventinfo));
//		if (eventname == EVENT_GAMEENTER && parameter != "")
//		{
//			string metricsEventParam = Protocol::Serializer::Serialize<EventReaction>(EventReaction("GameLaunch", parameter));
//			EventReaction * newMetricsEvent = new EventReaction(EVENT_DATACOLLECTION, std::move(metricsEventParam));
//			addinclient.fire_custom_event(Protocol::Serializer::Serialize<EventReaction>(*newMetricsEvent));
//			delete newMetricsEvent;
//		}
//	}
//	
//
//	DWORD _Thread_GameStatusNotify()
//	{
//		ULONG monitorcount = 2;
//		HANDLE hlist[2] = {m_gamerunning, m_gameexit};
//		for (;;)
//		{
//			vector<RunningGameInformation> gamelist;
//			ULONG ret = WaitForMultipleObjects(monitorcount, hlist, FALSE, INFINITE);
//			if (m_bkillnotifythread)
//				break;
//			DBG_PRINTF_FL((L"Notify game status change. event (%d)\n",ret));
//			DWORD retKeyValue = 0;
//			bool bRet = CRegUtil().GetKeyValueDword(
//				HKEY_CURRENT_USER,
//				REG_SETDLG_COMMEN,
//				REG_SETDLG_AUTOGAMEMODE_VALUE,
//				retKeyValue
//			);
//			if (bRet)
//			{
//				if (retKeyValue == 0)
//				{
//					DBG_PRINTF_FL((L"Auto Gamemode disabled!\n"));
//					ULONG status = GAME_EXIT;
//					EventNotify(EVENT_GAMEEXIT, "");
//
//					continue;
//				}
//			}
//			if (ret == WAIT_OBJECT_0)//game running
//			{				
//				m_status = GAME_RUNNING;
//				gamelist.clear();
//				CProcessMonitor::getinstance()->GetRunGameList(gamelist);
//
//				GetRunningGamelistResponse response;
//				response.gamelist = gamelist;
//				EventNotify(EVENT_GAMEENTER, MakeResult<GetRunningGamelistResponse>(response));
//			}
//			else if (ret == (WAIT_OBJECT_0+1))//game exit
//			{
//				m_status = GAME_EXIT;
//				CProcessMonitor::getinstance()->GetRunGameList(gamelist);
//
//				GetRunningGamelistResponse response;
//				response.gamelist = gamelist;
//				EventNotify(EVENT_GAMEEXIT, MakeResult<GetRunningGamelistResponse>(response));
//				m_status = GAME_UNKNOWN;
//
//			}
//		}
//		return 0;
//	}
//
//	HANDLE m_gamerunning;
//	HANDLE m_gameexit;
//	std::thread m_notifythread;
//	bool	m_bkillnotifythread;
//	bool    m_brenoitfy;
//	CriticalSection m_renotifylock;
//
//	UINT m_lastgamestatus;
//	ULONG m_status;
};

