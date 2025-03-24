//#include "stdafx.h"
//#include "LibBiz.h"

#include "MsgHandlerWnd.h"
#include <windows.h>
#include "Runnable.h"
#include "log.h"
#include "AsyncFuncCall.h"
#define WM_ASYNC_HANDLE			  (WM_APP + 1001)

using namespace pcutil;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HMODULE GetSelfModuleHandle()
{
	MEMORY_BASIC_INFORMATION mbi;
	return ((::VirtualQuery(GetSelfModuleHandle, &mbi, sizeof(mbi)) != 0) ? (HMODULE)mbi.AllocationBase : NULL);
}

CMessageHandlerWnd::CMessageHandlerWnd()
{
	LOG_INFO(_T("create CMessageHandlerWnd"));
	m_hWnd = NULL;

	MSG          msg;
	WNDCLASS     wndclass;
	
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = GetSelfModuleHandle();
	wndclass.hIcon = NULL;
	wndclass.hCursor = NULL;
	wndclass.hbrBackground = NULL;
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = LID_WND_CLASS_NAME;

	if (!RegisterClass(&wndclass))
	{
		DWORD error = GetLastError();
		if (::GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
			LOG_ERROR(_T("CreateWindowEx RegisterClass error :%d"), GetLastError());
			return;
		}
	}
	m_hWnd = ::CreateWindowEx(0, LID_WND_CLASS_NAME, NULL, 0, 0, 0, 0, 0, NULL, NULL, wndclass.hInstance, this);
	if (m_hWnd == NULL || !::IsWindow(m_hWnd))
	{
		LOG_ERROR(_T("CreateWindowEx error :%d"), GetLastError());
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_ASYNC_HANDLE)
	{
		IAsyncFuncCall* callback = static_cast<IAsyncFuncCall*>((void*)wParam);
		__try
		{
			
			callback->Execute();
			delete callback;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return NULL;
		}
	}
	else if (message == 199)
	{
		__try
		{
			IRunnable* uiRunnable = static_cast<IRunnable*>((void*)wParam);
			uiRunnable->Run();
			delete uiRunnable;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return NULL;
		}
	}
	else if ( message == WM_TIMER )
	{
		__try
		{
			::KillTimer(hwnd, wParam);

			IRunnable* uiRunnable = static_cast<IRunnable*>((void*)wParam);
			uiRunnable->Run();
			delete uiRunnable;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return NULL;
		}
	}
	return true;
}
CMessageHandlerWnd::~CMessageHandlerWnd()
{
	
}

HWND CMessageHandlerWnd::GetHWND()
{
	return m_hWnd;
}


void CMessageHandlerWnd::Close()
{
	::PostMessage(m_hWnd, WM_CLOSE, NULL, NULL);
}


//
//LRESULT CMessageHandlerWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//	switch (uMsg)
//	{
//	case WM_ASYNC_HANDLE:
//		SwitchToMainThread((void*)wParam);
//		return 0;
//		break;
//	}
//	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
//}
