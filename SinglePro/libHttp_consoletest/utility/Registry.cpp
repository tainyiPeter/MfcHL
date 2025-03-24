//#include "stdafx.h"

#include "Registry.h"
//#include <atlbase.h>
#include "Tool.h"
#include <winioctl.h>

#define LS_MAX_PATH 1024*10

#pragma region RegHelper

CRegHelper::CRegHelper(void)
{
}

CRegHelper::~CRegHelper(void)
{
}

BOOL CRegHelper::SetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, CString szRegValue, DWORD dwDataType, BOOL bUserRedirect)
{
	DWORD mdwDataType;
	DWORD mdwSize;
	HKEY  mhCtrlNwKey;
	bool bStaus = FALSE;
	mdwDataType = dwDataType;
	mdwSize = (DWORD)szRegValue.GetLength();
	DWORD dwOption = KEY_ALL_ACCESS;
	if (bUserRedirect)
		dwOption |= KEY_WOW64_64KEY;

	if (RegCreateKeyEx(hKey, szRegKeyPath, 0, NULL, REG_OPTION_NON_VOLATILE, dwOption, NULL, &mhCtrlNwKey, NULL) == ERROR_SUCCESS)
	{
		if (RegSetValueEx(mhCtrlNwKey, szRegKeyName, NULL, mdwDataType, (LPBYTE)szRegValue.GetBuffer(), mdwSize*sizeof(WCHAR)) == ERROR_SUCCESS)
		{
			bStaus = TRUE;
			RegFlushKey(mhCtrlNwKey);
		}

		RegCloseKey(mhCtrlNwKey);
	}//end of if RegOpenKeyEx
	return bStaus;
}

BOOL CRegHelper::SetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, BOOL bRegValue)
{
	DWORD dwRegValue = bRegValue;
	return SetRegistryValue( hKey,  szRegKeyPath,  szRegKeyName,  dwRegValue);
}

BOOL CRegHelper::SetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, DWORD dwRegValue, BOOL bUseWOW )
{
	DWORD mdwDataType;
	HKEY  mhCtrlNwKey;
	bool bStaus = FALSE;
	mdwDataType = REG_DWORD;

	DWORD dwOption = KEY_ALL_ACCESS;
	if (bUseWOW)
		dwOption |= KEY_WOW64_64KEY;

	if (RegCreateKeyEx(hKey, szRegKeyPath, 0, NULL, REG_OPTION_NON_VOLATILE, dwOption, NULL, &mhCtrlNwKey, NULL) == ERROR_SUCCESS)
	{
		if (RegSetValueEx(mhCtrlNwKey, szRegKeyName, NULL, mdwDataType, (PUCHAR)&dwRegValue, sizeof(dwRegValue)) == ERROR_SUCCESS)
		{
			bStaus = TRUE;
			RegFlushKey(mhCtrlNwKey);
		}
		RegCloseKey(mhCtrlNwKey);
	}//end of if RegOpenKeyEx
	return bStaus;
}

//@002A start
//This function should be used only when caller is impersonated.
BOOL CRegHelper::GetCurrentUserRegistryValue(const CString& szRegKeyPath, const CString& szRegKeyName, CString& szRegValue)
{
	DWORD mdwDataType;
	DWORD mdwSize;
	HKEY mhCurUser;
	BOOL bStaus = FALSE;
	mdwDataType = REG_SZ;
	mdwSize = LS_MAX_PATH*sizeof(TCHAR);
	try
	{
		CString strMsg;
		TCHAR  *szValue = new TCHAR[LS_MAX_PATH];
		if (szValue)
		{
			memset(szValue, 0, LS_MAX_PATH*sizeof(TCHAR));

			if (RegOpenCurrentUser(KEY_READ | KEY_QUERY_VALUE | KEY_WOW64_64KEY, &mhCurUser) == ERROR_SUCCESS)
			{
				if (RegGetValue(mhCurUser, szRegKeyPath, szRegKeyName, RRF_RT_REG_SZ, &mdwDataType, (LPBYTE)szValue, &mdwSize) == ERROR_SUCCESS)
				{
					szRegValue = szValue;
					bStaus = TRUE;
				}

				RegCloseKey(mhCurUser);
			}//end of if RegOpenKeyEx
			//DWORD dwError = GetLastError();
			delete[] szValue;
		}
	}
	catch (std::bad_alloc)
	{

	}
	return bStaus;
}
//@002A end

//@002A start
//This function should be used only when caller is impersonated.
BOOL CRegHelper::GetCurrentUserRegistryValue(const CString& szRegKeyPath, const CString& szRegKeyName, DWORD& dwRegValue)
{
	DWORD mdwDataType;
	DWORD mdwSize = sizeof(DWORD);
	HKEY mhCurUser;
	BOOL bStaus = FALSE;
	mdwDataType = REG_DWORD;
	CString strMsg;

	LONG hRet = RegOpenCurrentUser(KEY_READ | KEY_QUERY_VALUE | KEY_WOW64_64KEY, &mhCurUser);

	if (hRet == ERROR_SUCCESS)
	{
		if (RegGetValue(mhCurUser, szRegKeyPath, szRegKeyName, RRF_RT_DWORD, &mdwDataType, (LPBYTE)&dwRegValue, &mdwSize) == ERROR_SUCCESS)
		{
			bStaus = TRUE;
		}
		RegCloseKey(mhCurUser);
	}//end of if RegOpenKeyEx

	return bStaus;
}

BOOL CRegHelper::GetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, DWORD& dwRegValue,BOOL bUseWOW)
{
	DWORD mdwDataType;
	DWORD mdwSize = sizeof(DWORD);
	HKEY  mhCtrlNwKey;
	bool bStaus = FALSE;
	mdwDataType = REG_DWORD;

	DWORD dwOption = KEY_READ | KEY_QUERY_VALUE;
	if (bUseWOW)
		dwOption = KEY_READ | KEY_QUERY_VALUE | KEY_WOW64_64KEY;

	if (RegOpenKeyEx(hKey, szRegKeyPath, 0, dwOption, &mhCtrlNwKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(mhCtrlNwKey, szRegKeyName, NULL, &mdwDataType, (LPBYTE)&dwRegValue, &mdwSize) == ERROR_SUCCESS)
		{
			bStaus = TRUE;
		}

		RegCloseKey(mhCtrlNwKey);
	}//end of if RegOpenKeyEx
	return bStaus;
}

