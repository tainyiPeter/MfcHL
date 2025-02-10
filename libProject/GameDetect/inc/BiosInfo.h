// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <comutil.h>
#include <wbemidl.h>
#include <string>
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "comsuppw.lib")

using namespace std;

#include "SMBiosInformationRetriever.h"


class BiosInfo
{
public:

	BiosInfo()
	{
#pragma warning(suppress: 6031)
		CoInitializeEx(0, COINIT_APARTMENTTHREADED);
	}

	BiosInfo(bool bInitCom)
	{
		if (bInitCom)
			CoInitializeEx(0, COINIT_APARTMENTTHREADED);
	}

	~BiosInfo()
	{
		//CoUninitialize();
	}

	BOOL IsNotebook()
	{
		wstring Enclosuretype;
		MachineInformationHelper MaInfoHelper;
		Enclosuretype = MaInfoHelper.EnclosureType();
		if (Enclosuretype == L"Notebook")
			return TRUE;
		return FALSE;
	}

	BOOL IsSupportWarterCooling()
	{
		char coolingtype[256] = { 0 };
		SecureZeroMemory(coolingtype, 256);
		MachineInformationHelper smbiosinfo;
		if (smbiosinfo.GetOEMString(7, coolingtype))
		{
			if (coolingtype[3] == '0')
				return TRUE;
		}
		return FALSE;
	}

	BOOL GetBoxCapacity(int & outValue)
	{
		outValue = 0;
		char boxCapacity[256] = { 0 };
		SecureZeroMemory(boxCapacity, 256);
		MachineInformationHelper smbiosinfo;
		if (smbiosinfo.GetOEMString(8, boxCapacity))
		{
			if (boxCapacity[2] == '0' || boxCapacity[2] == '1')
				outValue = 28;
			else if (boxCapacity[2] == '2')
				outValue = 18;

			return TRUE;
		}
		return FALSE;
	}

	BOOL IsSupportMemoryOverClock()
	{
		char coolingtype[256] = { 0 };
		SecureZeroMemory(coolingtype, 256);
		MachineInformationHelper smbiosinfo;
		if (smbiosinfo.GetOEMString(7, coolingtype))
		{
			if (coolingtype[1] == '1')
				return TRUE;		
		}
		return FALSE;
	}

	int GetBIOSInfomation(bstr_t query, bstr_t name, bstr_t* pValue)
	{
		int rel = 0;//0,succed;1,failed		
		IWbemLocator * g_pLoc = NULL;
		IWbemServices * g_pSvc = NULL;

		try {
			HRESULT hr = CoInitializeSecurity(
				NULL,                       // Security descriptor    
				-1,                            // COM negotiates authentication service
				NULL,                       // Authentication services
				NULL,                       // Reserved
				RPC_C_AUTHN_LEVEL_DEFAULT, // Default authentication level for proxies
				RPC_C_IMP_LEVEL_IMPERSONATE,// Default Impersonation level for proxies
				NULL,                  // Authentication info
				EOAC_NONE,          // Additional capabilities of the client or server
				NULL);              // Reserved

			if (FAILED(hr))
			{
				if (RPC_E_TOO_LATE != hr)
					rel = 1;
				//return rel; ///< not allow return ! else g_pSvc is NULL
			}

			g_pLoc = NULL;
			hr = CoCreateInstance(
				CLSID_WbemLocator,
				0,
				CLSCTX_INPROC_SERVER,
				IID_IWbemLocator,
				(LPVOID *)&g_pLoc
				);

			if (FAILED(hr))
			{
				return 1;
			}

			g_pSvc = NULL;
			hr = g_pLoc->ConnectServer(
				_bstr_t(L"root\\CIMV2"),
				NULL,
				NULL,
				0,
				NULL,
				0,
				0,
				&g_pSvc
				);

			if (FAILED(hr))
			{
				return 1;
			}

			hr = CoSetProxyBlanket(
				g_pSvc,
				RPC_C_AUTHN_WINNT,
				RPC_C_AUTHZ_NONE,
				NULL,
				RPC_C_AUTHN_LEVEL_CALL,
				RPC_C_IMP_LEVEL_IMPERSONATE,
				NULL,
				EOAC_NONE
				);

			if (FAILED(hr))
			{
				return 1;
			}


			IEnumWbemClassObject* pEnumerator = NULL;
			hr = g_pSvc->ExecQuery(
				bstr_t("WQL"),
				query,
				WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
				NULL,
				&pEnumerator
				);

			if (FAILED(hr))
			{
				return 1;
			}

			IWbemClassObject *pclsObj = NULL;
			while (pEnumerator)
			{
				ULONG uReturn = 0;
				hr = pEnumerator->Next(
					WBEM_INFINITE,
					1,
					&pclsObj,
					&uReturn
					);
				
				if (hr == S_OK)
				{
					if (0 == uReturn)
						break;
					VARIANT vtProp;
					VariantInit(&vtProp);
					hr = pclsObj->Get(name, 0, &vtProp, 0, 0);
					if (hr == S_OK)
					{
						*pValue = _bstr_t(vtProp.bstrVal);
						VariantClear(&vtProp);
						break;
					}
					
				}
			}

			if (pclsObj != NULL){
				pclsObj->Release();
			}
			if (pEnumerator != NULL){
				pEnumerator->Release();
			}
		}
		catch (...)
		{
			rel = 1;
		}

		return rel;
	}


