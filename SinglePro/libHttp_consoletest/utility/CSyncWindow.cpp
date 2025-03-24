#include "CSyncWindow.h"
#include <windef.h>
#include <WinBase.h>
#include "MsgHandlerWnd.h"
#include "log.h"
#include "AsyncFuncCall.h"
#define WM_ASYNC_HANDLE			  (WM_APP + 1001)

static UINT g_moduleMainThreadID = 0;
static CMessageHandlerWnd* s_messageHandlerWnd = NULL;
 using namespace pcutil;
void SwitchToMainThread(void* arg, BOOL forcePost, BOOL sendMessage)
{
	if (arg == NULL)
	{
		return;
	}

	if (g_moduleMainThreadID == 0)
	{
		//如果没有初始化主线程窗口消息，则直接回调（兼容业务线程驱动的问题）
		//LOG_INFO(_T("尚未初始化消息窗口，直接回调"));
		IAsyncFuncCall* callback = static_cast<IAsyncFuncCall*>(arg);
		callback->Execute();
		delete callback;
		return;
	}

	if (s_messageHandlerWnd == NULL || !::IsWindow(s_messageHandlerWnd->GetHWND()))
	{
		//如果窗口消息已经关闭，则直接回调
		LOG_ERROR(_T("消息窗口已经关闭，不再回调"));
		return;
	}

	DWORD threadId = GetCurrentThreadId();
	if ((threadId == g_moduleMainThreadID && !forcePost)/* || s_messageHandlerWnd == NULL || !::IsWindow(s_messageHandlerWnd->GetHWND())*/)
	{
		IAsyncFuncCall* callback = static_cast<IAsyncFuncCall*>(arg);
		callback->Execute();
		delete callback;
	}
	else
	{
		if (!sendMessage)
		{
			::PostMessage(s_messageHandlerWnd->GetHWND(), WM_ASYNC_HANDLE, (WPARAM)arg, NULL);
		}
		else
		{
			::SendMessage(s_messageHandlerWnd->GetHWND(), WM_ASYNC_HANDLE, (WPARAM)arg, NULL);
		}

	}
}


extern HWND GetMainThreadWindow()
{
	if (s_messageHandlerWnd == NULL)
	{
		return NULL;
	}
	return s_messageHandlerWnd->GetHWND();
}