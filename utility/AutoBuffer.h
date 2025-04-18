#pragma once
//#include "ClientConfig.h"
//#include "auxtools.h"
#include <atlbase.h>
#include <string>


class CAutoBuffer
{
public:
	CAutoBuffer();
	explicit CAutoBuffer(unsigned int size);
	CAutoBuffer(const CAutoBuffer& src);
	virtual ~CAutoBuffer();

public:
	bool CopyBuffer(const char* pDataSrc, unsigned int nSize);
	void AppendBuffer(const char* pDataSrc, unsigned int nSize);
	void DetachBuffer();
	bool AttachBuffer(char*& pDataSrc, unsigned int nSize);
	bool AllocateBuffer(unsigned int nSize);
	void SwapBuffer(CAutoBuffer& other);
	void FreeBuffer();
	const char& operator[](unsigned int i) const;
	char& operator[](unsigned int i);

public:
	unsigned int GetBufferSize()const;
	char* GetBuffer()const;
	std::string ToString();

private:
	unsigned int m_nSize;
	char* m_pData;

public:
	CAutoBuffer& operator=(const CAutoBuffer& src);
};