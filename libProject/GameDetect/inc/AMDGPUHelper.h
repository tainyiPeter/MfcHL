// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "DebugUtil.h"
#include "../ThirdParty/ADL_SDK/include/adl_sdk.h"

class AMDI2CInterface
{
public:
	enum CmdAction
	{
		read = 1,
		write = 2
	};

	virtual bool WriteI2C(BYTE register_addr, BYTE data, CmdAction action = CmdAction::write, int address = 0x93) = 0;
	virtual LPAdapterInfo GetHandle() = 0;
	virtual ~AMDI2CInterface() {};
};

static void* __stdcall ADL_Main_Memory_Alloc(int iSize)
{
	void* lpBuffer = malloc(iSize);
	return lpBuffer;
}

static void __stdcall ADL_Main_Memory_Free(void** lpBuffer)
{
	if (NULL != *lpBuffer)
	{
		free(*lpBuffer);
		*lpBuffer = NULL;
	}
}

class AMDGPUHelper : public AMDI2CInterface
{
public:
	typedef int(*ADL_MAIN_CONTROL_CREATE)(ADL_MAIN_MALLOC_CALLBACK, int);
	typedef int(*ADL_MAIN_CONTROL_DESTROY)();
	typedef int(*ADL_ADAPTER_NUMBEROFADAPTERS_GET) (int*);
	typedef int(*ADL_ADAPTER_ADAPTERINFO_GET) (LPAdapterInfo, int);
	typedef int(*ADL_DISPLAY_COLORCAPS_GET) (int, int, int *, int *);
	typedef int(*ADL_DISPLAY_COLOR_GET) (int, int, int, int *, int *, int *, int *, int *);
	typedef int(*ADL_DISPLAY_COLOR_SET) (int, int, int, int);
	typedef int(*ADL_DISPLAY_DISPLAYINFO_GET) (int, int *, ADLDisplayInfo **, int);

	typedef int(*ADL_DISPLAY_WRITEANDREADI2C) (int iAdapterIndex, ADLI2C *plI2C);