//BOOL CRegHelper::GetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, DWORD & dwRegValue, DWORD & dwResult)
//{
//	DWORD mdwDataType;
//	DWORD mdwSize = sizeof(DWORD);
//	HKEY  mhCtrlNwKey;
//	mdwDataType = REG_DWORD;
//	dwResult = RegOpenKeyEx(hKey, szRegKeyPath, 0, KEY_READ | KEY_QUERY_VALUE | KEY_WOW64_64KEY, &mhCtrlNwKey);
//	if (dwResult == ERROR_SUCCESS)
//	{
//		dwResult = RegQueryValueEx(mhCtrlNwKey, szRegKeyName, NULL, &mdwDataType, (LPBYTE)&dwRegValue, &mdwSize);
//		RegCloseKey(mhCtrlNwKey);
//	}
//	return dwResult;
//}

//Delete the registry key
BOOL  CRegHelper::DeleteRegistryKey(HKEY hKey, const CString& szRegKeyPath, const CString& szKeyName, DWORD dwOptions)
{
	HKEY mhCtrlNwKey = NULL;
	DWORD dwQueryResult = ERROR_SUCCESS;

	if (RegOpenKeyEx(hKey, szRegKeyPath, 0, dwOptions, &mhCtrlNwKey) != ERROR_SUCCESS)
		return FALSE;

	dwQueryResult = RegDeleteKey(mhCtrlNwKey, szKeyName);
	RegCloseKey(mhCtrlNwKey);
	RegCloseKey(hKey);

	if (dwQueryResult == ERROR_SUCCESS)
		return TRUE;
	else
		return FALSE;
}



