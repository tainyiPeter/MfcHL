#pragma once

#include <windows.h>


enum BufferPos
{
	BP_Begin = 0,
	BP_End,
	BP_Current
};

class SmartBuffer
{
public:
	SmartBuffer(UINT nCapacity = 0);
	~SmartBuffer();

	const char* Detach();

	const char* Data();
	UINT Length() const;
	UINT Capacity() const;
	BOOL Empty() const;

	void Reset();

	UINT WriteData(const char* pData, UINT nLen, UINT nPos);

	UINT AppendData(const char* pData, UINT nLen);
	UINT AppendString(const char* str);
	UINT AppendString(const wchar_t* str);

	BOOL ToFile(LPCTSTR lpFileName);
private:
	UINT Alloc(UINT nLen);
private:
	char*	m_pBuffer;					// 缓冲区
	UINT	m_nCapacity;				// 容量
	UINT	m_nLen;						// 长度
};
