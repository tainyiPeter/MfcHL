#pragma once

#include "inc/MacroDef.h"
#include "inc/UserPrivilegeUtil.h"
#include "inc/LockObject.h"
#include "inc/EventTimer.h"
#include <initguid.h>  

#include "inc/DevInfo.h"
#include "inc/GetDisplayinfofromDx.h"
#include <map>
#include <utility>
#include "inc/gpuusagehelper.h"


#define _WIN32_DCOM


enum DISPLAY_CARD_TYPE
{
	NVIDIA,
	AMD,
	INTEL,
	UnKnowType
};

enum DISPLAY_CARD_INTERFACE
{
	UnKnow,
	Integrated,
	Discrete
};


class GPUManager
{
private:
	GPUManager() : m_notifyhandle(nullptr),
	m_EventTimer(CallBack),
	m_gameMode(false),
	check_proc_list_(nullptr)
	{
		InitGPUSInfo();
	};

	//next callback will wait for the current callback is finished
	static VOID CALLBACK CallBack(PVOID pData, BOOLEAN TimerOrWaitFired)
	{
		UNREFERENCED_PARAMETER(TimerOrWaitFired);
		if (pData == NULL)
			return;

		const auto pManage = static_cast<GPUManager*>(pData);
		if (!pManage->m_gameMode)
		{
			int utilization = 0;
			static size_t index = 0;
			vector<ULONG> check_list = pManage->check_proc_list_->Getpidlist();
			for (auto &item : pManage->m_GPUs)
			{
				if (index >= item.second->game_engines.size())
					index = 0;

				if (item.second->game_engines.size() > 0)
				{
					int current_gpu = (int)item.second->gpuUsage.UpdateGPUUtilization(item.second->game_engines[index], check_list);
					if (current_gpu > utilization)
						utilization = current_gpu;
					DBG_PRINTF_FL((L"GPUManager::CallBack utilization %d current_gpu %d index %d\n", utilization, current_gpu, index));
				}
			}

			if (utilization < GZ_GAME_DGPU_LOAD_MIN)
			    index++;
		}	

		if (pManage->m_notifyhandle != NULL)
			SetEvent(pManage->m_notifyhandle);
	}

public:
	~GPUManager() {
		if (m_notifyhandle != NULL)
			CloseHandle(m_notifyhandle);
		m_GPUs.clear();
	};
	static GPUManager* getInstance(bool destroy = false);

	struct GPUInfo
	{
		~GPUInfo()
		{
		}
		wstring displayName;
		wstring adpterName;
		DISPLAY_CARD_INTERFACE  displayType = UnKnow;
		CGpuHelper gpuUsage;
		ULONG intervaltime = CHECK_GPU_LOAD_TIME_S;
		HANDLE hEvent = NULL;
		wstring highPartOfGUID;
		wstring lowPartOfGUID;
		vector<wstring> game_engines;
	};

	void Initialize(HANDLE & handle, ThreadSafeMap<UINT, ULONG> *pid_list)
	{
		if (m_GPUs.size() <= 0)
			return;

		if (m_notifyhandle == NULL)
			m_notifyhandle = CreateEvent(NULL, FALSE, FALSE, NULL);
		handle = m_notifyhandle;

		check_proc_list_ = pid_list;
	}

	void StartMonitor(bool gamemode)
	{
		m_gameMode = gamemode;
		m_EventTimer.Start(300, CHECK_GPU_LOAD_TIME_S, this);
	}

	void EndMonitor()
	{
		m_EventTimer.End();
		for (auto &item : m_GPUs)
		{
			item.second->gpuUsage.ClearUtilization();
		}
	}

	void ChangeCheckTime(ULONG time)
	{
		if (m_GPUs.size() > 0)
			m_EventTimer.ChangeTimer(time);
	}

	bool CheckGPULoading()
	{
		for (auto &item : m_GPUs)
		{
			ULONG load = (ULONG)item.second->gpuUsage.GetGpuUtilization();
			DBG_PRINTF_FL((L"start CheckGPULoading %s (%d)\n", item.second->displayName.c_str(), load));

			if (load < GZ_GAME_DGPU_LOAD_MIN)
			{
				continue;
			}
			else
			{
				ChangeCheckTime(CHECK_GPU_LOAD_TIME_S1);
			}

			bool bGpuLoad = true;
			for (int i = 0; i < 10; i++)
			{
				load = (ULONG)item.second->gpuUsage.GetGpuUtilization();
				DBG_PRINTF_FL((L"current gpu-detail (%d)\n", load));
				if (load < GZ_GAME_DGPU_LOAD_MIN)
				{
					bGpuLoad = false;
				}
				if (!bGpuLoad)
					break;
				Sleep(500);
			}
			if (bGpuLoad)
				return true;
		}
		return false;
	}

	size_t GetGPUCount()
	{
		return m_GPUs.size();
	}

	template<typename Fn>
	bool ForEach(Fn f)
	{
		for (auto &item : m_GPUs)
		{
			if (f(item.first, item.second))
				return true;
		}
		return false;
	}


