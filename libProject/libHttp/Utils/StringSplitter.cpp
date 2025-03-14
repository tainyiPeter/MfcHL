#include "pch.h"
//
//#include "StringSplitter.h"
//
//StringSplitter::StringSplitter(const CString& strDelimiter)
//{
//	SetDelimiter(strDelimiter);
//}
//
//StringSplitter::~StringSplitter(void)
//{
//}
//
//void StringSplitter::SetDelimiter(const CString& strDelimiter)
//{
//	m_strDelimiter = strDelimiter;
//}
//
//UINT StringSplitter::Split(const CString& strInput)
//{
//	m_vecResult.clear();
//
//	if (strInput.IsEmpty())
//	{
//		return 0;
//	}
//
//	int nLen = strInput.GetLength() + 1;
//
//	TCHAR *pBuffer = new TCHAR[nLen];
//	memset(pBuffer, 0, nLen * (sizeof TCHAR));
//	_tcscpy_s(pBuffer, nLen, strInput);
//
//	TCHAR *pToken = NULL;
//	
//	TCHAR *pPos = _tcstok_s(pBuffer, m_strDelimiter, &pToken);
//
//	while (pPos != NULL)
//	{
//		m_vecResult.push_back(pPos);
//		pPos = _tcstok_s(NULL, m_strDelimiter, &pToken);
//	}
//
//	delete[] pBuffer;
//
//	return m_vecResult,Size();
//}
//
//UINT StringSplitter::Size() const
//{
//	return m_vecResult.size();
//}
//
//CONST_CSTRINGVECITR StringSplitter::Begin() const 
//{
//	return m_vecResult.begin();
//}
//
//CONST_CSTRINGVECITR StringSplitter::End() const
//{
//	return m_vecResult.end();
//}
//
//CSTRINGVEC& StringSplitter::Data()
//{
//	return m_vecResult;
//}
//
//const CString& StringSplitter::operator[](UINT nIndex)
//{
//	if (nIndex >= m_vecResult.size())
//	{
//		static CString empty_string;
//		return empty_string;
//	}
//
//	return m_vecResult[nIndex];
//}
//
//BOOL StringSplitter::Empty() const
//{
//	return m_vecResult.empty();
//}
