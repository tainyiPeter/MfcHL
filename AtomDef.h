#pragma once

#include <any>
#include <queue>

/** ԭ�Ӳ�������
*/
enum class AtomType
{
	Unknown = 0,
	ShowMsg,
	Action,
};


struct ActionCache
{
	ActionCache() {};
	ActionCache(const std::string& strAction, const std::string& strJson)
		:m_strAction(strAction)
		,m_strJson(strJson)
	{}
	std::string m_strAction;
	std::string m_strJson;
	std::string m_strResponse; 
};
/** �Ѳ����������
*/
struct AsnycPipeData
{
	AtomType	m_type = AtomType::Unknown;
	std::any	m_data;
};

using AsyncQueueType = std::queue<AsnycPipeData>;