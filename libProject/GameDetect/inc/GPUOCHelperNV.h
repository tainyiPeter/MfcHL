// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <windows.h>
#include "ThirdParty/NVIDIA_API/include/nvapi.h"
#include "DebugUtil.h"
#include "DGpuInfo.h"
#include <thread>
#define BIT(i)  (1 << (i))

typedef struct
{
	NvU32   gpuCount;

} SYSInfoStatic, *PSYSInfoStatic;   // system static info (retrieved only once)

class GpuFrequency
{
public:
	GpuFrequency():_core_frequency(0),
	_vram_frequency(0){

	}
	GpuFrequency(unsigned long core_frequency,
		unsigned long vram_frequency) :
		_core_frequency(core_frequency),
		_vram_frequency(vram_frequency)
	{
	}
	unsigned long _core_frequency;
	unsigned long _vram_frequency;
};
class GpuDriverInitializer {
public:
	
	GpuDriverInitializer():_handle(nullptr) {
		Init();
	}

	virtual ~GpuDriverInitializer() {
		Unload();
	}
	NvPhysicalGpuHandle GetDriverHandle() {
		return _handle;
	}
private:
	void Init() {
		NvPhysicalGpuHandle Gpuhandle[NVAPI_MAX_PHYSICAL_GPUS] = { 0 };
		int gpuId = 0;
		_status = NvAPI_Initialize();
		if (_status == NVAPI_OK)
		{
			NvU32 gpuCount = 0;
			if ((NvAPI_EnumPhysicalGPUs(Gpuhandle, &gpuCount) != NVAPI_OK) || (gpuCount < 1))
			{
				DBG_PRINTF_FL((L"Enumerate NV API failed %x\n", GetLastError()));
			}
			else
			{

				if (gpuCount >= 2) {
					DGpuInfo dGpuInfo;
					for (unsigned long i = 0; i < gpuCount; i++) {
						NvU32 busId = 0;
						if (NvAPI_GPU_GetBusId(Gpuhandle[i], &busId) == NVAPI_OK) {
							if (busId == dGpuInfo.GetBusNumber()) {
								gpuId = i;
								break;
							}
						}
					}
				}
				_handle = Gpuhandle[gpuId];
			}
		}
		else {
			DBG_PRINTF_FL((L"Init NV API failed %x\n", GetLastError()));
		}
	}
	void Unload() {
		if (_status == NVAPI_OK)
			NvAPI_Unload();
		_handle = nullptr;
	}

private:
	NvPhysicalGpuHandle _handle;
	NvAPI_Status _status;
};
class GPUOCHelperNV
{
public:
	virtual ~GPUOCHelperNV() {

	}
	static GPUOCHelperNV* GetInstance()
	{
		static GPUOCHelperNV nv_helper;
		return &nv_helper;
	}

protected:
	GPUOCHelperNV() {}

public:
	virtual BOOL CheckNVInterfaceStatus() {
		return GpuDriverInitializer().GetDriverHandle() != nullptr ? TRUE : FALSE;
	}

	virtual BOOL ModifyClockOffset(std::vector<std::pair<NV_GPU_PUBLIC_CLOCK_ID,ULONG>> & tunelist)
	{
		GpuDriverInitializer gpuInit;
		NvAPI_Status status;
		NV_GPU_PERF_PSTATES20_INFO ps20Info = { 0 };
		std::function<void(bool)> dumpGpuTuneList = [&](bool isSuccess) -> void {
			for (const auto &item : tunelist) {
				DBG_PRINTF_FL((L"\n Delta CLK increase isSuccess %d,NV_GPU_PUBLIC_CLOCK_ID= %d, offset = %d mhz\n \n",
					isSuccess, item.first, item.second / 1000));
			}
		};
		if (gpuInit.GetDriverHandle() == nullptr)
			return FALSE;

		DBG_PRINTF_FL((L"Set gpu oc\n"));
		//Set P0 GRAPHICS , MEMORY delta value
		ps20Info.version = NV_GPU_PERF_PSTATES20_INFO_VER1;
		// Specify only data required for given parameter.
		ps20Info.numPstates = 1;
		ps20Info.numClocks = NvU32(tunelist.size());
		ps20Info.numBaseVoltages = 0;
		ps20Info.pstates[0].pstateId = NVAPI_GPU_PERF_PSTATE_P0;
		int i = 0;
		for (const auto &item : tunelist) {
			ps20Info.pstates[0].clocks[i].domainId = item.first; //for P0 GRAPHICS
			ps20Info.pstates[0].clocks[i].freqDelta_kHz.value = item.second;
			i++;
		}

		status = NvAPI_GPU_SetPstates20(gpuInit.GetDriverHandle(), &ps20Info);
		if (status == NVAPI_OK)
		{
			dumpGpuTuneList(true);
			return TRUE;
		}
		else
		{
			dumpGpuTuneList(false);
			return FALSE;
		}
	}

