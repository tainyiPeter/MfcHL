// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once

#include <ObjBase.h>

class CAutoCoInit
{
public:
	CAutoCoInit()
	{
		m_hr = ::CoInitialize(NULL);
	}
	~CAutoCoInit()
	{
		if ( IsSucceed() )
			::CoUninitialize();
	}

	CAutoCoInit(IN LPVOID pvReserved, IN DWORD dwCoInit)
	{
		m_hr = ::CoInitializeEx(pvReserved, dwCoInit);
	}

	BOOL IsSucceed()
	{
		return SUCCEEDED(m_hr);
	}
protected:
	HRESULT	m_hr;
};

