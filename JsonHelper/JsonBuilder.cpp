#include "JsonBuilder.h"
#include "../MacroDef.h"
#include "Convert.h"

std::string CJsonBuilder::ImageDataToJson(const ImageData& data)
{
	/*
	* {
	*  "type":"post",
	*  "data":{
	*          "gameType":2,
	*          "areaInfo":
	*          [
	*          {"areaId":19,"info":"»÷/ËÀ/Öú 3/6/0"},
	*          {"areaId":19,"info":"»÷/ËÀ/Öú 5/4/0"}
	*          ]
	*         }
	* }
	*/

	Json::Value jsonData;
	jsonData[JsGameType] = (int)data.m_gameType;

	Json::Value jsonAreaInfoList;
	for (auto it : data.m_vecAreaInfo)
	{
		Json::Value jsonObj;
		jsonObj[JsAreaId] = (int)it.m_areaid;
		jsonObj[JsInfo] = Utility::unicode_2_utf8(it.m_strInfo);
		jsonAreaInfoList.append(jsonObj);
	}
	jsonData[JsAreaInfo] = jsonAreaInfoList;

	Json::Value jsonRoot;
	jsonRoot[JsType] = JsPost_Hl;
	jsonRoot[JsData] = jsonData;
	Json::FastWriter fw;

	return fw.write(jsonRoot);
}

std::string CJsonBuilder::BuildCancelGenVideo()
{
	Json::Value jsonRoot;
	jsonRoot["type"] = "cgv";
	Json::FastWriter fw;

	return fw.write(jsonRoot);
}

std::string CJsonBuilder::BuildCancelGenVideo(const std::string& strRecordTime)
{
	Json::Value jsonData;
	jsonData["video_name"] = strRecordTime;

	Json::Value jsonRoot;
	jsonRoot[MODULE_CODE] = jsonData;
	Json::FastWriter fw;

	return fw.write(jsonRoot);
}

std::string CJsonBuilder::BuildLangDL(int type)
{
	Json::Value jsonData;
	jsonData["type"] = type;

	Json::Value jsonRoot;
	jsonRoot[MODULE_CODE] = jsonData;

	Json::FastWriter fw;
	return fw.write(jsonRoot);
}

std::string CJsonBuilder::BuildStyleDL(int type)
{
	Json::Value jsonData;
	jsonData["style"] = type;

	Json::Value jsonRoot;
	jsonRoot[MODULE_CODE] = jsonData;

	Json::FastWriter fw;
	return fw.write(jsonRoot);
}