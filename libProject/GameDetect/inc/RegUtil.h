// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include "UserPrivilegeUtil.h"
#include "GroupUser.h"

#define REG_MAX_TREE_COUNT 512
#define REG_MAX_DATA_COUNT 4096
#define IS_KEY_LEN 1024


class CRegUtil
{
public:
	CRegUtil()
	{

	}
	~CRegUtil()
	{
	}


public:
	/*Open reg*/

	static BOOL OpenKey(__in HKEY hRootKey,
		__in LPCTSTR lpRunKey,
		__out HKEY &hKey,
		__in_opt DWORD option = KEY_READ)
	{
		HKEY Key = NULL;
 
 
		HKEY RealRootKey = hRootKey;
		WCHAR keypath[REG_MAX_TREE_COUNT] = { 0 };
		CUserPrivilegeUtil userprivutil;
		hKey = NULL;
		if (hRootKey == HKEY_CURRENT_USER)
		{
			if (userprivutil.IsSystemProcess())
			{
				wstring cur_user;
				wstring cur_domain;
				wstring cur_user_sid;
				DBG_PRINTF_FL((L"userprivutil.IsSystemProcess"));
				if (GroupUser::GetCurLogonUser(&cur_user, &cur_domain, nullptr) == 0)
				{
					DBG_PRINTF_FL((L"GroupUser::GetCurLogonUser %s, %s",cur_user.c_str(), cur_domain.c_str()));

					wstring cur_domain_user = cur_domain;
					cur_domain_user += L"\\";
					cur_domain_user += cur_user;
					if (GroupUser::GetSIDFromUserName(cur_domain_user.c_str(), &cur_user_sid, nullptr) == 0)
					{
						DBG_PRINTF_FL((L"GroupUser::GetSIDFromUserName %s", cur_user_sid.c_str()));
						if (cur_user_sid.length() > 0)
						{
							RealRootKey = HKEY_USERS;
							swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s\\%s", cur_user_sid.c_str(), lpRunKey);
						}
						else
							swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);
					}
				}
			}
			else
				swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);
		}
		else
			swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);

		if (RegOpenKeyEx(RealRootKey,
			keypath,
			0,
			option,
			&Key) == ERROR_SUCCESS)
		{
			hKey = Key;
			return TRUE;
		}
		return FALSE;
	}

	static BOOL SetKeyValue(__in HKEY hRootKey,
		__in LPCTSTR lpRunKey,
		__in const WCHAR *wcKeyName,
		__in PVOID keyvalue,
		__in ULONG keyvaluelength,
		__in_opt ULONG type = REG_DWORD,
		__in_opt BOOL bcreatekey = FALSE)
	{
		HKEY Key = NULL;
		DWORD err = 0;
		BOOL result = FALSE;
		HKEY RealRootKey = hRootKey;
		WCHAR keypath[REG_MAX_TREE_COUNT] = { 0 };
		const WCHAR *strsid = NULL;
		CUserPrivilegeUtil userprivutil;
		if (hRootKey == HKEY_CURRENT_USER)
		{
			if (userprivutil.IsSystemProcess())
			{
				strsid = userprivutil.GetCurrentUserSIDString();
				if (strsid)
				{
					RealRootKey = HKEY_USERS;
					swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s\\%s", strsid, lpRunKey);
				}
				else
					swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);
			}
			else
				swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);
		}
		else
			swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);

		if (RegOpenKeyEx(RealRootKey,
			keypath,
			0,
			KEY_QUERY_VALUE | KEY_SET_VALUE,
			&Key) != ERROR_SUCCESS)
		{
			if (!bcreatekey)
				return FALSE;//no need clean registry
			else
			{
				if (RegCreateKeyEx(RealRootKey,
					keypath,
					0,
					NULL,
					REG_OPTION_NON_VOLATILE,
					KEY_QUERY_VALUE | KEY_SET_VALUE | KEY_CREATE_SUB_KEY,
					NULL,
					&Key,
					NULL) != ERROR_SUCCESS)
				{					
					return FALSE;//no need clean registry
				}
			}
		}

		if (RegSetValueEx(Key,
			wcKeyName,
			NULL,
			type,
			(LPBYTE)keyvalue,
			keyvaluelength) != ERROR_SUCCESS)
		{
			err = GetLastError();
			goto CLEAN;
		}

		result = TRUE;

	CLEAN:
		if (Key)
			RegCloseKey(Key);
		return result;
	}

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
		const WCHAR *strsid = NULL;
		CUserPrivilegeUtil userprivutil;
		if (wcKeyName == NULL)
			return false;
		if (hRootKey == HKEY_CURRENT_USER)
		{
			if (userprivutil.IsSystemProcess())
			{
				strsid = userprivutil.GetCurrentUserSIDString();
				if (strsid)
				{
					RealRootKey = HKEY_USERS;
					swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s\\%s", strsid, lpRunKey);
				}
				else
					swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);
			}
			else
				swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);
		}
		else
			swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);

		if (RegOpenKeyEx(RealRootKey,
			keypath,
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

	static std::wstring GetKeyValueStr(__in HKEY hRootKey,
		__in LPCTSTR lpRunKey,
		__in LPCTSTR wcKeyName,
		__in BOOL is32 = FALSE)
	{
		HKEY Key;
		DWORD dwDisp = 0;
		WCHAR wcData[REG_MAX_DATA_COUNT] = { 0 };
		HKEY RealRootKey = hRootKey;
		WCHAR keypath[REG_MAX_TREE_COUNT] = { 0 };
		const WCHAR *strsid = NULL;
		CUserPrivilegeUtil userprivutil;
		if (wcKeyName == NULL)
			return L"";
		if (hRootKey == HKEY_CURRENT_USER)
		{
			if (userprivutil.IsSystemProcess())
			{
				strsid = userprivutil.GetCurrentUserSIDString();
				if (strsid)
				{
					RealRootKey = HKEY_USERS;
					swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s\\%s", strsid, lpRunKey);
				}
				else
					swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);
			}
			else
				swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);
		}
		else
			swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);
	
		if (RegOpenKeyEx(RealRootKey,
			keypath,
			0,
			(is32 ? KEY_WOW64_32KEY | KEY_QUERY_VALUE : KEY_QUERY_VALUE),
			&Key) != ERROR_SUCCESS)
		{
			return L"";
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
			return L"";
		}

		RegCloseKey(Key);
		return wcData;
	}

	static DWORD GetKeyValueType(__in HKEY hRootKey,
		__in LPCTSTR lpRunKey,
		__in WCHAR *wcKeyName)
	{
		HKEY Key;
		DWORD dwDisp = 0;
		static WCHAR wcData[MAX_PATH];
		SecureZeroMemory(wcData, sizeof(wcData));
		HKEY RealRootKey = hRootKey;
		WCHAR keypath[REG_MAX_TREE_COUNT] = { 0 };
		const WCHAR *strsid = NULL;
		CUserPrivilegeUtil userprivutil;
		DWORD type = 0xFF;
		if (hRootKey == HKEY_CURRENT_USER)
		{
			if (userprivutil.IsSystemProcess())
			{
				strsid = userprivutil.GetCurrentUserSIDString();
				if (strsid)
				{
					RealRootKey = HKEY_USERS;
					swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s\\%s", strsid, lpRunKey);
				}
				else
					swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);
			}
			else
				swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);
		}
		else
			swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);

		if (RegOpenKeyEx(RealRootKey,
			keypath,
			0,
			KEY_QUERY_VALUE,
			&Key) != ERROR_SUCCESS)
		{
			return NULL;
		}

		dwDisp = sizeof(wcData);
		if (RegQueryValueEx(Key,
			wcKeyName,
			NULL,
			&type,
			NULL,
			NULL) != ERROR_SUCCESS)
		{
			RegCloseKey(Key);
			return NULL;
		}

		RegCloseKey(Key);
		return type;
	}
	
	static BOOL DeleteKeyValue(__in HKEY hRootKey,
		__in LPCTSTR lpRunKey,
		__in const WCHAR *wcKeyName)
	{
		HKEY Key = NULL;
		DWORD dwDisp = 0;
		BOOL result = FALSE;
		HKEY RealRootKey = hRootKey;
		WCHAR keypath[REG_MAX_TREE_COUNT] = { 0 };
		const WCHAR *strsid = NULL;
		CUserPrivilegeUtil userprivutil;
		if (hRootKey == HKEY_CURRENT_USER)
		{
			if (userprivutil.IsSystemProcess())
			{
				strsid = userprivutil.GetCurrentUserSIDString();
				if (strsid)
				{
					RealRootKey = HKEY_USERS;
					swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s\\%s", strsid, lpRunKey);
				}
				else
					swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);
			}
			else
				swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);
		}
		else
			swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);
		if (RegOpenKeyEx(RealRootKey,
			keypath,
			0,
			KEY_QUERY_VALUE | KEY_SET_VALUE,
			&Key) != ERROR_SUCCESS)
		{
			return FALSE;
		}
		dwDisp = RegDeleteValue(Key,
			wcKeyName);
		if (dwDisp == ERROR_FILE_NOT_FOUND)
		{
			result = TRUE;
		}
		else if (dwDisp != ERROR_SUCCESS)
		{
			goto CLEAN;
		}

		result = TRUE;

	CLEAN:
		if (Key)
			RegCloseKey(Key);
		return result;
	}


	static BOOL DeleteKey(__in HKEY hRootKey,
		__in LPCTSTR lpRunKey)
	{
 
		BOOL result = FALSE;
		HKEY RealRootKey = hRootKey;
		WCHAR keypath[REG_MAX_TREE_COUNT] = { 0 };
		const WCHAR *strsid = NULL;
		CUserPrivilegeUtil userprivutil;
		if (hRootKey == HKEY_CURRENT_USER)
		{
			if (userprivutil.IsSystemProcess())
			{
				strsid = userprivutil.GetCurrentUserSIDString();
				if (strsid)
				{
					RealRootKey = HKEY_USERS;
					swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s\\%s", strsid, lpRunKey);
				}
				else
					swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);
			}
			else
				swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);
		}
		else
			swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);
		if (RegDeleteKey(RealRootKey,
			keypath) != ERROR_SUCCESS)
		{
			ULONG err = GetLastError();
			if (err == ERROR_FILE_NOT_FOUND)
				result = TRUE;
			goto CLEAN;
		}
		result = TRUE;


	CLEAN:

		return result;
	}


	static BOOL Find(HKEY hRootKey, LPCTSTR lpRunKey, LPCTSTR KeyName, LPCTSTR findString)
	{
		HKEY Key;
		HKEY RealRootKey = hRootKey;
		WCHAR keypath[REG_MAX_TREE_COUNT] = { 0 };
		const WCHAR *strsid = NULL;
		CUserPrivilegeUtil userprivutil;
		if (KeyName == NULL)
			return FALSE;
		if (hRootKey == HKEY_CURRENT_USER)
		{
			if (userprivutil.IsSystemProcess())
			{
				strsid = userprivutil.GetCurrentUserSIDString();
				if (strsid)
				{
					RealRootKey = HKEY_USERS;
					swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s\\%s", strsid, lpRunKey);
				}
				else
					swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);
			}
			else
				swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);
		}
		else
			swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);

		if (RegOpenKeyEx(RealRootKey,
			keypath,
			0,
			KEY_READ,
			&Key) != ERROR_SUCCESS)
		{
			return FALSE;
		}

		DWORD dwIndex = 0;
		LONG lRet;
		DWORD cbName = IS_KEY_LEN;
		TCHAR szSubKeyName[IS_KEY_LEN];
		BOOL bFind = FALSE;

		while ((lRet = ::RegEnumKeyEx(Key, dwIndex, szSubKeyName, &cbName, NULL,
			NULL, NULL, NULL)) != ERROR_NO_MORE_ITEMS)
		{
			if (lRet == ERROR_SUCCESS)
			{
				HKEY hItem;
				if (::RegOpenKeyEx(Key, szSubKeyName, 0, KEY_READ, &hItem) != ERROR_SUCCESS)
					continue;

				TCHAR szDisplayName[IS_KEY_LEN] = { 0 };
				DWORD dwSize = sizeof(szDisplayName);
				DWORD dwType;

				if (::RegQueryValueEx(hItem, KeyName, NULL, &dwType,
					(LPBYTE)&szDisplayName, &dwSize) == ERROR_SUCCESS)
				{
					CString strName = szDisplayName;
					strName.MakeLower();
					if (strName.Find(findString) >= 0)
					{
						bFind = TRUE;
						::RegCloseKey(hItem);
						break;
					}
				}
				::RegCloseKey(hItem);
			}
			else
				break;
			dwIndex++;
			cbName = IS_KEY_LEN;
		}

		RegCloseKey(Key);
		return bFind;
	}

	template<typename Fn>
	static void Foreach_Reg(HKEY hRootKey, LPCTSTR lpRunKey, Fn func, bool is32=false)
	{
		HKEY Key;
		HKEY RealRootKey = hRootKey;
		WCHAR keypath[REG_MAX_TREE_COUNT] = { 0 };
		const WCHAR *strsid = NULL;
		CUserPrivilegeUtil userprivutil;
		if (lpRunKey == NULL)
			return;
		if (hRootKey == HKEY_CURRENT_USER)
		{
			if (userprivutil.IsSystemProcess())
			{
				strsid = userprivutil.GetCurrentUserSIDString();
				if (strsid)
				{
					RealRootKey = HKEY_USERS;
					swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s\\%s", strsid, lpRunKey);
				}
				else
					swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);
			}
			else
				swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);
		}
		else
			swprintf_s(keypath, REG_MAX_TREE_COUNT, L"%s", lpRunKey);

		if (RegOpenKeyEx(RealRootKey,
			keypath,
			0,
			(is32?KEY_READ| KEY_WOW64_32KEY: KEY_READ),
			&Key) != ERROR_SUCCESS)
		{
			return;
		}

		DWORD dwIndex = 0;
		LONG lRet;
		DWORD cbName = IS_KEY_LEN;
		TCHAR szSubKeyName[IS_KEY_LEN] = { 0 };
		TCHAR szValue[IS_KEY_LEN] = { 0 };
		DWORD cbValue = IS_KEY_LEN;

		while ((lRet = ::RegEnumValue(Key, dwIndex, szSubKeyName, &cbName, NULL,
			NULL, (LPBYTE)szValue, &cbValue)) != ERROR_NO_MORE_ITEMS)
		{
			if (lRet == ERROR_SUCCESS)
			{
				func(szSubKeyName, szValue);
			}
			dwIndex++;
			cbName = IS_KEY_LEN;
			cbValue = IS_KEY_LEN;
		}

		RegCloseKey(Key);
	}
};

