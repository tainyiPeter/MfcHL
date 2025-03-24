#pragma once

#include <windows.h>
#define LID_WND_CLASS_NAME _T("lenovo_lib_wnd_class")

class CMessageHandlerWnd
{
public:
	CMessageHandlerWnd();
	~CMessageHandlerWnd();
public:
	HWND GetHWND();
	void Close();

private:
	HWND m_hWnd;
};