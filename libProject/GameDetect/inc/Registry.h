// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <Windows.h>

#include "UserPrivilegeUtil.h"
#include "StringUtility.h"

using std::string;
using std::pair;
using std::vector;
using std::cout;
using std::endl;

namespace registry
{
	constexpr int  max_reg_path = 512;
	constexpr int  max_key_len = 1024;
	constexpr int  reg_max_data_count = 4096;
	class registry_key final
	{
	public:
		explicit registry_key(HKEY root, string path,const unsigned long access_right = KEY_READ, bool create_key = false);
		registry_key():h_key_(nullptr),root_key_(nullptr)
		{
		}
		registry_key(const registry_key &other) = delete;
		registry_key(registry_key &&o) noexcept;
		registry_key &operator=(const registry_key &o) = delete;
		registry_key &operator=(registry_key &&o) noexcept;

		bool valid() const;
		~registry_key()
		{
			dispose();
		}

		void dispose();

		registry_key open_sub_key(const string &path, unsigned long access_right = KEY_READ) const;
		vector<string> get_value_names() const;
		vector<string> get_sub_key_names() const;
		int set_key(const string &key_name) const;

		static unsigned long to_type(const string &type_name)
		{
			if (type_name == typeid(string).name())
			{
				return REG_SZ;
			}
			if (type_name == typeid(vector<char>).name()) {
				return REG_BINARY;
			}
			if (type_name == typeid(DWORD).name())
			{
				return REG_DWORD;
			}
			return REG_SZ;
		}

		static void copy_value(string& t, const BYTE* key_value, const unsigned long length) {
			t = (char*)(key_value);
		}
		static void copy_value(vector<unsigned char> &t, const BYTE*  key_value, const unsigned long  key_value_length) {
			for (unsigned long i = 0; i < key_value_length; i++) {
				t.push_back(key_value[i]);
			}
		}
		static void copy_value(unsigned long &t, const BYTE*  key_value, const unsigned long length) {
			t = *reinterpret_cast<const unsigned long*>(key_value);
		}

		static void copy_value(size_t &t, const BYTE*  key_value, const unsigned long length) {
			t = *reinterpret_cast<const size_t *>(key_value);
		}

		template<typename T>
		long get_value(const string &value_name, T &t) {
			unsigned long type = 0;
			BYTE key_value[reg_max_data_count] = { 0 };
			unsigned long value_length = reg_max_data_count;
			const DWORD ret_error = RegQueryValueExA(h_key_,
			                                         value_name.c_str(),
			                                         nullptr,
			                                         &type,
			                                         static_cast<LPBYTE>(key_value),
			                                         &value_length);
			if (ret_error == ERROR_SUCCESS)
				registry_key::copy_value(t, key_value, value_length);

			return ret_error;
		}

		template<typename T>
		long get_value(const std::wstring &value_name, T &t) {
			return get_value<T>(string(value_name.begin(), value_name.end()), t);
		}

		long get_value(const string &value_name, unsigned long &type, BYTE *key_value, unsigned long value_length) {

			return RegQueryValueExA(h_key_,
				value_name.c_str(),
				nullptr,
				&type,
				key_value,
				&value_length);
		}

		template<typename T>
		long set_value(const string &value_name, const T &t) {
			unsigned long length = 0;
			unsigned char* p_value = (unsigned char*)(get_data_ptr(t, length));
			const unsigned long type = to_type(typeid(T).name());
			return RegSetValueExA(h_key_,
				value_name.c_str(),
				NULL,
				type,
				static_cast<LPBYTE>(p_value),
				length);
		}

		template<typename T>
		long set_value(const std::wstring &value_name, const T &t) {
			return set_value<T>(string(value_name.begin(), value_name.end()), t);
		}	

		auto set_key(HKEY h_key)
		{
			h_key_ = h_key;
		}

		HKEY get_key() const {
			return h_key_;
		}

		string get_path() const
		{
			return path_;
		}

		inline LSTATUS QueryValue(
			_In_opt_z_ LPCTSTR pszValueName,
			_Out_opt_ DWORD* pdwType,
			_Out_opt_ void* pData,
			_Inout_ ULONG* pnBytes) throw()
		{
			return(::RegQueryValueEx(h_key_, pszValueName, NULL, pdwType, static_cast<LPBYTE>(pData), pnBytes));
		}

		long create_key()
		{
			if(root_key_ == nullptr)
			{
				deal_path_with_root_path();
			}
			if (h_key_ == nullptr && root_key_ != nullptr && path_.length() > 0)
			{
				return RegCreateKeyExA(root_key_,
					path_.c_str(),
					0,
					nullptr,
					REG_OPTION_NON_VOLATILE,
					KEY_QUERY_VALUE | KEY_SET_VALUE | KEY_CREATE_SUB_KEY,
					nullptr,
					&h_key_,
					nullptr);
			}
			//debug log;

			return ERROR_ALREADY_EXISTS;
		}

		bool del_key_value(const string &value_name) const
		{
			if (h_key_ != nullptr)
			{
				const unsigned long ret = RegDeleteValueA(h_key_, value_name.c_str());
				if (ret == ERROR_FILE_NOT_FOUND || ret == ERROR_SUCCESS)
					return true;
			}
			return false;
		}

