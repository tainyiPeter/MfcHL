// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include "windows.h"
#include <string>
#include <vector>
using namespace std;
#include <setupapi.h>
#include <cfgmgr32.h>
#include <initguid.h>  

#pragma comment(lib, "setupapi.lib")

//{1ca05180 - a699 - 450a - 9a0c - de4fbe3ddd89}
DEFINE_GUID(GUID_DEVINTERFACE_DISPLAY_CDevInfo, 0x1ca05180, 0xa699, 0x450a, 0x9a, 0x0c, 0xde, 0x4f, 0xbe, 0x3d, 0xdd, 0x89);

DEFINE_GUID(GUID_DEVINTERFACE_DISK, 0x53f56307L, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b);

//{378DE44C - 56EF - 11D1 - BC8C - 00A0C91405DD}
DEFINE_GUID(GUID_DEVINTERFACE_MOUSE, 0x378DE44C, 0x56EF, 0x11D1, 0xBC, 0x8C, 0x00, 0xa0, 0xc9, 0x14, 0x05, 0xDD);



class CDevInfo
{
public:
	CDevInfo(CONST GUID *ClassGuid = 0, PCWSTR Enumerator = 0, DWORD Flags = DIGCF_PRESENT | DIGCF_PROFILE | DIGCF_INTERFACEDEVICE/*DIGCF_ALLCLASSES | DIGCF_PROFILE*/)
		:
		m_hDevInfo(0L),
		m_bDevInfo(FALSE),
		m_MemberIndex(-1)
	{
		SecureZeroMemory(&m_spDevInfoData, sizeof(SP_DEVINFO_DATA));
		if (ClassGuid == 0)
		{
			Flags = Flags | DIGCF_ALLCLASSES;
		}
		m_ClassGuid = (GUID *)ClassGuid;
		m_hDevInfo = SetupDiGetClassDevs(ClassGuid, Enumerator, 0, Flags);
		if (m_hDevInfo == INVALID_HANDLE_VALUE)
		{
			;

		}
	}
	~CDevInfo(void)
	{
		if (m_hDevInfo)
		{
			::SetupDiDestroyDeviceInfoList(m_hDevInfo);
			m_hDevInfo = 0L;
		}
	}

	BOOL EnumDeviceInfo(void)
	{
		m_MemberIndex++;
		m_spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		m_bDevInfo = ::SetupDiEnumDeviceInfo(m_hDevInfo, m_MemberIndex, &m_spDevInfoData);
		if (!m_bDevInfo)
		{

		}
		return m_bDevInfo;
	}

	bool GetDeviceState(bool & bIsopen)
	{
		ULONG dwDevStatus;
		ULONG dwProblem;
		if (CM_Get_DevNode_Status(&dwDevStatus, &dwProblem, m_spDevInfoData.DevInst, 0) != CR_SUCCESS)
		{
			DBG_PRINTF_FL((L"CDevInfo::GetDeviceState CM_Get_DevNode_Status return error=%d\n", GetLastError()));
			return false;
		}

		DBG_PRINTF_FL((L"CDevInfo::GetDeviceState CM_Get_DevNode_Status dwDevStatus=%x\n", dwDevStatus));
		if (DN_HAS_PROBLEM & dwDevStatus)
		{
			bIsopen = false;
		}
		else
		{
			bIsopen = true;
		}

		return true;
	}

	BOOL GetDeviceRegistryProperty(DWORD Property, PBYTE PropertyBuffer)
	{
		BOOL bGotRegProp = ::SetupDiGetDeviceRegistryProperty(m_hDevInfo, &m_spDevInfoData,
			Property,
			0L,
			PropertyBuffer,
			2048,
			0);



		if (!bGotRegProp)
		{

		}
		return bGotRegProp;
	}

	BOOL GetClassDescription(PWSTR ClassDescription)
	{
		BOOL bRet = ::SetupDiGetClassDescription(&m_spDevInfoData.ClassGuid, ClassDescription, MAX_PATH, NULL);
		if (!bRet)
		{

		}
		return bRet;
	}

	BOOL EnableDevice()
	{
		return SetDeviceState(DICS_ENABLE);
	}

	BOOL DisableDevice()
	{
		return SetDeviceState(DICS_DISABLE);
	}

