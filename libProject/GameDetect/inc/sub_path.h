// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <Windows.h>
#include <filesystem>
#include <shlobj.h>
#include "inc/DebugUtil.h"

class sub_path : public std::filesystem::path
{
public:
	sub_path() : std::filesystem::path() {}
	explicit sub_path(std::wstring & source) : std::filesystem::path(source) {}
	explicit sub_path( std::wstring && source ) : std::filesystem::path(source) {}
	explicit sub_path( const wchar_t * source ) : std::filesystem::path(source) {}

	sub_path& operator=(path&& other) {
		std::filesystem::path::operator=(std::move(other));
		return *this;
	}

	static sub_path windows_dir()
	{
		wchar_t sz_path[MAX_PATH + 1] = { 0 };

		::GetWindowsDirectory(sz_path, MAX_PATH + 1);

		return sub_path(sz_path);
	}

	static sub_path system_dir()
	{
		wchar_t sz_path[MAX_PATH + 1] = { 0 };

		::GetSystemDirectory(sz_path, MAX_PATH + 1);

		return sub_path(sz_path);
	}

	static sub_path common_appdata()
	{
		return special_dir(CSIDL_COMMON_APPDATA);
	}

	static sub_path current_appdata()
	{
		return special_dir(CSIDL_LOCAL_APPDATA);
	}

	static sub_path temp_dir()
	{
		wchar_t sz_path[MAX_PATH + 1] = { 0 };

		::GetTempPath(MAX_PATH + 1, sz_path);

		return sub_path(sz_path);
	}

	static sub_path special_dir(const int csidl)
	{
		wchar_t buffer[MAX_PATH + 1] = { 0 };

		if (::SHGetSpecialFolderPath(nullptr, buffer, csidl, FALSE))
			return sub_path(buffer);

		return sub_path(L"");
	}

	[[nodiscard]] bool exists() const
	{
		return std::filesystem::exists(*this);
	}

	[[nodiscard]] bool is_folder() const
	{
		//std::filesystem::is_directory(*this) will check the exist of the folder
		return extension().empty();
	}

	int copy_from(const sub_path& other) const
	{
		if (!other.exists())
		{
			return -2;
		}

		bool bRet = create_directories();
		if (!bRet)
		{
			return -3;
		}

		error_code ec;
		std::filesystem::copy(other, *this,
			std::filesystem::copy_options::recursive, ec);

		return 0;
	}

	bool create_directories() const
	{
		sub_path path = *this;
		if (!path.is_folder())
			path = path.parent_path();

		if (!exists())
		{
			std::error_code nError;
			std::filesystem::create_directories(path, nError);
			DBG_PRINTF_FL((L"[sub_path]: create_directories error code: %d.\r\n", nError.value()));

			if (!exists())
			{
				DBG_PRINTF_FL((L"[sub_path]: failed call create_directories, call SHCreateDirectoryEx.\r\n"));

				int nRet = SHCreateDirectoryEx(nullptr, path.c_str(), nullptr);
				if (ERROR_SUCCESS != nRet)
				{
					DBG_PRINTF_FL((L"[sub_path]: SHCreateDirectoryEx error code: %d.\r\n", nRet));
				}
			}

			bool bRet = exists();
			DBG_PRINTF_FL((L"[sub_path]: create_directories return %s.\r\n", bRet ? L"true" : L"false"));

			return bRet;
		}
		else
		{
			return true;
		}
	}

	sub_path directory() const
	{
		sub_path path = *this;
		if (!path.is_folder())
			path = path.parent_path();
		return path;
	}
};