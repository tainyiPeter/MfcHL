#pragma once

#include "HttpLib\std_macro.h"

#undef EXTERN_CLASS
#ifdef TOOLLIB_PROJECT
#define EXTERN_CLASS _declspec(dllexport)
#else
#define EXTERN_CLASS _declspec(dllimport)
#endif

DECLARE_VECTOR(CString, CSTRINGVEC)

class EXTERN_CLASS StringSplitter
{
public:
	StringSplitter(const CString& strDelimiter = _T("\n"));
	~StringSplitter(void);

public:
	void SetDelimiter(const CString& strDelimiter);
	UINT Split(const CString& strInput);
	UINT Size() const;
	BOOL Empty() const;
	CONST_CSTRINGVECITR Begin() const;
	CONST_CSTRINGVECITR End() const;
	const CString& operator[](UINT nIndex);
	CSTRINGVEC& Data();

private:
	CString		m_strDelimiter;				// �ָ���
	CSTRINGVEC  m_vecResult;				// �����
};