	BOOL RemoveDevice()
	{
		return SetupDiRemoveDevice(m_hDevInfo, &m_spDevInfoData);
	}

	wstring GetDeviceInterfacePath()
	{
		SP_DEVICE_INTERFACE_DATA devInfoData;
		SecureZeroMemory(&devInfoData, sizeof(SP_DEVICE_INTERFACE_DATA));
		devInfoData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		int deviceNo = 0;
		DWORD  DRET = 0;
		BOOL result = FALSE;
		SetLastError(NO_ERROR);

		BOOL Result = SetupDiEnumInterfaceDevice(m_hDevInfo, &m_spDevInfoData, m_ClassGuid, deviceNo, &devInfoData);
		if (Result)
		{
			DWORD strSize = 0, requiredLength = 0;
			DRET = SetupDiGetInterfaceDeviceDetail(m_hDevInfo, &devInfoData, NULL, 0, &strSize, NULL);
			if (DRET == 0)
			{
				DRET = GetLastError();
			}
#pragma warning(suppress: 6102)
			requiredLength = strSize;
			PSP_INTERFACE_DEVICE_DETAIL_DATA devDetail = (SP_INTERFACE_DEVICE_DETAIL_DATA*)malloc(requiredLength);
			if (NULL == devDetail)
				return FALSE;
			devDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
			SP_DEVINFO_DATA did;
			did.cbSize = sizeof(SP_DEVINFO_DATA);
			result = SetupDiGetDeviceInterfaceDetail(m_hDevInfo, &devInfoData, devDetail, strSize, &requiredLength, &did);

			if (result)
				return devDetail->DevicePath;

			free(devDetail);
		}
		return L"";
	}

	BOOL GetDeviceInterfacePath(vector<wstring> &interfacepathlist)
	{
		SP_DEVICE_INTERFACE_DATA devInfoData;
		SecureZeroMemory(&devInfoData, sizeof(SP_DEVICE_INTERFACE_DATA));
		devInfoData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		int deviceNo = 0;
		DWORD  DRET = 0;
		BOOL result = FALSE;
		SetLastError(NO_ERROR);
		while (GetLastError() != ERROR_NO_MORE_ITEMS)
		{
			BOOL Result = SetupDiEnumInterfaceDevice(m_hDevInfo, 0, m_ClassGuid, deviceNo, &devInfoData);
			if (Result)
			{
				DWORD strSize = 0, requiredLength = 0;
				DRET = SetupDiGetInterfaceDeviceDetail(m_hDevInfo, &devInfoData, NULL, 0, &strSize, NULL);
				if (DRET == 0)
				{
					DRET = GetLastError();
				}
#pragma warning(suppress: 6102)
				requiredLength = strSize;
				PSP_INTERFACE_DEVICE_DETAIL_DATA devDetail = (SP_INTERFACE_DEVICE_DETAIL_DATA*)malloc(requiredLength);
				if (NULL == devDetail)
					return FALSE;
				devDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
				SP_DEVINFO_DATA did;
				did.cbSize = sizeof(SP_DEVINFO_DATA);
				result = SetupDiGetDeviceInterfaceDetail(m_hDevInfo, &devInfoData, devDetail, strSize, &requiredLength, &did);

				if (result)
				{
					interfacepathlist.push_back(devDetail->DevicePath);
				}
				free(devDetail);
			}
			++deviceNo;

		}
		return DRET;
	}
private:
	BOOL SetDeviceState(DWORD dwState)
	{
		SP_PROPCHANGE_PARAMS propChange = { sizeof(SP_CLASSINSTALL_HEADER) };
		propChange.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
		propChange.Scope = DICS_FLAG_GLOBAL;
		propChange.StateChange = dwState;
		BOOL rlt;

		rlt = SetupDiSetClassInstallParams
		(
			m_hDevInfo,
			&m_spDevInfoData,
			(SP_CLASSINSTALL_HEADER *)&propChange,
			sizeof(propChange)
		);
		if (!rlt)
		{

			return FALSE;
		}
		rlt = SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, m_hDevInfo, &m_spDevInfoData);
		if (!rlt)
		{

		}
		return rlt;
	}
	GUID			*m_ClassGuid;
	HDEVINFO		m_hDevInfo;
	SP_DEVINFO_DATA m_spDevInfoData;
	BOOL			m_bDevInfo;
	short           m_MemberIndex;
};