	 BOOL GetGpuFrequency(GpuFrequency &frequency) {
		NvAPI_Status status;
		NV_GPU_PERF_PSTATES20_INFO ps20Info = { 0 };
		if(IsGpuOff())
		{
			frequency._core_frequency = 0;
			frequency._vram_frequency = 0;
			return TRUE;
		}
		int try_not_powered = 10;

		while (try_not_powered > 0) {
			GpuDriverInitializer gpuInit;
			ps20Info.version = NV_GPU_PERF_PSTATES20_INFO_VER;
			status = NvAPI_GPU_GetPstates20(gpuInit.GetDriverHandle(), &ps20Info);

			if (status == NVAPI_INCOMPATIBLE_STRUCT_VERSION)
			{
				ps20Info.version = NV_GPU_PERF_PSTATES20_INFO_VER1;
				status = NvAPI_GPU_GetPstates20(gpuInit.GetDriverHandle(), &ps20Info);
			}
			if (NVAPI_OK == status)
			{
				frequency = GpuFrequency{ ps20Info.pstates[0].clocks[0].data.range.maxFreq_kHz,
					ps20Info.pstates[0].clocks[1].data.range.maxFreq_kHz };

				DBG_PRINTF_FL((L"P0 GRAPHICS max frequency: %d\n",
					frequency._core_frequency));
				DBG_PRINTF_FL((L"P0 MEMORY max frequency: %d\n",
					frequency._vram_frequency));
				break;
			}
			else
			{
				DBG_PRINTF_FL((L"NvAPI_GPU_GetPstates20 API call failed with error %d\n", status));
			}
			try_not_powered--;
		}

		return (NVAPI_OK == status) ? TRUE : FALSE;
	}

