#pragma once

#include "json/json.h"
#include "../GuiInterface.h"
#include "../ParseDef.h"

#include <unordered_map>
#include <string>
#include <vector>

/** Json��ʽ������
*/
class JsonParser
{
public:
	JsonParser();
	~JsonParser();

	static bool LoadJson(const std::wstring& strPath, ActionMap& mapJson);

	static bool ParseGenVideoList(const std::string& strJson, VideoGenInfoVec& vecData);
};