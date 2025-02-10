// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once

#include "CMSXMLDoc.h"


#define xml_displayname      L"DisplayName"
#define xml_language      L"Language"
#define xml_languagesub      L"Language-"
#define xml_ResourceMapSubtree L"ResourceMapSubtree"
#define xml_NamedResource L"NamedResource"
#define xml_name      L"name"
#define xml_Name      L"Name"
#define xml_Candidate L"Candidate"
#define xml_qualifiers L"qualifiers"
#define xml_Value L"Value"

#define xml_displayname_node				L"Package\\Properties\\DisplayName"
#define xml_resources_node				L"Package\\Resources"
#define Appxmanifestxml                     L"\\Appxmanifest.xml"
#define resourcespri                     L"\\resources.pri"
#define xml_Identity_node				L"Package\\Identity"

#define xml_lauguageresources_node				L"PriInfo\\ResourceMap"
class CUWPFunction
{
public:
	std::wstring Utf82Unicode(const std::string& utf8string)
	{
		int widesize = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, NULL, 0);
		if (widesize == ERROR_NO_UNICODE_TRANSLATION)
		{
			return L"";
		}
		if (widesize == 0)
		{
			return L"";
		}
		std::vector<wchar_t>resultstring(widesize);
		int convresult = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, &resultstring[0], widesize);
		if (convresult != widesize)
		{
			return L"";
		}
		return std::wstring(&resultstring[0]);
	}

	void ParseManifestXML(const wstring & wsAppPath, wstring & displayName)
	{
		try
		{
			displayName = L"";

			wstring appmanifest = wsAppPath.c_str();
			appmanifest += Appxmanifestxml;
			CMSXMLDoc manifestxml;
			if (!manifestxml.LoadXML(true, appmanifest))
			{
				return;
			}

			displayName = manifestxml.GetAttrValue(xml_Identity_node, xml_Name);
			if (!displayName.empty())
			{
				size_t nIdex = displayName.find(L".");
				if (nIdex != displayName.npos)
					displayName = displayName.substr(nIdex+1);
			}
		}
		catch (const std::exception&)
		{
			//DBG_PRINTF_FL((L"NetworkBoost::GetNetworkBoostXML exception message is %s\n", e.what()));
			return;
		}
	};

};