	BOOL GetOverClockInformation(signed int &graphic_occlock, signed int &mem_occlock)
	{
		GpuDriverInitializer gpuInit;
		NvAPI_Status status;
		NV_GPU_PERF_PSTATES20_INFO ps20Info = { 0 };

		if (gpuInit.GetDriverHandle() == nullptr)
			return FALSE;
		//Dump P0 GRAPHICS , MEMORY CLOCK range
		ps20Info.version = NV_GPU_PERF_PSTATES20_INFO_VER;

		status = NvAPI_GPU_GetPstates20(gpuInit.GetDriverHandle(), &ps20Info);

		if (status == NVAPI_INCOMPATIBLE_STRUCT_VERSION)
		{
			ps20Info.version = NV_GPU_PERF_PSTATES20_INFO_VER1;
			status = NvAPI_GPU_GetPstates20(gpuInit.GetDriverHandle(), &ps20Info);
		}
		if (NVAPI_OK == status)
		{
			DBG_PRINTF_FL((L"\nP0 GRAPHICS freqDelta Value range: [%d, %d], GRAPHICS freqDelta Value: [%d]\n",
				ps20Info.pstates[0].clocks[0].freqDelta_kHz.valueRange.min, ps20Info.pstates[0].clocks[0].freqDelta_kHz.valueRange.max,
				ps20Info.pstates[0].clocks[0].freqDelta_kHz.value));
			DBG_PRINTF_FL((L"\nP0 MEMORY   freqDelta Value range: [%d, %d], MEMORY   freqDelta Value: [%d]\n",
				ps20Info.pstates[0].clocks[1].freqDelta_kHz.valueRange.min, ps20Info.pstates[0].clocks[1].freqDelta_kHz.valueRange.max,
				ps20Info.pstates[0].clocks[1].freqDelta_kHz.value));
			graphic_occlock = ps20Info.pstates[0].clocks[0].freqDelta_kHz.value;
			mem_occlock = ps20Info.pstates[0].clocks[1].freqDelta_kHz.value;
			if (ps20Info.pstates[0].clocks[0].bIsEditable)
			{
				DBG_PRINTF_FL((L"GPU support overclock.\n"));
			}
			else
				DBG_PRINTF_FL((L"GPU did NOT support overclock!\n"));
		}
		else
		{
			DBG_PRINTF_FL((L"NvAPI_GPU_GetPstates20 API call failed with error %d\n", status));
		}
		return (NVAPI_OK == status) ? TRUE : FALSE;
	}
	NvAPI_Status getClocks(NV_GPU_CLOCK_FREQUENCIES_CLOCK_TYPE type,ULONG &graphic_clock,ULONG &mem_clock)
	{
		GpuDriverInitializer gpuInit;
		NvAPI_Status                status;
		NV_GPU_CLOCK_FREQUENCIES    clkFreqs = { 0 };

		clkFreqs.version = NV_GPU_CLOCK_FREQUENCIES_VER;
		clkFreqs.ClockType = type;

		if (gpuInit.GetDriverHandle() == nullptr)
			return NVAPI_ERROR;

		status = NvAPI_GPU_GetAllClockFrequencies(gpuInit.GetDriverHandle(), &clkFreqs);
		if (status == NVAPI_OK)
		{
			graphic_clock = clkFreqs.domain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].frequency;
			mem_clock = clkFreqs.domain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY].frequency;
		}
		else
		{
			printf("API call failed with error %d\n", status);
		}
		return status;
	}
	//return true - gpu on, false - gpu off.
	static bool IsGpuOff()
	{
		NvU32 co_proc_status = 0;
		NV_COPROC_STATE co_proc_state = NV_COPROC_STATE_DGPU_GOLD;
		const NvAPI_Status ret_status = NvAPI_Coproc_GetCoprocStatus(&co_proc_status, &co_proc_state);
		if(ret_status == NVAPI_OK)
		{
			DBG_PRINTF_FL((L"NvAPI_Coproc_GetCoprocStatus co proc status %d,state %d",co_proc_status,co_proc_state));
		}
		else
		{
			DBG_PRINTF_FL((L"NvAPI_Coproc_GetCoprocStatus failed! %d",ret_status));
		}

		return ret_status == NVAPI_OK && co_proc_state == NV_COPROC_STATE_DGPU_GOLD;
	}
	BOOL GetCurrentClockInfo(ULONG &graphic_clock, ULONG &mem_clock)
	{
		NvAPI_Status status = NVAPI_OK;

		if(IsGpuOff())
		{
			graphic_clock = 0;
			mem_clock = 0;
			return TRUE;
		}

		int try_not_powered = 10;

		while (try_not_powered > 0) {
			graphic_clock = 0;
			mem_clock = 0;
			GpuDriverInitializer gpuInit;

			if (gpuInit.GetDriverHandle() == nullptr)
				return FALSE;

			status = getClocks(NV_GPU_CLOCK_FREQUENCIES_CURRENT_FREQ, graphic_clock, mem_clock);
			if (status != NVAPI_OK)
			{

				DBG_PRINTF_FL((L"GetCurrentClockInfo API call failed with error %d\n", status));
				getClocks(NV_GPU_CLOCK_FREQUENCIES_BASE_CLOCK, graphic_clock, mem_clock);
			}
			else
			{
				DBG_PRINTF_FL((L" CURRENT Graphics CLOCK: %7d kHz\n", graphic_clock));
				DBG_PRINTF_FL((L" CURRENT Memory   CLOCK: %7d kHz\n", mem_clock));
				break;
			}
			try_not_powered--;
		}

		return (status == NVAPI_OK) ? TRUE : FALSE;
	}

	BOOL GetFullName(wstring &gpuFullName) {
		static string cached_full_name = "";
		string sgpuName = "";
		NvAPI_Status status = NVAPI_OK;
		NvAPI_ShortString gpuName;
		GpuDriverInitializer gpuInit;

		if (gpuInit.GetDriverHandle() == nullptr) {
			DBG_PRINTF_FL((L"No gpu handle\n"));
			return FALSE;
		}
		status = NvAPI_GPU_GetFullName(gpuInit.GetDriverHandle(), gpuName);
		if (status == NVAPI_OK)
		{
			sgpuName = gpuName;
			cached_full_name = gpuName;
			DBG_PRINTF_FL((L"    gpu - Failed to retreive data %d\n,status"));
		}
		else
		{
			sgpuName = cached_full_name;
		}
		
		
		if (sgpuName.substr(0, 6) != "NVIDIA")
			gpuFullName = L"NVIDIA ";

		gpuFullName += wstring(sgpuName.begin(), sgpuName.end());
		return TRUE;
	}
};


