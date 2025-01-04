#pragma once

#include "../ParseDef.h"
#include "json/json.h"

#include <string>
#include <tuple>
#include <vector>
#include <set>

/** 
* Json���������
*/
class CJsonBuilder
{
public:
	/** Image����ת��Json��ʽ
	*/
	static std::string ImageDataToJson(const ImageData& data);
	static std::string BuildCancelGenVideo();
	static std::string BuildCancelGenVideo(const std::string& strRecordTime);
	static std::string BuildLangDL(int type);
	static std::string BuildStyleDL(int type);
};

