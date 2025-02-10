// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <Windows.h>
#include <string>
#include <map>
#include <functional>
#include "FileInfo.h"
#include <strsafe.h>

using namespace std;
class LibraryCaller
{
public:
	LibraryCaller(LibraryCaller &&other)
	{
		_filename = other._filename;
		_hMod = other._hMod;
		_map = other._map;
		other._hMod = nullptr;
	}
	LibraryCaller()
	{
	}
	LibraryCaller(const std::wstring &filename, bool is_resolute_path = false) :
		_filename(filename),
		_is_resolute_path(is_resolute_path)
	{
		Load();
	}

	~LibraryCaller()
	{
		UnLoad();
	}

	bool Load()
	{
		WCHAR file_path[MAX_PATH] = { 0 };
		WCHAR dll_path[MAX_PATH] = { 0 };
		wstring file;
		if (_is_resolute_path == false)
		{
			{
				CFileInfo().GetSameModuleFilePath(const_cast<WCHAR *>(_filename.c_str()), file_path);
				CFileInfo().SplitFileandPath(file_path, dll_path, NULL);
				AddDllDirectory(dll_path);
			}
			_hMod = LoadLibraryExW(file_path, NULL, LOAD_LIBRARY_SEARCH_SYSTEM32 | LOAD_LIBRARY_SEARCH_USER_DIRS);
		}
		else
		{
			_hMod = LoadLibraryExW(_filename.c_str(), NULL, LOAD_LIBRARY_SEARCH_SYSTEM32 | LOAD_LIBRARY_SEARCH_USER_DIRS);
		}

		if (nullptr == _hMod)
		{
			return false;
		}

		return true;
	}

	template <typename T, typename... Args>
	auto ExcecuteFunc(std::string funcName, Args &&... args)
	{
		auto f = GetFunction<T>(funcName);
		if (f == nullptr)
		{
			std::string s = "can not find this function " + funcName;
			throw std::exception(s.c_str());
		}

		return f(std::forward<Args>(args)...);
	}
	bool isLoaded()
	{
		return _hMod != nullptr;
	}
private:
	bool UnLoad()
	{
		if (_hMod == nullptr)
		{
			return true;
		}

		auto b = FreeLibrary(_hMod);
		if (!b)
		{
			return false;
		}

		_hMod = nullptr;
		return true;
	}

	template <typename T>
	T *GetFunction(const std::string &funcName)
	{
		if (_map.find(funcName) != _map.end())
		{
			return (T *)(_map[funcName]);
		}

		auto addr = GetProcAddress(_hMod, funcName.c_str());
		if (addr != nullptr)
		{
			_map[funcName] = addr;
		}

		return (T *)(addr);
	}

private:
	HMODULE _hMod = nullptr;
	std::wstring _filename;
	std::map<std::string, FARPROC> _map;
	bool _is_resolute_path = false;
};