BOOL CRegHelper::RegDeleteKeyRec(HKEY  hKey, LPCTSTR pszSubKey, BOOL bUseWOW64)
{
	HKEY hkSubKey = NULL;
	REGSAM samDesired = KEY_ENUMERATE_SUB_KEYS | KEY_SET_VALUE | KEY_QUERY_VALUE ;
	if (bUseWOW64)
	{
		samDesired |= KEY_WOW64_64KEY;
	}
	DWORD dwError = RegOpenKeyEx(hKey, pszSubKey, 0, samDesired, &hkSubKey);
	if (dwError != ERROR_SUCCESS) {
		return dwError;
	}
	__try {
		DWORD dwIndex = 0;
		dwError = RegQueryInfoKey(hkSubKey, NULL, NULL, NULL, &dwIndex, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		if (dwError != ERROR_SUCCESS) {
			return dwError;
		}
		while (dwIndex-- > 0) {
			TCHAR szSubKeyName[256] = { 0 };
			DWORD cchSubKeyName = ARRAYSIZE(szSubKeyName);
			dwError = RegEnumKeyEx(hkSubKey, dwIndex, szSubKeyName, &cchSubKeyName, NULL, NULL, NULL, NULL);
			if (dwError != ERROR_SUCCESS) {
				return dwError;
			}
			dwError = RegDeleteKeyRecursively(hkSubKey, szSubKeyName, samDesired);
			if (dwError != ERROR_SUCCESS) {
				return dwError;
			}
		}
	}
	__finally {
		RegCloseKey(hkSubKey);
	}
	return RegDeleteKeyEx(hKey, pszSubKey, samDesired, 0);
}

BOOL CRegHelper::RegDeleteKeyRecursively(HKEY  hKey, LPCTSTR pszSubKey, REGSAM samWowFlag)
{
	HKEY hkSubKey = NULL;
	REGSAM samDesired = KEY_ENUMERATE_SUB_KEYS | KEY_SET_VALUE | KEY_QUERY_VALUE | samWowFlag;
	DWORD dwError = RegOpenKeyEx(hKey, pszSubKey, 0, samDesired, &hkSubKey);
	if (dwError != ERROR_SUCCESS) {
		return dwError;
	}
	__try {
		DWORD dwIndex = 0;
		dwError = RegQueryInfoKey(hkSubKey, NULL, NULL, NULL, &dwIndex, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		if (dwError != ERROR_SUCCESS) {
			return dwError;
		}
		while (dwIndex-- > 0) {
			TCHAR szSubKeyName[256] = { 0 };
			DWORD cchSubKeyName = ARRAYSIZE(szSubKeyName);
			dwError = RegEnumKeyEx(hkSubKey, dwIndex, szSubKeyName, &cchSubKeyName, NULL, NULL, NULL, NULL);
			if (dwError != ERROR_SUCCESS) {
				return dwError;
			}
			dwError = RegDeleteKeyRecursively(hkSubKey, szSubKeyName, samWowFlag);
			if (dwError != ERROR_SUCCESS) {
				return dwError;
			}
		}
	}
	__finally {
		RegCloseKey(hkSubKey);
	}
	return RegDeleteKeyEx(hKey, pszSubKey, samWowFlag, 0);
}

BOOL  CRegHelper::DeleteRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szValueName, BOOL bWow64)
{
	HKEY mhCtrlNwKey = NULL;
	DWORD dwQueryResult = ERROR_SUCCESS;
	if (!bWow64)
	{
		if (RegOpenKeyEx(hKey, szRegKeyPath, 0, KEY_WRITE, &mhCtrlNwKey) != ERROR_SUCCESS)
			return FALSE;
	}
	else
	{
		if (RegOpenKeyEx(hKey, szRegKeyPath, 0, KEY_WRITE |KEY_WOW64_64KEY, &mhCtrlNwKey) != ERROR_SUCCESS)
			return FALSE;
	}


	dwQueryResult = RegDeleteValue(mhCtrlNwKey, szValueName);

	RegCloseKey(hKey);

	if (dwQueryResult == ERROR_SUCCESS)
		return TRUE;
	else
		return FALSE;
}

BOOL CRegHelper::DeleteRegistryValueEx(HKEY hKey, const CString& strRegKeyPath, const CString& strValueName, BOOL bWow64 /*= FALSE*/)
{
	// to delete Reg Value by force,
	//be careful with this func because it must be supported by lrtp.sys or some else
	if (HKEY_LOCAL_MACHINE != hKey) return FALSE;
	CString _strRegKeyPath = L"\\REGISTRY\\MACHINE\\";
	_strRegKeyPath += strRegKeyPath;
	if (!bWow64)
		_strRegKeyPath.Replace(L"SOFTWARE", L"SOFTWARE\\WOW6432Node");

	typedef struct _DelRegKey
	{
		WCHAR KeyName[MAX_PATH];
		WCHAR ValueName[MAX_PATH];
	} DelRegKey, *PDelRegKey;

	DelRegKey tagDelRegKey;
	_tcscpy_s(tagDelRegKey.KeyName, _strRegKeyPath.GetString());
	_tcscpy_s(tagDelRegKey.ValueName, strValueName.GetString());

	HANDLE hDevice = CreateFile(L"\\\\.\\lrtp",
		GENERIC_WRITE | GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (INVALID_HANDLE_VALUE == hDevice) return FALSE;
	DWORD dwBytes = 0;
	if (DeviceIoControl(hDevice, CTL_CODE(FILE_DEVICE_UNKNOWN, 0x807, METHOD_BUFFERED, FILE_ANY_ACCESS), &tagDelRegKey, sizeof(DelRegKey), NULL, NULL, &dwBytes, NULL))
	{
		CloseHandle(hDevice);
		return TRUE;
	}
	CloseHandle(hDevice);
	return FALSE;

}

BOOL CRegHelper::RenameRegistryKey(HKEY hKey, const CString& szSrcRegKeyPath, const CString& szSrcRegKeyName, const CString& szDstRegKeyName, DWORD dwOptions)
{
	CString strRegValue, strMsg;
	HKEY mhCtrlNwKey = NULL;
	DWORD mdwDataType = REG_SZ;
	DWORD mdwSize = LS_MAX_PATH*sizeof(TCHAR);
	if (RegOpenKeyEx(hKey, szSrcRegKeyPath, 0, dwOptions, &mhCtrlNwKey) == ERROR_SUCCESS)
	{
		TCHAR  *szValue = new TCHAR[LS_MAX_PATH];
		memset(szValue, 0, LS_MAX_PATH*sizeof(TCHAR));
		if (RegQueryValueEx(mhCtrlNwKey, szSrcRegKeyName, NULL, &mdwDataType, (LPBYTE)szValue, &mdwSize) == ERROR_SUCCESS)
		{
			strRegValue = szValue;

			mdwSize = (DWORD)strRegValue.GetLength();
			mdwDataType = REG_SZ;
			if (RegSetValueEx(mhCtrlNwKey, szDstRegKeyName, NULL, mdwDataType, (LPBYTE)strRegValue.GetBuffer(), mdwSize*sizeof(WCHAR)) == ERROR_SUCCESS)
			{
				if (RegDeleteValue(mhCtrlNwKey, szSrcRegKeyName) == ERROR_SUCCESS)
				{
					RegCloseKey(mhCtrlNwKey);
					RegCloseKey(hKey);
					strMsg.ReleaseBuffer();
					return TRUE;
				}
			}
		}
		RegCloseKey(mhCtrlNwKey);
		RegCloseKey(hKey);
	}

	strMsg.ReleaseBuffer();

	return FALSE;
}

DWORD CRegHelper::EnumSubValues(HKEY keyDomain, const CString& szRegKeyPath, map<CString, CString>& subValues, bool bWow64)
{
	DWORD dwRet = 0;
	try
	{
		HKEY hKey = NULL;
		dwRet = RegOpenKeyEx(keyDomain, szRegKeyPath, REG_NONE, bWow64 ? KEY_READ | KEY_WOW64_64KEY : KEY_READ, &hKey);
		if (dwRet == ERROR_SUCCESS)
		{
			TCHAR* valueName = NULL;
			TCHAR* data = NULL;
			do
			{
				DWORD nSubKeyCount = 0;
				DWORD nSubValueCount = 0;
				DWORD nSize = 0;
				DWORD nMaxSubKeyLen = 0;
				DWORD nMaxValueNameLen = 0;
				DWORD nMaxValueLen = 0;
				dwRet = RegQueryInfoKey(hKey, 0, 0, 0, &nSubKeyCount, &nMaxSubKeyLen, 0, &nSubValueCount, &nMaxValueNameLen, &nMaxValueLen, 0, 0);
				if (dwRet != ERROR_SUCCESS)
					break;

				nMaxValueNameLen++;
				nMaxValueLen++;
				valueName = new TCHAR[nMaxValueNameLen]{};
				data = new TCHAR[nMaxValueLen]{};

				DWORD dwValueNameSize = 0;
				DWORD dwDataSize = 0;
				DWORD dwType = 0;
				CString valueStr;
				for (int index = 0; index < nSubValueCount; index++)
				{
					dwValueNameSize = nMaxValueNameLen;
					dwDataSize = nMaxValueLen;
					dwRet = ::RegEnumValue(hKey, index, valueName, &dwValueNameSize, 0, &dwType, (PUCHAR)data, &dwDataSize);
					if (dwRet == ERROR_SUCCESS)
					{
						CString ValueName(valueName);
						if (dwType == REG_SZ || dwType == REG_EXPAND_SZ)
						{
							CString Data(data);
							subValues.insert(std::make_pair(ValueName, Data));
						}
						else if (dwType == REG_DWORD)
						{
							valueStr.Format(L"%u", *(DWORD*)data);
							subValues.insert(std::make_pair(ValueName, valueStr));
						}
						else if (dwType == REG_QWORD)
						{
							valueStr.Format(L"%I64d", *(LONGLONG*)data);
							subValues.insert(std::make_pair(ValueName, valueStr));
						}
					}
				}
			} while (false);
			if (valueName)
				delete[] valueName;
			if (data)
				delete[] data;
			RegCloseKey(hKey);
		}
	}
	catch (...)
	{
	}
	return dwRet;
}

DWORD CRegHelper::EnumSubKeys(HKEY keyDomain, const CString& szRegKeyPath, std::vector<CString>& subKeys, bool bWow64)
{
	DWORD dwRet = 0;
	try
	{
		subKeys.clear();
		HKEY hKey = NULL;
		dwRet = RegOpenKeyEx(keyDomain, szRegKeyPath, REG_NONE, bWow64 ? KEY_READ | KEY_WOW64_64KEY : KEY_READ, &hKey);
		if (dwRet == ERROR_SUCCESS)
		{
			//TCHAR achKey[MAX_PATH] = { 0 };
			DWORD nSubKeyCount = 0;
			DWORD nSize = 0;
			DWORD nMaxSubKeyLen = 0;
			dwRet = RegQueryInfoKey(hKey, 0, 0, 0, &nSubKeyCount, &nMaxSubKeyLen, 0, 0, 0, 0, 0, 0);
			if (dwRet == ERROR_SUCCESS)
			{
				nSize = nMaxSubKeyLen + 1;
				TCHAR* achKey = new TCHAR[nSize];
				if (!achKey)
					return dwRet;
				size_t bufferLen = (nMaxSubKeyLen + 1) * sizeof(TCHAR);
				for (int index = 0; index < nSubKeyCount; ++index)
				{
					ZeroMemory(achKey, bufferLen);
					dwRet = ::RegEnumKey(hKey, index, achKey, nSize);
					if (ERROR_SUCCESS == dwRet)
						subKeys.push_back(achKey);
				}
				delete[] achKey;
			}
			RegCloseKey(hKey);
		}
	}
	catch (...)
	{
	}
	
	return dwRet;
}

BOOL CRegHelper::GetUserRegValue(LPCTSTR lpKeyName, LPCTSTR lpValueName, DWORD& dwValue)
{
	return GetRegistryValue(HKEY_CURRENT_USER, lpKeyName, lpValueName, dwValue);	
}

BOOL CRegHelper::GetUserRegValueS(LPCTSTR lpKeyName, LPCTSTR lpValueName, CString& szValue)
{
	return GetRegistryValue(HKEY_CURRENT_USER, lpKeyName, lpValueName, szValue);
}

BOOL CRegHelper::SetUserRegValue(LPCTSTR lpKeyName, LPCTSTR lpValueName, DWORD dwValue)
{
	return SetRegistryValue(HKEY_CURRENT_USER, lpKeyName, lpValueName, dwValue);
}

BOOL CRegHelper::GetCurUserRegValue(LPCTSTR lpKeyName, LPCTSTR lpValueName, DWORD& dwValue)
{
	//LONG lResult;
	HKEY hKey = NULL;
	PSID pSid = NULL;
	CString strSid, sKey;
	DWORD dwGet = 0;
	BOOL bSucc = FALSE;
	CImpersonateUser ImpUser;
	ImpUser.ImpersonateCurrentUser();

	return GetCurrentUserRegistryValue(lpKeyName, lpValueName, dwValue);
	/*
	TCHAR szBuf[MAX_PATH] = L"";
	DWORD dwRet = MAX_PATH;
	if (!GetUserName(szBuf, &dwRet))
	{
		//event_log(L"GetUserName=%ws  code = %d ", szBuf, GetLastError());
		return FALSE;
	};

	if (GetUserSid(szBuf, &pSid))
	{
		//get the hiden key name
		GetSidString(pSid, strSid);

		sKey = strSid + L"\\";
		sKey += lpKeyName;
		////event_log(L"key = %ws", sKey);

		DWORD dwSize = sizeof(DWORD);
		DWORD dwType = REG_DWORD;

		lResult = RegOpenKeyEx(HKEY_USERS, sKey, NULL, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &hKey);
		//event_log(L"RegOpenKeyEx  code = %d ", GetLastError() );
		if (lResult != ERROR_SUCCESS)
			return FALSE;

		if (RegQueryValueEx(hKey,
			lpValueName,
			0,
			&dwType,
			(LPBYTE)(&dwGet),
			&dwSize) == ERROR_SUCCESS)
		{
			bSucc = TRUE;
			dwValue = dwGet;
		}
		//event_log(L"RegQueryValueEx %ws = %d	 error = %d", lpValueName, dwValue, GetLastError());
		RegCloseKey(hKey);

		//if (RegGetValue(HKEY_USERS,
		//	sKey, 
		//	lpValueName,
		//	RRF_RT_DWORD,
		//	&dwType,
		//	(LPBYTE)&dwValue,
		//	&dwSize) == ERROR_SUCCESS)
		//{
		//	;
		//}
		//event_log(L"RegGetValue %ws = %d	 error = %d", lpValueName, dwValue, GetLastError());
	}
	else
	{
		//event_log(L"GetUserSid=%ws  code = %d ", szBuf, GetLastError());
		return FALSE;
	}
	ImpUser.Revert();

	return bSucc;
	*/
}

BOOL CRegHelper::GetCurUserRegValueS(LPCTSTR lpKeyName, LPCTSTR lpValueName, CString& szValue)
{
	//LONG lResult;
	HKEY hKey = NULL;
	PSID pSid = NULL;
	CString strSid, sKey;
	//TCHAR szGet[MAX_PATH];
	BOOL bSucc = FALSE;
	CImpersonateUser ImpUser;
	ImpUser.ImpersonateCurrentUser();

	return GetCurrentUserRegistryValue(lpKeyName, lpValueName, szValue);
	/*
	TCHAR szBuf[MAX_PATH] = L"";
	DWORD dwRet = MAX_PATH;
	GetUserName(szBuf, &dwRet);

	if (GetUserSid(szBuf, &pSid))
	{
		//get the hiden key name
		GetSidString(pSid, strSid);

		sKey = strSid + L"\\";
		sKey += lpKeyName;
		////event_log(L"key = %ws", sKey);

		DWORD dwSize = MAX_PATH;
		DWORD dwType = REG_SZ;

		lResult = RegOpenKeyEx(HKEY_USERS, sKey, NULL, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &hKey);
		//event_log(L"RegOpenKeyEx  code = %d ", GetLastError() );
		if (lResult != ERROR_SUCCESS)
			return FALSE;

		if (RegQueryValueEx(hKey,
			lpValueName,
			0,
			&dwType,
			(LPBYTE)(&szGet),
			&dwSize) == ERROR_SUCCESS && dwSize>0)
		{
			bSucc = TRUE;
			szValue = szGet;
		}
		//event_log(L"RegQueryValueEx %ws = %s	 error = %d", lpValueName, szValue, GetLastError());
		RegCloseKey(hKey);

		//if (RegGetValue(HKEY_USERS,
		//	sKey, 
		//	lpValueName,
		//	RRF_RT_DWORD,
		//	&dwType,
		//	(LPBYTE)&dwValue,
		//	&dwSize) == ERROR_SUCCESS)
		//{
		//	;
		//}
		//event_log(L"RegGetValue %ws = %d	 error = %d", lpValueName, dwValue, GetLastError());
	}

	ImpUser.Revert();

	return bSucc;
	*/
}


BOOL CRegHelper::SetCurUserRegValueS(LPCTSTR lpKeyName, LPCTSTR lpValueName, CString szValue)
{
	LONG lResult;
	HKEY hKey = NULL;
	PSID pSid = NULL;

	CImpersonateUser ImpUser;
	ImpUser.ImpersonateCurrentUser();


	TCHAR szBuf[MAX_PATH] = L"";
	DWORD dwRet = MAX_PATH;
	GetUserName(szBuf, &dwRet);

	CString strSid, sKey;
	if (GetUserSid(szBuf, &pSid))
	{
		//get the hiden key name
		GetSidString(pSid, strSid);

		sKey = strSid + L"\\";
		sKey += lpKeyName;
		//event_log(L"key = %ws", sKey);
		lResult = RegOpenKeyEx(HKEY_USERS, sKey, NULL, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &hKey);
		//event_log(L"RegOpenKeyEx HKEY_USERS %ws error = %d", lpValueName, GetLastError());
		if (lResult != ERROR_SUCCESS)
			return FALSE;

		if (RegSetValueEx(hKey,
			lpValueName,
			0,
			REG_SZ,
			(LPBYTE)(szValue.GetBuffer()),
			sizeof(DWORD)) != ERROR_SUCCESS)
		{
			;// event_log(L"write registry error = %d", GetLastError());
		}
		//event_log(L"write registry userquit = %d	 error = %d", dwValue, GetLastError());
		RegCloseKey(hKey);
	}

	ImpUser.Revert();
	return TRUE;
}

BOOL CRegHelper::SetCurUserRegValue(LPCTSTR lpKeyName, LPCTSTR lpValueName, DWORD dwValue)
{	
	LONG lResult;
	HKEY hKey = NULL;
	PSID pSid = NULL;

	CImpersonateUser ImpUser;
	ImpUser.ImpersonateCurrentUser();


	TCHAR szBuf[MAX_PATH] = L"";
	DWORD dwRet = MAX_PATH;
	GetUserName(szBuf, &dwRet);

	CString strSid, sKey;
	if (GetUserSid(szBuf, &pSid))
	{
		//get the hiden key name
		GetSidString(pSid, strSid);

		sKey = strSid + L"\\";
		sKey += lpKeyName;
		//event_log(L"key = %ws", sKey);
		lResult = RegOpenKeyEx(HKEY_USERS, sKey, NULL, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &hKey);
		//event_log(L"RegOpenKeyEx HKEY_USERS %ws error = %d", lpValueName, GetLastError());
		if (lResult != ERROR_SUCCESS)
			return FALSE;

		if (RegSetValueEx(hKey,
			lpValueName,
			0,
			REG_DWORD,
			(LPBYTE)(&dwValue),
			sizeof(DWORD)) != ERROR_SUCCESS)
		{
			;// event_log(L"write registry error = %d", GetLastError());
		}
		//event_log(L"write registry userquit = %d	 error = %d", dwValue, GetLastError());
		RegCloseKey(hKey);
	}

	ImpUser.Revert();
	return TRUE;
}

BOOL CRegHelper::GetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, DWORD64& dwRegValue, BOOL bUseWOW )
{
	DWORD mdwDataType;
	DWORD mdwSize = sizeof(DWORD64);
	HKEY  mhCtrlNwKey;
	bool bStaus = FALSE;
	mdwDataType = REG_QWORD;
	CString strMsg;

	DWORD dwOption = KEY_READ | KEY_QUERY_VALUE ;
	if (bUseWOW)
		dwOption = KEY_READ | KEY_QUERY_VALUE | KEY_WOW64_64KEY;

	if (RegOpenKeyEx(hKey, szRegKeyPath, 0, dwOption, &mhCtrlNwKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(mhCtrlNwKey, szRegKeyName, NULL, &mdwDataType, (LPBYTE)&dwRegValue, &mdwSize) == ERROR_SUCCESS)
		{
			//strMsg.Format(TEXT("CRegHelper::GetRegistryValue Passed for %s"), szRegKeyName.GetBuffer());
			//szRegKeyName.ReleaseBuffer();
			bStaus = TRUE;
		}
		else
		{
			//		strMsg.Format(TEXT("CRegHelper::GetRegistryValue for %s Failed with error 0x%08lx"), szRegKeyName.GetBuffer(), GetLastError());
			//		szRegKeyName.ReleaseBuffer();
		}

		//#ifdef _DEBUG
		//		::OutputDebugString(strMsg);
		//#endif

		RegCloseKey(mhCtrlNwKey);
	}//end of if RegOpenKeyEx
	return bStaus;
}


BOOL CRegHelper::SetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, DWORD64 dwRegValue, bool bUseWOW/* = TRUE*/)
{
	DWORD mdwDataType;
	HKEY  mhCtrlNwKey;
	bool bStaus = FALSE;
	mdwDataType = REG_QWORD;

	DWORD l_dwUseWOW = KEY_ALL_ACCESS;
	if (bUseWOW)
		l_dwUseWOW |= KEY_WOW64_64KEY;
	//dwRegValue = dwRegValue;
	CString strMsg;
	if (RegCreateKeyEx(hKey, szRegKeyPath, 0, NULL, REG_OPTION_NON_VOLATILE, l_dwUseWOW, NULL, &mhCtrlNwKey, NULL) == ERROR_SUCCESS)
	{
		//wprintf(TEXT("RegOpenKeyEx Passed for %s"), szRegKeyPath.GetBuffer());
		//szRegKeyPath.ReleaseBuffer();
		if (RegSetValueEx(mhCtrlNwKey, szRegKeyName, NULL, mdwDataType, (PUCHAR)&dwRegValue, sizeof(dwRegValue)) == ERROR_SUCCESS)
		{
			strMsg.Format(TEXT("CRegHelper::SetRegistryValue Passed for %s"), szRegKeyName);
			//szRegKeyName.ReleaseBuffer();
			bStaus = TRUE;
			RegFlushKey(mhCtrlNwKey);
		}
		else
		{
			//strMsg.Format(TEXT("CRegHelper::SetRegistryValue for %s Failed with error 0x%08lx"), szRegKeyName.GetBuffer(), GetLastError());
			//szRegKeyName.ReleaseBuffer();
		}
		//#ifdef _DEBUG
		//		::OutputDebugString(strMsg);
		//#endif
		RegCloseKey(mhCtrlNwKey);
	}//end of if RegOpenKeyEx
	return bStaus;
}