		bool del_key() const
		{
			if (h_key_ != nullptr)
			{
				const unsigned long ret = RegDeleteKeyA(root_key_, path_.c_str());
				if (ret == ERROR_FILE_NOT_FOUND || ret == ERROR_SUCCESS)
					return true;
			}
			return false;
		}

		static bool del_key(HKEY root, const string &key_name)
		{
			registry_key reg(root, key_name, KEY_READ | KEY_WRITE);
			//doesn't exist
			if (!reg.valid())
				return true;
			return reg.del_key();
		}

		static bool recurse_delete_key(HKEY root, const string &key_name)
		{
			registry_key reg(root, key_name, KEY_READ | KEY_WRITE);
			//doesn't exist
			if (!reg.valid())
				return true;

			bool bRet = false;
			FILETIME time;
			DWORD dwSize = 256;
			char szBuffer[256] = {0};
			while (RegEnumKeyExA(reg.get_key(), 0, szBuffer, &dwSize, NULL, NULL, NULL,
				&time) == ERROR_SUCCESS)
			{
				string subkey = reg.get_path()+"\\"+ szBuffer;
				bRet = recurse_delete_key(root, subkey);
				if (!bRet)
					return bRet;
				dwSize = 256;
			}

			return reg.del_key();
		}

		
		template<typename fn>
		bool start_monitor(fn f)
		{
 			const HANDLE h_notify = CreateEvent(NULL,
			                                   FALSE,
			                                   TRUE,
			                                   NULL);

			bool ret = false;
			if (RegNotifyChangeKeyValue(h_key_,
				TRUE,
				REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_LAST_SET,
				h_notify,
				TRUE
			) == ERROR_SUCCESS)
			{
				if (WaitForSingleObject(h_notify, INFINITE) != WAIT_FAILED)
				{
					//RegCloseKey will trigger the event and return false
					if (h_key_ != nullptr)
					{
						f();
						ret = true;
					}
				}
			}

			CloseHandle(h_notify);
			return ret;
		}

		template<typename fn>
		bool start_monitor(fn f, ULONG intervaltime)
		{
			const HANDLE h_notify = CreateEvent(NULL,
				FALSE,
				TRUE,
				NULL);

			bool ret = false;
			if (RegNotifyChangeKeyValue(h_key_,
				TRUE,
				REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_LAST_SET,
				h_notify,
				TRUE
			) == ERROR_SUCCESS)
			{
				if (WaitForSingleObject(h_notify, intervaltime) != WAIT_FAILED)
				{
					//RegCloseKey will trigger the event and return false
					if (h_key_ != nullptr)
					{
						f();
						ret = true;
					}
				}
			}

			CloseHandle(h_notify);
			return ret;
		}

	private:

		std::string add_user_sid_to_path(const string &path)
		{
			char keypath[max_reg_path] = { 0 };
			if (root_key_ == HKEY_CURRENT_USER)
			{
				if (CUserPrivilegeUtil userprivutil; userprivutil.IsSystemProcess())
				{
					const WCHAR *pCurUserSID = userprivutil.GetCurrentUserSIDString();
					if (nullptr == pCurUserSID)
					{
						_ASSERT(FALSE);
					}

					const std::wstring sid = (pCurUserSID == nullptr ? L"" : pCurUserSID);

					if (!sid.empty())
					{
						root_key_ = HKEY_USERS;
						sprintf_s(keypath, max_reg_path, "%S\\%s", sid.c_str(), path.c_str());
					}
					else
						sprintf_s(keypath, max_reg_path, "%s", path.c_str());
				}
				else
					sprintf_s(keypath, max_reg_path, "%s", path.c_str());
			}
			else
			{
				sprintf_s(keypath, max_reg_path, "%s", path.c_str());
			}

			return keypath;
		}

		void deal_path_with_root_path()
		{
			const string local_machine_root_path = "HKEY_LOCAL_MACHINE\\";
			const string current_user_root_path = "HKEY_CURRENT_USER\\";

			if (path_.find(local_machine_root_path) != string::npos)
			{
				root_key_ = HKEY_LOCAL_MACHINE;
				path_ = path_.substr(local_machine_root_path.length(), path_.length() - local_machine_root_path.length());
			}
			if (path_.find(current_user_root_path) != string::npos)
			{
				root_key_ = HKEY_CURRENT_USER;
				path_ = path_.substr(current_user_root_path.length(), path_.length() - current_user_root_path.length());
			}
		}
		static const void *get_data_ptr(const DWORD &t, DWORD &length)
		{
			length = sizeof(DWORD);
			return &t;
		}

		static const void *get_data_ptr(const string &t, DWORD &length)
		{
			length = static_cast<unsigned long>(t.size());
			return (t.c_str());
		}

		static const void *get_data_ptr(const vector<char> &t, DWORD &length)
		{
			length = static_cast<unsigned long>(t.size());
			return t.data();
		}
	
		HKEY h_key_;
		HKEY root_key_;
		string path_;
	};

	registry_key registry_current_user(const string &path,const unsigned long access_right);
	registry_key registry_local_machine(const string &path,const unsigned long access_right);
	registry_key registry_current_user(const std::wstring &path, const unsigned long access_right);
	registry_key registry_local_machine(const std::wstring &path, const unsigned long access_right);

	void set_if_not_exsit(HKEY root, wstring path, wstring value_name, wstring value);
	void set_if_not_exsit(HKEY root, wstring path, wstring value_name, DWORD value);
	void copy_registry_tree(HKEY root, string source, string destination);
}
