// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary
#pragma once
#include <windows.h>
#include <vector>
#include <filesystem>
#include "ServiceHelper.h"
class file_version
{
public:
	file_version(WORD h_ms, WORD h_ls, WORD l_ms, WORD l_ls) :
		h_ms_(h_ms),
		h_ls_(h_ls),
		l_ms_(l_ms),
		l_ls_(l_ls)
	{
	}
	inline bool operator < (const file_version& other) const
	{
		if (h_ms_ > other.h_ms_)
		{
			return false;
		}
		if (h_ls_ > other.h_ls_)
		{
			return false;
		}
		if (l_ms_ > other.l_ms_)
		{
			return false;
		}
		if (l_ls_ > other.l_ls_)
		{
			return false;
		}
		if (*this == other)
		{
			return false;
		}
		return true;
	}
	inline bool operator == (const file_version& other) const
	{
		if (h_ms_ == other.h_ms_ &&
			l_ms_ == other.l_ms_ &&
			h_ls_ == other.h_ls_ &&
			l_ls_ == other.l_ls_)
		{
			return true;
		}
		return false;

	}
	inline bool operator > (const file_version& other) const
	{
		if (h_ms_ > other.h_ms_)
		{
			return true;
		}
		if (h_ms_ < other.h_ms_)
		{
			return false;
		}
		if (h_ls_ > other.h_ls_)
		{
			return true;
		}
		if (h_ls_ < other.h_ls_)
		{
			return false;
		}
		if (l_ms_ > other.l_ms_)
		{
			return true;
		}
		if (l_ms_ < other.l_ms_)
		{
			return false;
		}
		if (l_ls_ > other.l_ls_)
		{
			return true;
		}
		return false;
	}
	WORD h_ms_;
	WORD l_ms_;
	WORD h_ls_;
	WORD l_ls_;
};
class xtu_wrapper_helper
{
public:
	static file_version GetExeVersion(const char* exePath)
	{
		UINT sz = GetFileVersionInfoSizeA(exePath, NULL);
		if (sz)
		{
			std::vector<char> pBuf(sz + 1);
			if (GetFileVersionInfoA(exePath, NULL, sz, pBuf.data()))
			{
				VS_FIXEDFILEINFO* pVsInfo = NULL;
				if (VerQueryValueA(pBuf.data(), "\\", (void**)&pVsInfo, &sz))
				{
					return {
						HIWORD(pVsInfo->dwFileVersionMS),
						LOWORD(pVsInfo->dwFileVersionMS),
						HIWORD(pVsInfo->dwFileVersionLS),
						LOWORD(pVsInfo->dwFileVersionLS) };

				}
			}
		}
		return { 0,0,0,0 };
	}

static bool is_new_xtu_version()
{
	CServiceHelper helper;
	string service_path = "";
	if (helper.GetServiceInstallPath(XTUSERVICENAME, service_path))
	{
		file_version service_version = GetExeVersion(service_path.c_str());
		DBG_PRINTF_FL((L"xtu service version %d.%d.%d.%d", service_version.h_ms_,
			service_version.h_ls_,
			service_version.l_ms_,
			service_version.l_ls_));
		return service_version > file_version{ 7,9,0,23 };
	}
	DBG_PRINTF_FL((L"xtu service path %S false", service_path.c_str()));
	return false;
}

static string get_xtu_wrapper_dll_path_name()
{
	string file_path;
	CFileInfo().GetCurrentModuleFilePath(file_path);
	const std::filesystem::path dll_path = file_path;
	DBG_PRINTF_FL((L"xtu sdk path 1 %S", dll_path.c_str()));

	string sdk_dll_path = dll_path.parent_path().string();
	DBG_PRINTF_FL((L"xtu sdk path 2 %S", sdk_dll_path.c_str()));

	if (is_new_xtu_version())
	{
		sdk_dll_path += "\\7.9\\SdkWrapperForNativeCode.dll";
	}
	else
	{
		sdk_dll_path += "\\7.8\\SdkWrapperForNativeCode.dll";
	}
	return sdk_dll_path;
}
static HMODULE load_xtu_wrapper()
{
	string sdk_dll_path = xtu_wrapper_helper::get_xtu_wrapper_dll_path_name();
	DBG_PRINTF_FL((L"xtu sdk path %S", sdk_dll_path.c_str()));
	return LoadLibraryExA(sdk_dll_path.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);
}
};
