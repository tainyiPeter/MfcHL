// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include "define/GZDefine.h"
#include "sub_path.h"
#include "Registry.h"

class PluginDataCopy
{
public:

	static sub_path system_addin_data_folder()
	{
		return sub_path((sub_path::common_appdata() / L"Lenovo\\Vantage\\AddinData\\LenovoGamingSystemAddin"));
	}

	static sub_path system_plugin_data_folder()
	{
		return sub_path((sub_path::common_appdata() / L"Lenovo\\ImController\\SystemPluginData\\LenovoGamingSystemPlugin"));
	}

	static sub_path user_addin_data_folder()
	{
		return sub_path((sub_path::current_appdata() / L"Lenovo\\Vantage\\AddinData\\LenovoGamingUserAddin"));
	}

	static sub_path user_plugin_data_folder()
	{
		return sub_path((sub_path::current_appdata() / L"Lenovo\\ImController\\PluginData\\LenovoGamingUserPlugin"));
	}

	static sub_path addin_data_folder_append(sub_path folder)
	{
		return sub_path((sub_path::common_appdata() / L"Lenovo\\Vantage\\AddinData\\LenovoGamingSystemAddin" / folder));
	}

	static sub_path plugin_data_folder_append(sub_path folder)
	{
		return sub_path((sub_path::common_appdata() / L"Lenovo\\ImController\\SystemPluginData\\LenovoGamingSystemPlugin" / folder));
	}

	static bool check_addin_plugin_datafolder(sub_path folder)
	{
		if (!folder.is_folder())
		{
			folder = folder.parent_path();
		}
		const sub_path addin_folder = addin_data_folder_append(folder);
		const sub_path plugin_folder = plugin_data_folder_append(folder);
		if (!addin_folder.exists() && !plugin_folder.exists())
		{
			return true;
		}
		return false;
	}

	static wstring copy_profile(sub_path file_folder)
	{
		sub_path folder_addin = addin_data_folder_append(file_folder);
		if (folder_addin.exists())
		{
			return folder_addin;
		}

		WCHAR filefullpath[MAX_PATH] = { 0 };
		CFileInfo().GetSameModuleFilePath((WCHAR *)file_folder.c_str(), filefullpath);
		sub_path addin_bin_file(filefullpath);
		folder_addin.directory().copy_from(addin_bin_file.directory());

		return folder_addin;
	}