class RegInterface
{
public:
	virtual BOOL SetKeyValue(__in HKEY hRootKey,
		__in LPCTSTR lpRunKey,
		__in const WCHAR *wcKeyName,
		__in PVOID keyvalue,
		__in ULONG keyvaluelength,
		__in_opt ULONG type = REG_DWORD,
		__in_opt BOOL bcreatekey = FALSE) = 0;

	virtual bool GetKeyValueDword(__in HKEY hRootKey,
		__in LPCTSTR lpRunKey,
		__in LPCTSTR wcKeyName,
		__out DWORD &retValue,
		__in BOOL isKey64 = FALSE) = 0;

	virtual BOOL DeleteKeyValue(__in HKEY hRootKey,
		__in LPCTSTR lpRunKey,
		__in const WCHAR *wcKeyName) = 0;
};

class CRegAgent : public RegInterface
{
public:
	CRegAgent() {}

	BOOL SetKeyValue(__in HKEY hRootKey,
		__in LPCTSTR lpRunKey,
		__in const WCHAR *wcKeyName,
		__in PVOID keyvalue,
		__in ULONG keyvaluelength,
		__in_opt ULONG type = REG_DWORD,
		__in_opt BOOL bcreatekey = FALSE)
	{
		return CRegUtil::SetKeyValue(hRootKey, lpRunKey, wcKeyName, keyvalue, keyvaluelength, type, bcreatekey);
	}

