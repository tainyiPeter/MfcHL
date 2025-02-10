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
#pragma once
#include "../define/GZDefine.h"
#pragma comment(lib, "setupapi.lib")

//{1ca05180 - a699 - 450a - 9a0c - de4fbe3ddd89}
DEFINE_GUID(GUID_DEVINTERFACE_DISPLAY, 0x1ca05180, 0xa699, 0x450a, 0x9a, 0x0c, 0xde, 0x4f, 0xbe, 0x3d, 0xdd, 0x89);

#define REG_MAX_TREE_COUNT 512
#define REG_MAX_DATA_COUNT 4096
#define IS_KEY_LEN 1024

class DGpuInfo
{
public:
	DGpuInfo()
		:m_hDevInfo(0L),
		_isFound(false)
	{
		_isNoteBook = false;
		_busNumber = 0;

		m_ClassGuid = (GUID *)&GUID_DEVINTERFACE_DISPLAY;
		m_hDevInfo = SetupDiGetClassDevs(m_ClassGuid, 0, 0, DIGCF_PRESENT | DIGCF_PROFILE | DIGCF_INTERFACEDEVICE);
		if (m_hDevInfo != INVALID_HANDLE_VALUE)
		{
			_isNoteBook = IsNoteBook();
			if (_isNoteBook) {
				_isFound = GetDGPUInfo(_deviceName, _busNumber);
			}
		}
	}
	~DGpuInfo(void)
	{
		if (m_hDevInfo)
		{
			::SetupDiDestroyDeviceInfoList(m_hDevInfo);
			m_hDevInfo = 0L;
		}
	}
	
	int GetBusNumber()
	{
		if (_isFound == false)
			return -1;

		return _busNumber;
	}

