// stringext.cpp : 定义 DLL 的初始化例程。
//
#include "pch.h"

#include "HttpLib\StaticString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ALLOC_INIT_BUFFER(buffer, len)		\
	buffer = new char[(len)];	\
	memset(buffer, 0, (len))

StaticString::StaticString(const char* pBuffer, STRING_CODE_PAGE cp) : m_pBuffer(NULL), m_nLen(0), m_enumCodePage(SCP_UNKNOWN)
{
	ASSERT(cp == SCP_ANSI || cp == SCP_UTF8);

	if (pBuffer != NULL)
	{
		UINT nLen = strlen(pBuffer);
		Assign((VOID*)pBuffer, nLen, cp);
	}
}

StaticString::StaticString(const WCHAR* pBuffer) : m_pBuffer(NULL), m_nLen(0), m_enumCodePage(SCP_UNKNOWN)
{
	ASSERT(pBuffer != NULL);

	UINT nLen = wcslen(pBuffer) * 2;

	Assign((VOID*)pBuffer, nLen, SCP_UNICODE);
}

StaticString::StaticString(LPVOID pBuffer, UINT nLen, STRING_CODE_PAGE cp) : m_pBuffer(NULL), m_nLen(0), m_enumCodePage(SCP_UNKNOWN)
{
	ASSERT(pBuffer != NULL);
	Assign(pBuffer, nLen, cp);
}

StaticString::StaticString() : m_pBuffer(NULL), m_nLen(0), m_enumCodePage(SCP_UNKNOWN)
{

}

StaticString::~StaticString()
{
	Reset();
}

void StaticString::Reset()
{
	delete []m_pBuffer;
	m_pBuffer = NULL;
	m_nLen = 0;
	m_enumCodePage = SCP_UNKNOWN;
}

void* StaticString::Detach()
{
	ASSERT(m_pBuffer != NULL);
	void *p = m_pBuffer;

	m_pBuffer = NULL;
	m_nLen = 0;
	m_enumCodePage = SCP_UNKNOWN;

	return p;
}

void StaticString::Assign(LPVOID pBuffer, UINT nLen, STRING_CODE_PAGE cp)
{
	Reset();

	ASSERT(pBuffer != NULL);

	ALLOC_INIT_BUFFER(m_pBuffer, nLen + 2);

	memcpy_s(m_pBuffer, nLen, pBuffer, nLen);
	m_enumCodePage = cp;
	m_nLen = nLen;
}

StaticString& StaticString::operator=(const char* pBuffer)
{
	Reset();

	ASSERT(pBuffer != NULL);
	
	UINT nLen = strlen(pBuffer);

	Assign((VOID*)pBuffer, nLen, SCP_ANSI);

	return *this;
}

StaticString& StaticString::operator=(const WCHAR* pBuffer)
{
	ASSERT(pBuffer != NULL);

	UINT nLen = wcslen(pBuffer) * 2;

	Assign((VOID*)pBuffer, nLen, SCP_UNICODE);

	return *this;
}

StaticString& StaticString::operator=(const StaticString& oth)
{
	if (&oth == this)
	{
		return *this;
	}

	Reset();

	Assign((void*)oth.m_pBuffer, oth.m_nLen, oth.m_enumCodePage);

	return *this;
}

const char* StaticString::AsString()
{
	if (m_nLen == 0)
	{
		return NULL;
	}

	switch (m_enumCodePage)
	{
	case SCP_UTF8:
		{
			AsWString();
		}								// No break here!!!!
	case SCP_UNICODE:
		{
			char* pNewBuffer;
			ALLOC_INIT_BUFFER(pNewBuffer, m_nLen + 2);
			m_nLen = WideCharToMultiByte(CP_ACP, 0, (WCHAR*)m_pBuffer, m_nLen / 2, pNewBuffer, m_nLen, NULL, NULL);
			delete []m_pBuffer;
			m_pBuffer = pNewBuffer;
			m_enumCodePage = SCP_ANSI;
		}
		break;
	default:
		{

		}

	}
	
	return m_pBuffer;
}

const WCHAR* StaticString::AsWString()
{
	if (m_nLen == 0)
	{
		return NULL;
	}

	if (m_enumCodePage == SCP_ANSI || m_enumCodePage == SCP_UTF8)
	{
		char* pNewBuffer;
		ALLOC_INIT_BUFFER(pNewBuffer, m_nLen * 2 + 2);

		m_nLen = MultiByteToWideChar(
			m_enumCodePage == SCP_ANSI ? CP_ACP : CP_UTF8,
			0,
			m_pBuffer, 
			m_nLen, 
			(WCHAR*)pNewBuffer,
			m_nLen * 2
			);

		ASSERT(m_nLen != 0);

		m_nLen *= 2;
		delete []m_pBuffer;
		m_pBuffer = pNewBuffer;
		m_enumCodePage = SCP_UNICODE;
	}
	
	return (WCHAR*)m_pBuffer;
}

const char* StaticString::AsU8String()
{
	if (m_nLen == 0)
	{
		return NULL;
	}

	switch (m_enumCodePage)
	{
	case SCP_ANSI:
		{
			AsWString();
		}								// No break here !!!!
	case SCP_UNICODE:
		{
			char* pNewBuffer;
			ALLOC_INIT_BUFFER(pNewBuffer, m_nLen * 2 + 2);

			m_nLen = WideCharToMultiByte(CP_UTF8, 0, (WCHAR*)m_pBuffer, m_nLen / 2, pNewBuffer, m_nLen * 2, 0, 0);
			delete []m_pBuffer;
			m_pBuffer = pNewBuffer;
			m_enumCodePage = SCP_UTF8;
		}
		break;
	default:
		{

		}
	}

	return m_pBuffer;
}