	void InitGPUSInfo()
	{
		m_GPUs.clear();

		unordered_multimap<wstring, wstring> displayCards;
		CDevInfo dev((GUID*)&GUID_DEVINTERFACE_DISPLAY_CDevInfo);
		while (dev.EnumDeviceInfo())
		{
			WCHAR driverDesc[MAX_PATH] = { 0 };
			dev.GetDeviceRegistryProperty(SPDRP_DEVICEDESC, (PBYTE)driverDesc);
			wstring wsdevicePath = TransformCase(dev.GetDeviceInterfacePath());
			displayCards.insert(make_pair(driverDesc, wsdevicePath));
			DBG_PRINTF_FL((L"GpuInfoHelper::InitGPUSInfo DeviceInterfacePath=%s\n", wsdevicePath.c_str()));
		}

		ForEachGraphicCard([&](const wchar_t * displayName, const wchar_t * cardInfo, const wchar_t * LowPart, const wchar_t * HighPart) 
		{
			shared_ptr<GPUInfo> gpu(new GPUInfo());
			gpu->displayName = TransformCase(displayName);		

			if (CGpuHelper::CheckDiscrete(LowPart, HighPart))
				gpu->displayType = DISPLAY_CARD_INTERFACE::Discrete;
			else
				gpu->displayType = DISPLAY_CARD_INTERFACE::Integrated;

			if (gpu->displayType == DISPLAY_CARD_INTERFACE::Discrete)
			{
				gpu->lowPartOfGUID = LowPart;
				gpu->highPartOfGUID = HighPart;
				
				for (auto it = displayCards.begin(); it != displayCards.end(); ++it) {
					if (it->second.find(cardInfo) != it->second.npos)
					{
						gpu->adpterName = it->second;
						//for the same displaycard
						displayCards.erase(it);
						DBG_PRINTF_FL((L"GpuInfoHelper::InitGPUSInfo find card info success"));
						break;
					}
				}

				std::set<wstring> engines;
				CPdhMoniter::GetAllEngines(gpu->highPartOfGUID.c_str(), gpu->lowPartOfGUID.c_str(), engines);
				for (auto item : engines)
				{
					DBG_PRINTF_FL((L"GPUManager::InitGPUSInfo engine:%s", item.c_str()));

					if (item.find(L"_3D") != item.npos ||
						item.find(L"_Graphics") != item.npos)
					{
						gpu->game_engines.push_back(item);
					}
				}
				m_GPUs.insert(make_pair(gpu->displayName, gpu));
			}

			DBG_PRINTF_FL((L"GpuInfoHelper::InitGPUSInfo displayName=%s cardInfo=%s LowPart=%s HighPart=%s gpu->displayType=%d\n",
				gpu->displayName.c_str(), cardInfo, LowPart, HighPart, gpu->displayType));

			return false;
		});
	}

	bool IsProcessUseGpuMemory(ULONG pid, long &outGpumemory, long &outSharememory)
	{
		outGpumemory = 0;
		outSharememory = 0;
		long gpuMem = 0;
		long shareMem = 0;
		GPUManager::getInstance()->ForEach([&](wstring displayName, shared_ptr<GPUManager::GPUInfo> & gpuinfo) {

			gpuinfo->gpuUsage.GetDisplayCardMem(pid, gpuinfo->highPartOfGUID.c_str(), gpuinfo->lowPartOfGUID.c_str(), gpuMem, shareMem);
			//choose the display card that has the highest loading
			if (gpuMem > outGpumemory)
			{
				outGpumemory = gpuMem;
				outSharememory = shareMem;
			}
			DBG_PRINTF_FL((L"(%s) Process gpu memory used %d MB, share memory %d MB\n",
				gpuinfo->displayName.c_str(), (int)gpuMem, (int)shareMem));
			return false;
		});
		return ((outSharememory == 0 && outGpumemory == 0) ? false : true);
	}

	int GetProcessGpuload(ULONG pid)
	{
		//find the max load in all the GPUs
		DWORD maxUtilization = 0;

		GPUManager::getInstance()->ForEach([&](wstring dispalyName, shared_ptr<GPUManager::GPUInfo> & gpuinfo) {
			DWORD utilization = 0;
			gpuinfo->gpuUsage.GetProcessUtilization(pid, gpuinfo->highPartOfGUID.c_str(), gpuinfo->lowPartOfGUID.c_str(), utilization);
			if (maxUtilization < utilization)
				maxUtilization = utilization;

			DBG_PRINTF_FL((L"GetProcessGpuload (%s) check pid(%d), utilization %d\n",
				gpuinfo->displayName.c_str(), pid, utilization));
			return false;
		});

		return (int)maxUtilization;
	}

private:
	multimap<wstring, shared_ptr<GPUInfo>> m_GPUs;
	HANDLE m_notifyhandle;
	EventTimer m_EventTimer;	
	bool m_gameMode;

	ThreadSafeMap<UINT, ULONG> *check_proc_list_;
};

inline GPUManager* GPUManager::getInstance(bool destroy)
{
	static unique_ptr<GPUManager> myInstance;
	if (destroy)
	{
		myInstance = nullptr;
		return nullptr;
	}

	if (myInstance == nullptr)
		myInstance.reset(new GPUManager);
	return myInstance.get();
}

