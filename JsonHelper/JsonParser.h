#pragma once

#include "json/json.h"
#include "../GuiInterface.h"
#include "../ParseDef.h"

#include <unordered_map>
#include <string>
#include <vector>

/** Json格式解析类
*/
class JsonParser
{
public:
	JsonParser();
	~JsonParser();

	static bool LoadJson(const std::wstring& strPath, ActionMap& mapJson);

	static bool ParseGenVideoList(const std::string& strJson, VideoGenInfoVec& vecData);
};