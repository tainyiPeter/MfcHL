// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include "RegUtil.h"

class VersionChecker
{
public:
	static bool IsVantageInstalled()
	{
		return IsVantageExist() || IsVantageBetaExist();
	}

	static bool IsVantageExist()
	{
		HKEY hkey = NULL;
		if (CRegUtil().OpenKey(HKEY_CURRENT_USER,
				L"SOFTWARE\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\CurrentVersion\\AppContainer\\Storage\\e046963f.lenovocompanion_k1h2ywk1493x8",
				hkey))
		{
			if (hkey != NULL)
			{
				RegCloseKey(hkey);
				return true;
			}
		}
		return false;
	}
	static bool IsVantageBetaExist()
	{
		HKEY hkey = NULL;
		if (CRegUtil().OpenKey(HKEY_CURRENT_USER,
				L"SOFTWARE\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\CurrentVersion\\AppContainer\\Storage\\e046963f.lenovocompanionbeta_k1h2ywk1493x8",
				hkey))
		{
			if (hkey != NULL)
			{
				RegCloseKey(hkey);
				return true;
			}
		}
		return false;
	}

	static void SetInstallValue(bool isInstalled)
	{
		m_isInstalled = isInstalled;
	}
	static bool GetInstallValue()
	{
		return m_isInstalled;
	}
	static void InitInstallValue()
	{
		m_isInstalled = IsVantageInstalled();
	}

protected:
	static bool m_isInstalled;
};