BOOL CRegHelper::SetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, LPVOID pRegValue, DWORD dwSize)
{
	DWORD mdwDataType;
	HKEY  mhCtrlNwKey;
	bool bStaus = FALSE;
	mdwDataType = REG_BINARY;
	//dwRegValue = dwRegValue;
	CString strMsg;
	if (RegCreateKeyEx(hKey, szRegKeyPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | KEY_WOW64_64KEY, NULL, &mhCtrlNwKey, NULL) == ERROR_SUCCESS)
	{
		//wprintf(TEXT("RegOpenKeyEx Passed for %s"), szRegKeyPath.GetBuffer());
		//szRegKeyPath.ReleaseBuffer();
		if (RegSetValueEx(mhCtrlNwKey, szRegKeyName, NULL, mdwDataType, (PUCHAR)pRegValue, dwSize) == ERROR_SUCCESS)
		{
			//strMsg.Format(TEXT("CRegHelper::SetRegistryValue Passed for %s"), szRegKeyName.GetBuffer());
			//szRegKeyName.ReleaseBuffer();
			bStaus = TRUE;
			RegFlushKey(mhCtrlNwKey);
		}
		else
		{
			//strMsg.Format(TEXT("CRegHelper::SetRegistryValue for %s Failed with error 0x%08lx"), szRegKeyName.GetBuffer(), GetLastError());
			//szRegKeyName.ReleaseBuffer();
		}
		//#ifdef _DEBUG
		//		::OutputDebugString(strMsg);
		//#endif
		RegCloseKey(mhCtrlNwKey);
	}//end of if RegOpenKeyEx
	return bStaus;
}

