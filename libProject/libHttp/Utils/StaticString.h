#pragma once

#include "../PrepareDef.h"


typedef enum STRING_CODE_PAGE
{
	SCP_ANSI	= 0,
	SCP_UTF8,
	SCP_UNICODE,
	SCP_UNKNOWN
} STRING_CODE_PAGE;

class EXTERN_CLASS StaticString
{
public:
	StaticString(const char* pBuffer, STRING_CODE_PAGE cp = SCP_ANSI);
	StaticString(const WCHAR* pBuffer);
	StaticString(LPVOID pBuffer, UINT nLen, STRING_CODE_PAGE cp = SCP_ANSI);
	StaticString();

	~StaticString();

	void Assign(LPVOID pBuffer, UINT nLen, STRING_CODE_PAGE cp = SCP_ANSI);

	StaticString& operator=(const char* pBuffer);
	StaticString& operator=(const WCHAR* pBuffer);
	StaticString& operator=(const StaticString& oth);

	const char* AsString();
	const WCHAR* AsWString();
	const char* AsU8String();

	UINT Length() const { return m_nLen; }

	void* Detach();
private:
	void Reset();
private:
	STRING_CODE_PAGE m_enumCodePage;
	char* m_pBuffer;
	UINT  m_nLen;
};
typedef std::shared_ptr<StaticString>	HStringPtr;