	wstring GetDeviceName()
	{
		if (_isFound == false)
			return L"";

		return _deviceName;
	}

private:
	static bool GetKeyValueDword(__in HKEY hRootKey,
		__in LPCTSTR lpRunKey,
		__in LPCTSTR wcKeyName,
		__out DWORD &retValue,
		__in BOOL isKey64 = FALSE)
	{
		HKEY Key;
		DWORD dwDisp = 0;
		WCHAR wcData[REG_MAX_DATA_COUNT] = { 0 };
		HKEY RealRootKey = hRootKey;
		WCHAR keypath[REG_MAX_TREE_COUNT] = { 0 };
		if (wcKeyName == NULL)
			return false;

		if (RegOpenKeyEx(hRootKey,
			lpRunKey,
			0,
			(isKey64 ? KEY_WOW64_64KEY | KEY_QUERY_VALUE : KEY_QUERY_VALUE),
			&Key) != ERROR_SUCCESS)
		{
			return false;
		}

		dwDisp = sizeof(wcData);
		if (RegQueryValueEx(Key,
			wcKeyName,
			NULL,
			NULL,
			(LPBYTE)wcData,
			&dwDisp) != ERROR_SUCCESS)
		{
			RegCloseKey(Key);
			return false;
		}

		RegCloseKey(Key);
		retValue = *(ULONG *)wcData;
		return true;
	}
//	//FW ASL CODE VERSION
//#define GAMEZONE_DATA_ASL_VERSION_REG_ROOT			HKEY_LOCAL_MACHINE
//#define GAMEZONE_DATA_ASL_VERSION_REG_PATH			L"SOFTWARE\\Lenovo\\VantageService\\AddinData\\LenovoGamingAddin\\Data\\FirmWare"
//#define GAMEZONE_DATA_ASL_VERSION_REG_VALUE_NAME	L"AslVersion"
//
//
////FW Product info
//#define GAMEZONE_DATA_PRODUCT_INFO_REG_ROOT			HKEY_LOCAL_MACHINE
//#define GAMEZONE_DATA_PRODUCT_INFO_REG_PATH			L"SOFTWARE\\Lenovo\\VantageService\\AddinData\\LenovoGamingAddin\\Data\\FirmWare"
//#define GAMEZONE_DATA_PRODUCT_INFO_REG_VALUE_NAME	L"ProductInfo"
	bool IsNoteBook() {
		DWORD aslVersion;
		DWORD productInfo;

		if (!GetKeyValueDword(GAMEZONE_DATA_ASL_VERSION_REG_ROOT,
			GAMEZONE_DATA_ASL_VERSION_REG_PATH,
			GAMEZONE_DATA_ASL_VERSION_REG_VALUE_NAME, aslVersion))
			return false;

		if (!GetKeyValueDword(GAMEZONE_DATA_PRODUCT_INFO_REG_ROOT,
			GAMEZONE_DATA_PRODUCT_INFO_REG_PATH,
			GAMEZONE_DATA_PRODUCT_INFO_REG_VALUE_NAME, productInfo))
			return false;

		if (productInfo >= 100 && aslVersion >= 11)
			return true;
		else 
			return false;

	}
	bool GetRegistryProperty(SP_DEVINFO_DATA &spDevInfoData, DWORD property, vector<BYTE> &buffer, DWORD &needSize) {
		DWORD dwRetVal = 0;
		DWORD outProperty;
		dwRetVal = SetupDiGetDeviceRegistryProperty(m_hDevInfo,
			&spDevInfoData,
			property,
			&outProperty,
			(PBYTE)buffer.data(),
			(DWORD)buffer.capacity(),
			&needSize);
		if (!dwRetVal) {
			dwRetVal = GetLastError();
			if (dwRetVal == ERROR_INSUFFICIENT_BUFFER) {
				buffer.resize(needSize);
				if (SetupDiGetDeviceRegistryProperty(m_hDevInfo,
					&spDevInfoData,
					property,
					&outProperty,
					(PBYTE)buffer.data(),
					(DWORD)buffer.capacity(),
					&needSize))
					return true;
			}
		}

		return false;
	}
	bool GetDGPUInfo(wstring &strdeviceName, int &busNumber)
	{
		const wstring DGPULocationPath = L"ACPI(_SB_)#ACPI(PCI0)#ACPI(PEG0)#ACPI(PEGP)";
		const wstring DGPULocationPath_X60 = L"ACPI(_SB_)#ACPI(PCI0)#ACPI(GPP0)#ACPI(PEGP)";
		SP_DEVINFO_DATA spDevInfoData = { 0L };
		short           wIdx = 0;
		bool            bIsFound = false;;

		spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		wIdx = 0;
		while (++wIdx)
		{
			if (SetupDiEnumDeviceInfo(m_hDevInfo, wIdx, &spDevInfoData))
			{
				vector<BYTE> buffer;
				DWORD needSize = 0;
				if (GetRegistryProperty(spDevInfoData, SPDRP_LOCATION_PATHS, buffer, needSize))
				{
					for (DWORD i = 0; i < needSize; )
					{
						DWORD error = 0;
						wstring deviceLocationPath = (WCHAR *)(buffer.data() + i);
						if (deviceLocationPath == DGPULocationPath || deviceLocationPath == DGPULocationPath_X60) {
							vector<BYTE> deviceName;
							vector<BYTE> busNumberbuffer;
							DWORD bufferSize;
							if (GetRegistryProperty(spDevInfoData, SPDRP_DEVICEDESC, deviceName, bufferSize)) {
								strdeviceName = (WCHAR *)deviceName.data();
							}
							else continue;

							if (GetRegistryProperty(spDevInfoData, SPDRP_BUSNUMBER, busNumberbuffer, bufferSize)) {
								busNumber = *(int *)busNumberbuffer.data();
							}
							else continue;

							bIsFound = true;
							break;
						}
						i += (DWORD)deviceLocationPath.length() * 2 + 2;
					}
				}

			};

			if (bIsFound)
				break;
		};
		return bIsFound;
	};
	GUID			*m_ClassGuid;
	HDEVINFO		m_hDevInfo;
	bool			_isNoteBook;
	bool			_isFound;
	wstring			_deviceName;
	int				_busNumber;
};


