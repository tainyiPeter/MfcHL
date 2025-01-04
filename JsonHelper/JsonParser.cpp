#include "JsonParser.h"
#include "../MacroDef.h"
#include "FileHelper.h"
#include "UtilsString.h"

//#include "UtilsString.h"

JsonParser::JsonParser()
{
	//  Init();
}

JsonParser::~JsonParser()
{

}

bool JsonParser::LoadJson(const std::wstring& strPath, ActionMap& mapJson)
{
	std::vector<CString> fileList;
	if (!CFileHelper::EnumSubFiles(strPath.c_str(), fileList))
		return false;

	mapJson.clear();
	for (auto it : fileList)
	{
		if (!CFileHelper::IsFileExists(it))
			continue;

		std::wstring strFileName = it;
		strFileName = UtilsString::GetFileNameNoExt(strFileName);
		mapJson[strFileName] = CFileHelper::ReadAllBinary(it);
	}

	return !mapJson.empty();
}

bool JsonParser::ParseGenVideoList(const std::string& strJson, VideoGenInfoVec& vecData)
{
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(strJson.c_str(), root))
	{
		return false;
	}

	const Json::Value& jsonModule = root[MODULE_CODE];
	if (!jsonModule.isObject())
		return false;

	const Json::Value& jsonData = jsonModule["data"];
	if (!jsonData.isObject())
		return false;

	if (!jsonData[ERROR_CODE].isNull() && jsonData[ERROR_CODE].isInt())
	{
		int ec = jsonData[ERROR_CODE].asInt();
		if (ERROR_OK != ec)
			return false;
	}

	if (!jsonData["progress"].isNull() && jsonData["progress"].isDouble())
	{
		float fProgress = jsonData["progress"].isDouble();
	}

	const Json::Value& jsonVideoList = jsonData["video_gen_list"];
	if (jsonVideoList.isArray())
	{
		for (auto& it : jsonVideoList)
		{
			VideoInfo vi;
			vi.m_strRecordTime = it["recordTime"].asString();
			VideoGenStatus status = (VideoGenStatus)it["status"].asInt();

			vecData.push_back(VideoGenInfo(vi, status));
		}
	}

	return true;
}