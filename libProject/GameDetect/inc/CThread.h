// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once


template <typename T>
class CThread
{
public:
	typedef DWORD(WINAPI *ThreadFunc)(PVOID lpparam); 

	typedef T & RefT;
	//typedef struct _THREADPARAM
	//{
	//	_THREADPARAM(RefParamType _refparam) :refparam(_refparam)
	//	{

	//	}
	//	RefParamType  refparam;
	//	HANDLE TerminateEvent;
	//}THREADPARAM, *PTHREADPARAM;

public:
	CThread(RefT _param, ThreadFunc _ThreadFunc) : m_hThread(nullptr)
		, m_threadid(0)/*, m_threadparam(_param)*/
	{
		//m_threadparam.TerminateEvent = CreateEvent(NULL,TRUE,FALSE,newGUID());
		m_hThread = CreateThread(NULL, 0, _ThreadFunc,(PVOID)&_param, 0, &m_threadid);
	}

	~CThread()
	{
		//if (m_threadparam.TerminateEvent != NULL)
		//{
		//	SetEvent(m_threadparam.TerminateEvent);
		//	CloseHandle(m_threadparam.TerminateEvent);
		//}
	}

	//THREADPARAM m_threadparam;
	DWORD m_threadid;
	HANDLE m_hThread;
};
