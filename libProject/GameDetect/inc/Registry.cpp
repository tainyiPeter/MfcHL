// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#include "Registry.h"
#include "UserPrivilegeUtil.h"
#include <utility>

namespace registry
{

	registry_key::registry_key(HKEY root, string path, const unsigned long access_right, bool create_key):
		h_key_(nullptr),
		root_key_(root),
		path_(std::move(path))
	{
		if (root_key_ == nullptr)
		{
			deal_path_with_root_path();
		}

		path_ = add_user_sid_to_path(path_);

		if (const long error = RegOpenKeyExA(root_key_,
			path_.c_str(),
			0,
			access_right,
			&h_key_))
		{
			if(error != ERROR_SUCCESS)
			{
				if ( create_key )
				{
					RegCreateKeyExA(root_key_,
						path_.c_str(),
						0,
						NULL,
						REG_OPTION_NON_VOLATILE,
						KEY_QUERY_VALUE | KEY_SET_VALUE | KEY_CREATE_SUB_KEY,
						NULL,
						&h_key_,
						NULL);
				}
			}
		}
	}

	registry_key::registry_key(registry_key&& o) noexcept:
	h_key_(o.get_key()),
	root_key_(o.root_key_),
	path_(std::move(o.path_))
	{
		o.h_key_ = nullptr;
		o.root_key_ = nullptr;
		o.path_ = "";
	}

	registry_key& registry_key::operator=(registry_key&& o) noexcept
	{
		h_key_ = o.get_key();
		root_key_  = o.root_key_;
		path_ = std::move(o.path_);
		o.h_key_ = nullptr;
		o.root_key_ = nullptr;
		o.path_ = "";
		return *this;
	}

	bool registry_key::valid() const
	{
		return h_key_ != nullptr;
	}

	void registry_key::dispose()
	{
		if (h_key_)
		{
			RegCloseKey(h_key_);
			h_key_ = nullptr;
		}
	}

	registry_key registry_key::open_sub_key(const string &path, const unsigned long access_right) const 
	{
		HKEY h_key = nullptr;
		if (const long error = RegOpenKeyExA(h_key_,
		                                     path.c_str(),
		                                     0,
		                                     access_right,
		                                     &h_key))
		{
			if(error != ERROR_SUCCESS)
			{
				cout << "Open Key failed --" << error << "-----" << path << endl;
			}
		}
		registry::registry_key reg_key;
		reg_key.set_key(h_key);
		return reg_key;
	}

	vector<string> registry_key::get_value_names() const
	{
		DWORD index = 0;
		LONG ret;
		DWORD value_name_length = max_key_len;
		CHAR value_name[max_key_len];
		vector<string> value_names;
		while ((ret = ::RegEnumValueA(h_key_, 
			index, 
			value_name, 
			&value_name_length, 
			nullptr,
			nullptr, 
			nullptr, 
			nullptr)) != ERROR_NO_MORE_ITEMS 
			&& ret == ERROR_SUCCESS)
		{
			index++;
			value_name_length = max_key_len;
			value_names.emplace_back(value_name);
		}

		return value_names;
	}

	vector<string> registry_key::get_sub_key_names() const
	{
		DWORD index = 0;
		LONG ret;
		DWORD length = max_key_len;
		CHAR key_name[max_key_len];
		vector<string> key_names;
		while ((ret = ::RegEnumKeyExA(h_key_, 
			index, 
			key_name, 
			&length, 
			nullptr,
			nullptr, 
			nullptr, 
			nullptr)) != ERROR_NO_MORE_ITEMS 
			&& ret == ERROR_SUCCESS)
		{
			index++;
			length = max_key_len;
			key_names.emplace_back(key_name);
		}
		return key_names;
	}

	int registry_key::set_key(const string &key_name) const
	{
		HKEY ret_key;
		if (RegCreateKeyExA(h_key_,
			key_name.c_str(),
			0,
			nullptr,
			REG_OPTION_NON_VOLATILE,
			KEY_QUERY_VALUE | KEY_SET_VALUE | KEY_CREATE_SUB_KEY,
			nullptr,
			&ret_key,
			nullptr) != ERROR_SUCCESS)
		{
			return -1;//no need clean registry
		}
		return ERROR_SUCCESS;
	}

	registry_key registry_current_user(const string& path, const unsigned long access_right)
	{
		return registry_key(HKEY_CURRENT_USER, path, access_right);
	}
	registry_key registry_local_machine(const string& path, const unsigned long access_right)
	{
		return registry_key(HKEY_LOCAL_MACHINE, path, access_right);
	}
	registry_key registry_current_user(const std::wstring& path, const unsigned long access_right)
	{
		return registry_current_user(string(path.begin(), path.end()), access_right);
	}
	registry_key registry_local_machine(const std::wstring& path, const unsigned long access_right)
	{
		return registry_local_machine(string(path.begin(), path.end()), access_right);
	}

	void set_if_not_exsit(HKEY root, wstring path, wstring value_name, wstring value)
	{
		registry_key reg(root, string(path.begin(), path.end()), KEY_ALL_ACCESS);
		if (!reg.valid())
		{
			reg.create_key();
			reg.set_value<string>(value_name, string(value.begin(), value.end()));
		}
		else
		{
			string out;
			if (ERROR_SUCCESS != reg.get_value<string>(value_name, out))
				reg.set_value<string>(value_name, string(value.begin(), value.end()));
		}
	}

	void set_if_not_exsit(HKEY root, wstring path, wstring value_name, DWORD value)
	{
		registry_key reg(root, string(path.begin(), path.end()), KEY_ALL_ACCESS);
		if (!reg.valid())
		{
			reg.create_key();
			reg.set_value<DWORD>(value_name, value);
		}
		else
		{
			DWORD out;
			if (ERROR_SUCCESS != reg.get_value<DWORD>(value_name, out))
				reg.set_value<DWORD>(value_name, value);
		}
	}

	void copy_registry_tree(HKEY root, string source, string destination)
	{
		registry_key dest_reg(root, destination, KEY_ALL_ACCESS);
		if (!dest_reg.valid())
		{
			dest_reg.create_key();

			registry_key source_reg(root, string(source.begin(), source.end()), KEY_ALL_ACCESS);
			if (source_reg.valid())
			{
				vector<string> values = source_reg.get_value_names();
				for (auto item_value : values)
				{
					BYTE key_value[reg_max_data_count];
					unsigned long type = 0;
					source_reg.get_value(item_value, type, key_value, reg_max_data_count);
					
					if (type == REG_SZ)
					{
						dest_reg.set_value<string>(item_value, string((char*)key_value));
					}
					else if (type == REG_DWORD)
					{
						dest_reg.set_value<DWORD>(item_value, (*((DWORD*)key_value)));
					}
					else
						continue;
				}


				vector<string> sub_keys= source_reg.get_sub_key_names();
				for (auto item : sub_keys)
				{
					string dest_sub_key = destination + "\\" + item;
					string source_sub_key = source + "\\" + item;

					copy_registry_tree(root, std::move(source_sub_key), std::move(dest_sub_key));
				}
			}
		}
	}
}