	static void init_registry()
	{
		//
		// Copy reg data from ImController for compatibility.
		//
		registry::registry_key plugin_key(HKEY_LOCAL_MACHINE, "SOFTWARE\\Lenovo\\ImController\\PluginData\\GamingPlugin", KEY_ALL_ACCESS);
		if (plugin_key.valid())
		{
			plugin_key.dispose();

			registry::copy_registry_tree(HKEY_LOCAL_MACHINE, "SOFTWARE\\Lenovo\\ImController\\PluginData\\GamingPlugin",
				"SOFTWARE\\Lenovo\\VantageService\\AddinData\\LenovoGamingAddin");

			registry::copy_registry_tree(HKEY_CURRENT_USER, "SOFTWARE\\Lenovo\\ImController\\PluginData\\GamingPlugin",
				"SOFTWARE\\Lenovo\\VantageService\\AddinData\\LenovoGamingAddin");
		}

		//
		// Set the default reg value if the copy operation fails.
		//

		//led
		registry::set_if_not_exsit(HKEY_LOCAL_MACHINE, GAMEZONE_DATA_LedProfile_REG_PATH, GAMEZONE_DATA_LedProfile_REG_VALUE_NAME, 0);
		//locker
		registry::set_if_not_exsit(HKEY_CURRENT_USER, GAMEZONE_DATA_AUTO_ENABLE_KEYLOCK_SETTINGS_REG_PATH, GAMEZONE_DATA_AUTO_ENABLE_KEYLOCK_REG_VALUE_NAME, 0);
		registry::set_if_not_exsit(HKEY_LOCAL_MACHINE, GAMEZONE_EVENT_TouchpadStatusEvent_REG_PATH_NAME, GAMEZONE_EVENT_TouchpadStatusEvent_REG_VALUE_NAME, 0);
		registry::set_if_not_exsit(HKEY_LOCAL_MACHINE, GAMEZONE_EVENT_WinKeyStatusEvent_REG_PATH_NAME, GAMEZONE_EVENT_WinKeyStatusEvent_REG_VALUE_NAME, 0);
		registry::set_if_not_exsit(HKEY_LOCAL_MACHINE, GAMEZONE_DATA_KEYlockFlag_REG_PATH_NAME, GAMEZONE_DATA_KEYlockFlag_REG_VALUE_NAME, 0);
		registry::set_if_not_exsit(HKEY_LOCAL_MACHINE, GAMEZONE_DATA_TouchpadlockFlag_REG_PATH_NAME, GAMEZONE_DATA_TouchpadlockFlag_REG_VALUE_NAME, 0);
		registry::set_if_not_exsit(HKEY_LOCAL_MACHINE, GAMEZONE_DATA_TouchpadlockFlag_REG_PATH_NAME, GAMEZONE_DATA_WinKeylockFlag_REG_VALUE_NAME, 0);
		//fan
		registry::set_if_not_exsit(HKEY_LOCAL_MACHINE, GAMEZONE_EVENT_ThermalModeReason_REG_PATH_NAME, GAMEZONE_EVENT_Caller_VALUE_NAME, 0);
		registry::set_if_not_exsit(HKEY_LOCAL_MACHINE, GAMEZONE_EVENT_SmartFanModuleEvent_REG_PATH_NAME, GAMEZONE_EVENT_SmartFanModule_VALUE_NAME, 0);
		registry::set_if_not_exsit(HKEY_LOCAL_MACHINE, GAMEZONE_EVENT_ThermalModeRealStatus_REG_PATH_NAME, GAMEZONE_EVENT_SmartFanModule_VALUE_NAME, 0);
		registry::set_if_not_exsit(HKEY_LOCAL_MACHINE, REG_SETTING_REG_PATH, REG_THERMALMODE_INTELL, REG_Setting_SwitchOff);
		//gamedetect
		registry::set_if_not_exsit(HKEY_LOCAL_MACHINE, GAMEZONE_EVENT_GAMEDETECT_EVENT_REG_PATH_NAME, GAMEZONE_EVENT_GAMEDETECT_EVENT_REG_VALUE_NAME, 0);
		registry::set_if_not_exsit(HKEY_CURRENT_USER, REG_SETTING_REG_PATH, REG_SETDLG_AUTOGAMEMODE_VALUE, 1);
		//oc
		registry::set_if_not_exsit(HKEY_LOCAL_MACHINE, GAMEZONE_EVENT_OCModeChangeEvent_REG_PATH_NAME, GAMEZONE_EVENT_OCModeChangeEvent_REG_VALUE_NAME, 0);
		registry::set_if_not_exsit(HKEY_LOCAL_MACHINE, GAMEZONE_EVENT_OCOnOffReasonEvent_REG_PATH_NAME, GAMEZONE_EVENT_OCOnOffReasonEvent_REG_VALUE_NAME, 0);
		registry::set_if_not_exsit(HKEY_LOCAL_MACHINE, GAMEZONE_DATA_AUTO_ENABLE_OC_SETTINGS_REG_PATH, GAMEZONE_DATA_AUTO_ENABLE_OC_REG_VALUE_NAME, 0);
		registry::set_if_not_exsit(HKEY_LOCAL_MACHINE, GAMEZONE_DATA_OVERCLOCK_PATH, GAMEZONE_DATA_OVERCLOCK_VALUE_NAME, 0);
		registry::set_if_not_exsit(HKEY_LOCAL_MACHINE, GAMEZONE_DATA_OVERCLOCK_SETTING, GAMEZONE_DATA_OVERCLOCK_CpuOverclockSetting, 0);
		registry::set_if_not_exsit(HKEY_LOCAL_MACHINE, GAMEZONE_DATA_OVERCLOCK_SETTING, GAMEZONE_DATA_OVERCLOCK_GpuOverclockSetting, 0);
		//macrokey
		registry::set_if_not_exsit(HKEY_LOCAL_MACHINE, MacroKeyPluginReg, MacroKeyType, 0);
		registry::set_if_not_exsit(HKEY_CURRENT_USER, REG_SETTING_REG_PATH, GAMEZONE_MACROKEYSWITCH_VALUE, REG_Setting_SwitchOff);
		registry::set_if_not_exsit(HKEY_CURRENT_USER, REG_SETTING_REG_PATH, GAMEZONE_MACROKEYDATA_VALUE, 1);
		//common
		registry::set_if_not_exsit(HKEY_LOCAL_MACHINE, GAMEZONE_EVENT_ACDCModeChangeEvent_REG_PATH_NAME, GAMEZONE_EVENT_ACDCModeChangeEvent_REG_VALUE_NAME, 0);
		//Optimization
		registry::set_if_not_exsit(HKEY_CURRENT_USER, REG_SETTING_REG_PATH, GAMEZONE_OPTIMIZATION_REG, REG_Setting_SwitchOff);
		//networkboost
		registry::set_if_not_exsit(HKEY_CURRENT_USER, REG_SETTING_REG_PATH, GAMEZONE_NETWORKBOOST_REG, REG_Setting_SwitchOff);

		registry::set_if_not_exsit(HKEY_LOCAL_MACHINE, REG_SETTING_REG_PATH, L"", L"");

		registry::set_if_not_exsit(HKEY_LOCAL_MACHINE, REG_LOG_PATH_W, L"", L"");
	}

	static void init_system_folder()
	{
		const sub_path system_addin_folder = system_addin_data_folder();
		const sub_path system_plugin_folder = system_plugin_data_folder();

		if (!system_addin_folder.exists())
		{
			if (system_plugin_folder.exists())
			{
				system_addin_folder.copy_from(system_plugin_folder);
			}
		}
	}

	static void init_user_folder()
	{
		DBG_PRINTF_FL((L"[PluginDataCopy]: Init user folder.\r\n"));

		const sub_path user_addin_folder = user_addin_data_folder();
		const sub_path user_plugin_folder = user_plugin_data_folder();
		if (!user_addin_folder.exists())
		{
			DBG_PRINTF_FL((L"[PluginDataCopy]: User addin folder not exists.\r\n"));

			if (user_plugin_folder.exists())
			{
				DBG_PRINTF_FL((L"[PluginDataCopy]: User plugin folder exists, copy file to user addin folder.\r\n"));

				int nRet = user_addin_folder.copy_from(user_plugin_folder);
				if (0 != nRet)
				{
					DBG_PRINTF_FL((L"[PluginDataCopy]: Failed to copy user files from plugin to addin folder, ret code: %d.\r\n", nRet));
				}
				else
				{
					DBG_PRINTF_FL((L"[PluginDataCopy]: Suc to copy user files from plugin to addin folder.\r\n"));
				}
			}
			else
			{
				DBG_PRINTF_FL((L"[PluginDataCopy]: User plugin folder not exists.\r\n"));
			}
		}
		else
		{
			DBG_PRINTF_FL((L"[PluginDataCopy]: User addin folder already exists, needn't copy.\r\n"));
		}

		DBG_PRINTF_FL((L"[PluginDataCopy]: End init user folder.\r\n"));
	}
};