BOOL CRegHelper::GetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, LPVOID pRegValue, DWORD& dwSize)
{
	DWORD mdwDataType;
	DWORD mdwSize = sizeof(DWORD64);
	HKEY  mhCtrlNwKey;
	bool bStaus = FALSE;
	mdwDataType = REG_BINARY;
	CString strMsg;

	if (RegOpenKeyEx(hKey, szRegKeyPath, 0, KEY_READ | KEY_QUERY_VALUE | KEY_WOW64_64KEY, &mhCtrlNwKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(mhCtrlNwKey, szRegKeyName, NULL, &mdwDataType, (LPBYTE)pRegValue, &dwSize) == ERROR_SUCCESS)
		{
			//strMsg.Format(TEXT("CRegHelper::GetRegistryValue Passed for %s"), szRegKeyName.GetBuffer());
			//szRegKeyName.ReleaseBuffer();
			bStaus = TRUE;
		}
		else
		{
			//		strMsg.Format(TEXT("CRegHelper::GetRegistryValue for %s Failed with error 0x%08lx"), szRegKeyName.GetBuffer(), GetLastError());
			//		szRegKeyName.ReleaseBuffer();
		}

		//#ifdef _DEBUG
		//		::OutputDebugString(strMsg);
		//#endif

		RegCloseKey(mhCtrlNwKey);
	}//end of if RegOpenKeyEx
	return bStaus;
}