	bool GetManufacturer(std::wstring &manufacturername)
	{
		_bstr_t Name;
		if (GetBIOSInfomation(L"SELECT * FROM Win32_ComputerSystemProduct", L"vendor", &Name) == 0)
		{
			manufacturername = Name.GetBSTR();
			return true;
		}
		return false;
	}

	bool GetProductName(std::wstring &productname)
	{
		_bstr_t Name;
		if (GetBIOSInfomation(L"Select * FROM Win32_ComputerSystemProduct", L"Name", &Name)
			== 0)
		{
			CString pname = Name.GetBSTR();
			productname = pname.Left(4);
			return true;
		}
		return false;
	}

	bool GetSerialNumber(std::wstring &serialnumber)
	{
		_bstr_t Name;
		if (GetBIOSInfomation(L"Select * FROM Win32_BIOS", L"SerialNumber", &Name)
			== 0)
		{
			serialnumber = Name.GetBSTR();
			return true;
		}
		return false;
	}

	bool GetOSVerInfo(std::wstring &windowsversion)
	{
		_bstr_t osversion;
		if (GetBIOSInfomation(L"SELECT * FROM Win32_OperatingSystem", L"Caption", &osversion) != 0)
		{
			return false;
		}

		if (GetNum(&osversion) > 0)
		{
			return false;
		}
		windowsversion = osversion.GetBSTR();
		return true;
	}



	//review ok
	int GetNum(bstr_t* pValue)
	{
		int rel = 0;//0,succeed;1,failed

		wchar_t* pw = (LPWSTR)(*pValue);

		int i = 0;

		wchar_t pNum[10];
		int  numIndex = 0;//pNumÖÐµÄÏÂ±ê

		while (pw[i] != '\0')
		{
			if (pw[i] >= '0' && pw[i] <= '9')
			{
				pNum[numIndex] = pw[i];
				numIndex++;
			}
			else if (pw[i] == '.' && numIndex)
			{
				pNum[numIndex] = pw[i];
				numIndex++;
			}
			else
			{
				if (numIndex>0)
				{
					break;
				}
			}
			i++;
		}
		if (numIndex>0)
		{
			pNum[numIndex] = '\0';
			bstr_t newValue(pNum);
			*pValue = newValue;
		}
		else
		{
			rel = 1;
		}
		return rel;

	}

};