	AMDGPUHelper()
	{
		int  iNumberAdapters = 0;		
		ADL_MAIN_CONTROL_CREATE          ADL_Main_Control_Create = NULL;		
		ADL_ADAPTER_NUMBEROFADAPTERS_GET ADL_Adapter_NumberOfAdapters_Get = NULL;
		ADL_ADAPTER_ADAPTERINFO_GET      ADL_Adapter_AdapterInfo_Get = NULL;
		ADL_DISPLAY_DISPLAYINFO_GET      ADL_Display_DisplayInfo_Get = NULL;	

		m_hDLL = LoadLibraryEx(L"atiadlxx.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
		if (NULL == m_hDLL)
		{
			DBG_PRINTF_FL((L"AMDLedAPI LoadLibrary failed\n"));
			return;
		}

		ADL_Main_Control_Create = (ADL_MAIN_CONTROL_CREATE)GetProcAddress(m_hDLL, "ADL_Main_Control_Create");
		m_ADL_Main_Control_Destroy = (ADL_MAIN_CONTROL_DESTROY)GetProcAddress(m_hDLL, "ADL_Main_Control_Destroy");
		ADL_Adapter_NumberOfAdapters_Get = (ADL_ADAPTER_NUMBEROFADAPTERS_GET)GetProcAddress(m_hDLL, "ADL_Adapter_NumberOfAdapters_Get");
		ADL_Adapter_AdapterInfo_Get = (ADL_ADAPTER_ADAPTERINFO_GET)GetProcAddress(m_hDLL, "ADL_Adapter_AdapterInfo_Get");
		ADL_Display_DisplayInfo_Get = (ADL_DISPLAY_DISPLAYINFO_GET)GetProcAddress(m_hDLL, "ADL_Display_DisplayInfo_Get");
		m_ADL_Display_WriteAndReadI2C = (ADL_DISPLAY_WRITEANDREADI2C)GetProcAddress(m_hDLL, "ADL_Display_WriteAndReadI2C");
		if (NULL == ADL_Main_Control_Create ||
			NULL == m_ADL_Main_Control_Destroy ||
			NULL == ADL_Adapter_NumberOfAdapters_Get ||
			NULL == ADL_Adapter_AdapterInfo_Get ||
			NULL == ADL_Display_DisplayInfo_Get ||
			NULL == m_ADL_Display_WriteAndReadI2C
			)
		{
			DBG_PRINTF_FL((L"AMDLedAPI GetProcAddress failed\n"));
			return;
		}

		if (ADL_OK != ADL_Main_Control_Create(ADL_Main_Memory_Alloc, 1))
		{
			DBG_PRINTF_FL((L"AMDLedAPI ADL_Main_Control_Create failed\n"));
			return ;
		}

		if (ADL_OK != ADL_Adapter_NumberOfAdapters_Get(&iNumberAdapters))
		{
			DBG_PRINTF_FL((L"AMDLedAPI ADL_Adapter_NumberOfAdapters_Get failed\n"));
			return;
		}

		DBG_PRINTF_FL((L"AMDLedAPI iNumberAdapters=%d\n", iNumberAdapters));

		if (0 < iNumberAdapters)
		{
			m_lpAdapterInfo = (LPAdapterInfo)malloc(sizeof(AdapterInfo) * iNumberAdapters);
			SecureZeroMemory(m_lpAdapterInfo, sizeof(AdapterInfo) * iNumberAdapters);
			ADL_Adapter_AdapterInfo_Get(m_lpAdapterInfo, sizeof(AdapterInfo) * iNumberAdapters);
		}
	}

	~AMDGPUHelper()
	{
		if (m_lpAdapterInfo!= NULL)
		    ADL_Main_Memory_Free((void **)&m_lpAdapterInfo);
		if (m_ADL_Main_Control_Destroy != NULL)
		    m_ADL_Main_Control_Destroy();
		if (m_hDLL != NULL)
		    FreeLibrary(m_hDLL);
	}

	LPAdapterInfo GetHandle() override
	{
		return m_lpAdapterInfo;
	}

	bool WriteI2C(BYTE register_addr, BYTE data, CmdAction action = CmdAction::write, int address = 0x93) override
	{
		if (m_lpAdapterInfo == NULL || m_ADL_Display_WriteAndReadI2C == NULL)
			return false;

		try
		{
			ADLI2C adli2c;
			SecureZeroMemory(&adli2c, sizeof(ADLI2C));
			adli2c.iAction = action;
			adli2c.iSize = sizeof(ADLI2C);
			adli2c.iAddress = address; // user input string to hex
			adli2c.iLine = 3;
			adli2c.iOffset = (int)register_addr; // user input string to hex
			adli2c.iDataSize = 1;
			adli2c.iSpeed = 100;
			char Tem;
			Tem = (char)data;
			adli2c.pcData = &Tem;      

			int ret = m_ADL_Display_WriteAndReadI2C( m_lpAdapterInfo[0].iAdapterIndex, &adli2c);
			DBG_PRINTF_FL((L"AMDGPUHelper WriteI2C return with status %d key=%x value=%x\n", ret, register_addr, data));
			if (ret == ADL_OK)
				return true;
		}
		catch (...)
		{
			DBG_PRINTF_FL((L"NVLedAPI WriteI2C throw exception\n"));
		}		

		return false;
	}

private:

	HINSTANCE m_hDLL = NULL;
	LPAdapterInfo m_lpAdapterInfo = NULL;
	ADL_MAIN_CONTROL_DESTROY  m_ADL_Main_Control_Destroy = NULL;
	ADL_DISPLAY_WRITEANDREADI2C		 m_ADL_Display_WriteAndReadI2C = NULL;
};