	bool GetKeyValueDword(__in HKEY hRootKey,
		__in LPCTSTR lpRunKey,
		__in LPCTSTR wcKeyName,
		__out DWORD &retValue,
		__in BOOL isKey64 = FALSE)
	{
		return CRegUtil::GetKeyValueDword(hRootKey, lpRunKey, wcKeyName, retValue, isKey64);
	}

	BOOL DeleteKeyValue(__in HKEY hRootKey,
		__in LPCTSTR lpRunKey,
		__in const WCHAR *wcKeyName)
	{
		return CRegUtil::DeleteKeyValue(hRootKey, lpRunKey, wcKeyName);
	}
};


class CRegInstance
{
public:
	static CRegInstance *Get(RegInterface* regInter = nullptr) {
		static CRegInstance kinstance(regInter);
		return &kinstance;
	}

	BOOL SetKeyValue(__in HKEY hRootKey,
		__in LPCTSTR lpRunKey,
		__in const WCHAR *wcKeyName,
		__in PVOID keyvalue,
		__in ULONG keyvaluelength,
		__in_opt ULONG type = REG_DWORD,
		__in_opt BOOL bcreatekey = FALSE)
	{
		return m_regInterface->SetKeyValue(hRootKey, lpRunKey, wcKeyName, keyvalue, keyvaluelength, type, bcreatekey);
	}

	bool GetKeyValueDword(__in HKEY hRootKey,
		__in LPCTSTR lpRunKey,
		__in LPCTSTR wcKeyName,
		__out DWORD &retValue,
		__in BOOL isKey64 = FALSE)
	{
		return m_regInterface->GetKeyValueDword(hRootKey, lpRunKey, wcKeyName, retValue, isKey64);
	}

	BOOL DeleteKeyValue(__in HKEY hRootKey,
		__in LPCTSTR lpRunKey,
		__in const WCHAR *wcKeyName)
	{
		return m_regInterface->DeleteKeyValue(hRootKey, lpRunKey, wcKeyName);
	}

private:
	CRegInstance(RegInterface * regInter)
	{
		if (regInter == nullptr)
			m_regInterface.reset(new CRegAgent());
		else
			m_regInterface.reset(regInter);
	}

	std::shared_ptr<RegInterface> m_regInterface;
};