//@002A end

//BOOL CRegHelper::GetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, CString& szRegValue)
//{
//
//	DWORD mdwDataType;
//	DWORD mdwSize;
//	HKEY  mhCtrlNwKey;
//	bool bStaus = FALSE;
//	mdwDataType = REG_SZ;
//	mdwSize = LS_MAX_PATH * sizeof(TCHAR);
//	try
//	{
//		CString strMsg;
//		TCHAR* szValue = new TCHAR[LS_MAX_PATH];
//		if (szValue)
//		{
//			memset(szValue, 0, LS_MAX_PATH * sizeof(TCHAR));
//
//			if (RegOpenKeyEx(hKey, szRegKeyPath, 0, KEY_READ | KEY_QUERY_VALUE | KEY_WOW64_64KEY, &mhCtrlNwKey) == ERROR_SUCCESS)
//			{
//				if (RegQueryValueEx(mhCtrlNwKey, szRegKeyName, NULL, &mdwDataType, (LPBYTE)szValue, &mdwSize) == ERROR_SUCCESS)
//				{
//					szRegValue = szValue;
//					bStaus = TRUE;
//				}
//
//				RegCloseKey(mhCtrlNwKey);
//			}//end of if RegOpenKeyEx
//			//DWORD dwError = GetLastError();
//			delete[] szValue;
//		}
//	}
//	catch (std::bad_alloc)
//	{
//
//	}
//	return bStaus;
//}

