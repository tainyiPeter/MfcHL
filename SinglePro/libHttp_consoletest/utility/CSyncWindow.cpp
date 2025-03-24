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
		//���û�г�ʼ�����̴߳�����Ϣ����ֱ�ӻص�������ҵ���߳����������⣩
		//LOG_INFO(_T("��δ��ʼ����Ϣ���ڣ�ֱ�ӻص�"));
		IAsyncFuncCall* callback = static_cast<IAsyncFuncCall*>(arg);
		callback->Execute();
		delete callback;
		return;
	}

	if (s_messageHandlerWnd == NULL || !::IsWindow(s_messageHandlerWnd->GetHWND()))
	{
		//���������Ϣ�Ѿ��رգ���ֱ�ӻص�
		LOG_ERROR(_T("��Ϣ�����Ѿ��رգ����ٻص�"));
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