// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#pragma warning(disable:4192)
#import <msxml6.dll>
using namespace MSXML2;

#include "TemplateUtil.h"

class CMSXMLDoc
{
public:
	CMSXMLDoc()
	{
		bInit = false;
	}

	bool LoadXML(bool bFile, const wstring & xml)
	{
		HRESULT hr = 0;
		hr = mResponseXMLDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60)/*, NULL, CLSCTX_INPROC_SERVER*/);
		if (hr != S_OK)
			return false;

		VARIANT_BOOL vbSuccessful = VARIANT_FALSE;
		if (bFile)
		{
			mResponseXMLDoc->put_async(VARIANT_FALSE);
			CComVariant  filePath(xml.c_str());
			vbSuccessful = mResponseXMLDoc->load(filePath);

			bInit = (vbSuccessful == VARIANT_FALSE ? false : true);
		}
		else
		{
			vbSuccessful = mResponseXMLDoc->loadXML(_bstr_t(xml.c_str()));
			bInit = (vbSuccessful == VARIANT_FALSE ? false : true);
		}

		mCurrentNode = mResponseXMLDoc;
		return bInit;
	}

	wstring GetAttrValue(const wchar_t * attrName, MSXML2::IXMLDOMNodePtr &spNode)
	{
		if (!bInit)
			return L"";

		MSXML2::IXMLDOMNamedNodeMapPtr pAttrMap = NULL;
		HRESULT hr = spNode->get_attributes(&pAttrMap);
		if (hr == S_OK && pAttrMap != NULL)
		{
			MSXML2::IXMLDOMNodePtr spDisplayNameAtrri = NULL;
			spDisplayNameAtrri = pAttrMap->getNamedItem((_bstr_t)attrName);
			if (spDisplayNameAtrri != NULL)
				return wstring((_bstr_t)spDisplayNameAtrri->nodeValue);
		}

		return L"";
	}


	wstring GetAttrValue(const wchar_t * nodePath, const wchar_t * attrName)
	{
		if (!bInit)
			return L"";

		MSXML2::IXMLDOMNodePtr spNode;
		selectSingleNode(nodePath, mCurrentNode, spNode);
		if (spNode != NULL)
			return GetAttrValue(attrName, spNode);

		return L"";
	}

	void GetAttrValueList(const wchar_t * nodePath, const wchar_t * attrName, vector<wstring> & valueList)
	{
		if (!bInit)
			return;

		MSXML2::IXMLDOMNodePtr spoutNode;
		selectSingleNode(nodePath, mCurrentNode, spoutNode);
		if (spoutNode != NULL)
		{
			MSXML2::IXMLDOMNodeListPtr childNodes = spoutNode->GetchildNodes();
			if (childNodes == NULL)
				return;

			HRESULT hr = 0;
			long lChildCount = 0;
			lChildCount = childNodes->length;

			for (int i = 0; i < lChildCount; i++)
			{
				MSXML2::IXMLDOMNodePtr node = NULL;
				hr = childNodes->get_item(i, &node);
				if (hr == S_OK && node != NULL)
				{
					wstring attrValue = GetAttrValue(attrName, node);
					if (!attrValue.empty())
						valueList.push_back(attrValue);
				}
			}
		}
	}

	wstring GetNodeValue(const wchar_t * nodePath)
	{
		if (!bInit)
			return L"";

		MSXML2::IXMLDOMNodePtr spNode;
		selectSingleNode(nodePath, mCurrentNode, spNode);
		if (spNode != NULL)
			return wstring((_bstr_t)spNode->text);

		return L"";
	}

	bool MoveToNode(const wchar_t * nodePath)
	{
		if (!bInit)
			return false;

		MSXML2::IXMLDOMNodePtr node = NULL;
		selectSingleNode(nodePath, mCurrentNode, node);
		if (node != NULL)
		{
			mCurrentNode = node;
			return true;
		}

		return false;
	}

	bool MoveToNode(const wchar_t * nodeName, const wchar_t * attrName, const wchar_t * attrValue)
	{
		if (!bInit)
			return false;

		if (mCurrentNode == NULL)
			return false;

		MSXML2::IXMLDOMNodePtr OutNode = NULL;;
		FindNode(mCurrentNode, nodeName, OutNode, attrName, attrValue);
		if (OutNode != NULL)
		{
			mCurrentNode = OutNode;
			return true;
		}

		return false;
	}

private:

	void selectSingleNode(const wchar_t * nodePath, MSXML2::IXMLDOMNodePtr &spNode, MSXML2::IXMLDOMNodePtr &outResoult)
	{
		if (!bInit)
			return;

		std::vector<wstring> vFindPath = SplitString(nodePath, L"\\");
		if (vFindPath.size() <= 0)
			return;

		MSXML2::IXMLDOMNodePtr rootnode = spNode;

		for (int i = 0; i < vFindPath.size(); i++)
		{
			MSXML2::IXMLDOMNodePtr node = NULL;
			FindNode(rootnode, vFindPath[i].c_str(), node);
			if (node == NULL)
				return;
			rootnode = node;
		}

		if (rootnode != NULL)
			outResoult = rootnode;
	}

	void FindNode(MSXML2::IXMLDOMNodePtr &node, const wchar_t * nodeFindName, MSXML2::IXMLDOMNodePtr & outResoult,
		const wchar_t * attrName = NULL, const wchar_t * attrValue = NULL)
	{
		HRESULT hr = NULL;
		MSXML2::IXMLDOMNodeListPtr childNodes = node->GetchildNodes();
		if (childNodes == NULL)
			return;

		long lChildCount = 0;
		lChildCount = childNodes->length;

		for (int i = 0; i < lChildCount; i++)
		{
			MSXML2::IXMLDOMNodePtr node = NULL;
			hr = childNodes->get_item(i, &node);
			if (hr == S_OK && node != NULL)
			{
				bstr_t nodeName = node->nodeName;
				wchar_t* pName = nodeName.GetBSTR();
				if (_wcsicmp(pName, nodeFindName) == 0)
				{
					if (attrName != NULL && attrValue != NULL)
					{
						wstring attrValueString = GetAttrValue(attrName, node);
						if (!attrValueString.empty())
						{
							if (_wcsicmp(attrValue, attrValueString.c_str()) == 0)
							{
								outResoult = node;
								return;
							}
						}
					}
					else
					{
						outResoult = node;
						return;
					}
				}
			}
			else
				return;
		}
	}

	MSXML2::IXMLDOMDocumentPtr mResponseXMLDoc;
	MSXML2::IXMLDOMNodePtr mCurrentNode;
	bool bInit;
};