BOOL CRegHelper::GetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, CString& szRegValue, BOOL bUseWOW)
{
	DWORD mdwDataType;
	DWORD mdwSize;
	HKEY  mhCtrlNwKey;
	bool bStaus = FALSE;
	mdwDataType = REG_SZ;
	mdwSize = LS_MAX_PATH * sizeof(TCHAR);
	try
	{
		CString strMsg;
		TCHAR* szValue = new TCHAR[LS_MAX_PATH];
		if (szValue)
		{
			memset(szValue, 0, LS_MAX_PATH * sizeof(TCHAR));
			LONG l_ret = ERROR_SUCCESS;
			if (bUseWOW)
				l_ret = RegOpenKeyEx(hKey, szRegKeyPath, 0, KEY_READ | KEY_QUERY_VALUE | KEY_WOW64_64KEY, &mhCtrlNwKey);
			else
				l_ret = RegOpenKeyEx(hKey, szRegKeyPath, 0, KEY_READ | KEY_QUERY_VALUE, &mhCtrlNwKey);

			if (l_ret == ERROR_SUCCESS)
			{
				if (RegQueryValueEx(mhCtrlNwKey, szRegKeyName, NULL, &mdwDataType, (LPBYTE)szValue, &mdwSize) == ERROR_SUCCESS)
				{
					szRegValue = szValue;
					bStaus = TRUE;
				}
				RegCloseKey(mhCtrlNwKey);
			}//end of if RegOpenKeyEx
			delete[] szValue;
		}
	}
	catch (std::bad_alloc)
	{

	}
	return bStaus;
}

BOOL CRegHelper::GetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, CString & szRegValue, DWORD & dwResutl, BOOL bUseWOW)
{
	DWORD mdwDataType;
	DWORD mdwSize;
	HKEY  mhCtrlNwKey;
	mdwDataType = REG_SZ;
	mdwSize = LS_MAX_PATH * sizeof(TCHAR);
	try
	{
		CString strMsg;
		TCHAR  *szValue = new TCHAR[LS_MAX_PATH];
		if (szValue)
		{
			memset(szValue, 0, LS_MAX_PATH * sizeof(TCHAR));
			dwResutl = ERROR_SUCCESS;
			if (bUseWOW)
				dwResutl = RegOpenKeyEx(hKey, szRegKeyPath, 0, KEY_READ | KEY_QUERY_VALUE | KEY_WOW64_64KEY, &mhCtrlNwKey);
			else
				dwResutl = RegOpenKeyEx(hKey, szRegKeyPath, 0, KEY_READ | KEY_QUERY_VALUE, &mhCtrlNwKey);

			if (dwResutl == ERROR_SUCCESS)
			{
				dwResutl = RegQueryValueEx(mhCtrlNwKey, szRegKeyName, NULL, &mdwDataType, (LPBYTE)szValue, &mdwSize);
				if (dwResutl == ERROR_SUCCESS)
				{
					szRegValue = szValue;
				}
				RegCloseKey(mhCtrlNwKey);
			}//end of if RegOpenKeyEx
			delete[] szValue;
		}
	}
	catch (std::bad_alloc)
	{

	}
	return dwResutl == ERROR_SUCCESS;
}

BOOL CRegHelper::SetSamRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, CString szRegValue, REGSAM dwSamEx)
{
	//DWORD mdwDataType;
	DWORD mdwSize;
	HKEY  mhCtrlNwKey;
	bool bStaus = FALSE;
	mdwSize = (DWORD)szRegValue.GetLength();
	if (RegCreateKeyEx(hKey, szRegKeyPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | dwSamEx, NULL, &mhCtrlNwKey, NULL) == ERROR_SUCCESS)
	{
		if (RegSetValueEx(mhCtrlNwKey, szRegKeyName, NULL, REG_SZ, (LPBYTE)szRegValue.GetBuffer(), mdwSize * sizeof(WCHAR)) == ERROR_SUCCESS)
		{
			bStaus = TRUE;
			RegFlushKey(mhCtrlNwKey);
		}

		RegCloseKey(mhCtrlNwKey);
	}//end of if RegOpenKeyEx
	return bStaus;
}


BOOL CRegHelper::GetSamRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, CString& szRegValue, REGSAM dwSamEx)
{
	DWORD mdwDataType;
	DWORD mdwSize;
	HKEY  mhCtrlNwKey;
	bool bStaus = FALSE;
	mdwDataType = REG_SZ;
	mdwSize = LS_MAX_PATH * sizeof(TCHAR);
	try
	{
		CString strMsg;
		TCHAR  *szValue = new TCHAR[LS_MAX_PATH];
		if (szValue)
		{
			memset(szValue, 0, LS_MAX_PATH * sizeof(TCHAR));
			if (ERROR_SUCCESS == RegOpenKeyEx(hKey, szRegKeyPath, 0, KEY_READ | KEY_QUERY_VALUE | dwSamEx, &mhCtrlNwKey))
			{
				if (RegQueryValueEx(mhCtrlNwKey, szRegKeyName, NULL, &mdwDataType, (LPBYTE)szValue, &mdwSize) == ERROR_SUCCESS)
				{
					szRegValue = szValue;
					bStaus = TRUE;
				}
				RegCloseKey(mhCtrlNwKey);
			}//end of if RegOpenKeyEx
			delete[] szValue;
		}
	}
	catch (std::bad_alloc)
	{

	}
	return bStaus;
}

