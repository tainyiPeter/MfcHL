
#include "SmartBuffer.h"
#include "mem_macro.h"

const UINT DEFAULT_DATA_BUFFER_LEN = (1024 * 10);

#define RESET_BUFFER() \
	m_nCapacity = 0;	\
	m_nLen = 0; \
	m_pBuffer = NULL;	

SmartBuffer::SmartBuffer(UINT nCapacity /*= 0*/) 
{
	RESET_BUFFER();
}

SmartBuffer::~SmartBuffer()
{
	DELETE_PTRA(m_pBuffer);
}

const char* SmartBuffer::Data()
{
	return m_pBuffer;
}

UINT SmartBuffer::Length() const
{
	return m_nLen;
}

UINT SmartBuffer::Capacity() const
{
	return m_nCapacity;
}

UINT SmartBuffer::WriteData(const char* pData, UINT nLen, UINT nPos)
{
	if (pData == NULL && nLen == 0)
	{
		ASSERT(FALSE);
		return m_nLen;
	}

	UINT nEndPos = nPos + nLen;

	if (nEndPos > m_nLen)
	{
		Alloc(nEndPos);
		m_nLen = nEndPos;
	}

	memcpy_s(m_pBuffer + nPos, nLen, pData, nLen);

	return nEndPos;
}

UINT SmartBuffer::AppendData(const char* pData, UINT nLen)
{
	return WriteData(pData, nLen, m_nLen);
}

UINT SmartBuffer::Alloc(UINT nLen)
{
	nLen += 2;

	if (nLen <= m_nCapacity)
	{
		return nLen;
	}

	char* pBuffer = m_pBuffer;

	m_pBuffer = new char[nLen];
	
	memset(m_pBuffer, 0, nLen * sizeof(char));

	if (pBuffer != NULL)
	{
		memcpy_s(m_pBuffer, nLen, pBuffer, m_nLen);

		DELETE_PTRA(pBuffer);
	}

	m_nCapacity = nLen;

	return m_nCapacity;
}

const char* SmartBuffer::Detach()
{
	const char* pBuffer = m_pBuffer;

	RESET_BUFFER();

	return pBuffer;
}

BOOL SmartBuffer::Empty() const
{
	return m_nLen == 0;
}

void SmartBuffer::Reset()
{
	DELETE_PTRA(m_pBuffer);
	RESET_BUFFER();
}

UINT SmartBuffer::AppendString(const char* str)
{
	return AppendData(str, strlen(str));
}

UINT SmartBuffer::AppendString(const wchar_t* str)
{
	return AppendData((const char*)str, wcslen(str) * 2);
}

BOOL SmartBuffer::ToFile(LPCTSTR lpFileName)
{
	HANDLE hFile = CreateFile(
		lpFileName,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (!Empty())
	{
		DWORD dwWriteLen = 0;
		WriteFile(hFile, m_pBuffer, m_nLen, &dwWriteLen, NULL);
		ASSERT(dwWriteLen == m_nLen);
	}

	CLOSE_FILE_HANDLE(hFile);

	return TRUE;
}



