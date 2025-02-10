// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <map>
#include <fstream>
#include "ThirdParty/NLOHMANN_JSON/json.hpp"
using std::vector;
using std::string;
using json = nlohmann::json;
namespace fs_fs = std::filesystem;
namespace Protocol
{
	class Serializer
	{
	public:
		template<typename T>
		static string Serialize(const T &&in) {
			json j;
			to_json(j, in);
			return j.dump();
		}

		template<typename T>
		static string Serialize(const T &in) {
			json j;
			to_json(j, in);
			return j.dump();
		}

		template<typename T>
		static T DeSerialize(const string &ins) {
			try
			{
				return json::parse(ins);
			}
			catch (...)
			{

			}
			return T();
		}

		template<typename T>
		static string Serialize(fs_fs::path filename,const T &in) {
			json j;
			std::ofstream is(filename.string().c_str(),std::ofstream::out);
			if constexpr (requires(T){ to_json(j, in); })
			{
				to_json(j, in);
			}
			else
			{
				j = in;
			}
			std::string str = j.dump();
			is.write(str.c_str(), str.length());
			return str;
		}


		template<typename T>
		static T DeSerialize(const fs_fs::path &filename) {
			try
			{
				std::ifstream is{ filename.string(), std::ios::binary | std::ios::ate };
				auto size = is.tellg();
				std::string str(size, '\0'); // construct string to stream size
				is.seekg(0);
				if (is.read(&str[0], size))
					std::cout << str << '\n';
				return json::parse(str);
			}
			catch (...)
			{
			}
			return T();
		}

		template<>
		static std::map<std::string,std::string> DeSerialize(const fs_fs::path &filename) {
			try
			{
				std::ifstream is{ filename.string(), std::ios::binary | std::ios::ate };
				auto size = is.tellg();
				std::string str(size, '\0'); // construct string to stream size
				is.seekg(0);
				if (is.read(&str[0], size))
					std::cout << str << '\n';
				json j = json::parse(str);
				return j.get<std::map<std::string, std::string>>();
			}
			catch (...)
			{
			}
			return {};
		}
	};
}