#pragma once

#include "../ParseDef.h"
#include "json/json.h"

#include <string>
#include <tuple>
#include <vector>
#include <set>

/** 
* Json数据组合类
*/
class CJsonBuilder
{
public:
	/** Image数据转成Json格式
	*/
	static std::string ImageDataToJson(const ImageData& data);
	static std::string BuildCancelGenVideo();
	static std::string BuildCancelGenVideo(const std::string& strRecordTime);
	static std::string BuildLangDL(int type);
	static std::string BuildStyleDL(int type);
};