BOOL CRegHelper::SetSamRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, DWORD dwRegValue, REGSAM dwSamEx)
{
	DWORD mdwDataType;
	HKEY  mhCtrlNwKey;
	bool bStaus = FALSE;
	mdwDataType = REG_DWORD;
	if (RegCreateKeyEx(hKey, szRegKeyPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | dwSamEx, NULL, &mhCtrlNwKey, NULL) == ERROR_SUCCESS)
	{
		if (RegSetValueEx(mhCtrlNwKey, szRegKeyName, NULL, mdwDataType, (PUCHAR)&dwRegValue, sizeof(dwRegValue)) == ERROR_SUCCESS)
		{
			bStaus = TRUE;
			RegFlushKey(mhCtrlNwKey);
		}
		RegCloseKey(mhCtrlNwKey);
	}//end of if RegOpenKeyEx
	return bStaus;
}

BOOL CRegHelper::GetSamRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, DWORD& dwRegValue, REGSAM dwSamEx)
{
	DWORD mdwDataType;
	DWORD mdwSize = sizeof(DWORD);
	HKEY  mhCtrlNwKey;
	bool bStaus = FALSE;
	mdwDataType = REG_DWORD;

	if (RegOpenKeyEx(hKey, szRegKeyPath, 0, KEY_READ | KEY_QUERY_VALUE | dwSamEx, &mhCtrlNwKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(mhCtrlNwKey, szRegKeyName, NULL, &mdwDataType, (LPBYTE)&dwRegValue, &mdwSize) == ERROR_SUCCESS)
		{
			bStaus = TRUE;
		}

		RegCloseKey(mhCtrlNwKey);
	}//end of if RegOpenKeyEx
	return bStaus;
}


void CRegHelper::GetSidString(PSID pSid, CString& strSid)
{
	//convert SID to string
	TCHAR szBuffer[256];
	SID_IDENTIFIER_AUTHORITY *psia = ::GetSidIdentifierAuthority(pSid);
	DWORD dwTopAuthority = psia->Value[5];
	_stprintf_s(szBuffer, 256, _T("S-1-%lu"), dwTopAuthority);

	TCHAR szTemp[32];
	int iSubAuthorityCount = *(GetSidSubAuthorityCount(pSid));
	for (int i = 0; i<iSubAuthorityCount; i++)
	{
		DWORD dwSubAuthority = *(GetSidSubAuthority(pSid, i));
		_stprintf_s(szTemp, 32, _T("%lu"), dwSubAuthority);
		_tcscat_s(szBuffer, _T("-"));
		_tcscat_s(szBuffer, szTemp);
	}

	strSid = szBuffer;
}

BOOL ParseRegPart(CString regPath, HKEY& regKey,CString& realRegPath, CString&regName) {
	if (regPath.Find(_T("HKEY_CURRENT_USER")) == 0)
	{
		regKey = HKEY_CURRENT_USER;
	}
	else if (regPath.Find(_T("HKEY_LOCAL_MACHINE")) == 0)
	{
		regKey = HKEY_LOCAL_MACHINE;
	}
	else if (regPath.Find(_T("HKEY_CLASSES_ROOT")) == 0)
	{
		regKey = HKEY_CLASSES_ROOT;
	}
	else if (regPath.Find(_T("HKEY_USERS")) == 0)
	{
		regKey = HKEY_USERS;
	}
	else
	{
		return FALSE;
	}

	CString path = regPath.Right(regPath.GetLength() - regPath.Find(L"\\") - 1);
	regName = path.Right(path.GetLength() - path.ReverseFind('\\') - 1);
	realRegPath = path.Left(path.ReverseFind('\\'));
	if (regName.GetLength()==0 || realRegPath.GetLength() == 0){
		return FALSE;
	}
	return TRUE;
}
BOOL CRegHelper::GetRegistryString(CString regPath, CString& result, bool wow64)
{
	HKEY regKey;
	CString realRegPath, regName;
	if (!ParseRegPart(regPath, regKey, realRegPath, regName)) {
		return FALSE;
	}
	return CRegHelper::GetRegistryValue(regKey, realRegPath, regName, result, wow64);
}

BOOL CRegHelper::GetRegistryInt(CString regPath, DWORD64& value, bool wow /*= false*/)
{
	HKEY regKey;
	CString realRegPath, regName;
	if (!ParseRegPart(regPath, regKey, realRegPath, regName)) {
		return FALSE;
	}
	return CRegHelper::GetRegistryValue(regKey, realRegPath, regName, value, wow);
}

BOOL CRegHelper::GetUserSid(LPTSTR UserName, PSID *ppSid)
{
	PSID pSID = NULL;
	DWORD cbSid = 0;
	LPTSTR DomainName = NULL;
	DWORD cbDomainName = 0;
	SID_NAME_USE SIDNameUse;
	BOOL  bDone = FALSE;

	__try
	{
		if (!LookupAccountName(NULL,
			UserName,
			pSID,
			&cbSid,
			DomainName,
			&cbDomainName,
			&SIDNameUse))
		{
			pSID = (PSID)malloc((size_t)cbSid);
			DomainName = (LPTSTR)malloc(cbDomainName * sizeof(TCHAR));
			if (!pSID || !DomainName)
			{
				//throw;
				//ASSERT(("Not enough memory! failed...\n"));
				__leave;
			}
			if (!LookupAccountName(NULL,
				UserName,
				pSID,
				&cbSid,
				DomainName,
				&cbDomainName,
				&SIDNameUse))
			{
				//throw;
				//assert("Not enough memory! failed...\n");
				__leave;

			}
			bDone = TRUE;
		}
	}
	__finally
	{
		if (DomainName)
			free(DomainName);
		if (!bDone && pSID)
			free(pSID);
	}

	if (bDone)
	{
		*ppSid = pSID;
	}

	return bDone;
}


#pragma endregion
