#pragma once
#include <vector>
using namespace pcutil;

class CLibThread;
class IGlobalTimer
{
protected:
	IGlobalTimer() {}
public:
	virtual ~IGlobalTimer() {}
public:
	//callback Timerִ�е�callback ����pcutil::bindAsyncFuncģ�淽������
	//interval Timer�ļ��������
	//loop �Ƿ�Ϊѭ��Timer�����Ϊfalse���ʾִֻ��һ�Σ�ָ��interval��ִ��һ�Σ�
	//uiThread Ĭ��ΪTRUE����ʾ�ڽ������߳�ִ�лص��� �����ڵ�ǰTimer�߳�ִ�лص����ص�������ҪΪ��ʱ������������Ӱ��Timer�ľ��ȣ�
	virtual BOOL AddTimer(IAsyncFuncCall *callback, DWORD interval, BOOL loop = FALSE, BOOL uiThread = TRUE) = 0;

	template <typename desttype>
	void DeleteTimer(desttype object)
	{
		void *objectTmp = reinterpret_cast<void*>(object);
		DeleteTimerImpl(objectTmp, NULL);
	}
	template <typename desttype, typename destcall>
	void DeleteTimer(desttype object, destcall callback)
	{
		void *objectTmp = reinterpret_cast<void*>(object);
		void *callbackTmp = reinterpret_cast<void*>(&callback);
		DeleteTimerImpl(objectTmp, callbackTmp);
	}
	virtual void DeleteTimer(pcutil::IAsyncFuncCall *callback) = 0;
protected:

	virtual void DeleteTimerImpl(void *object, void* callback) = 0;
};

class CGlobalTimer
	:public IGlobalTimer
{
protected:
	~CGlobalTimer();
	struct TTimerCallBack
	{
		TTimerCallBack()
		{
			m_interval = INFINITE;
			m_lastTime = 0;
			m_loop = FALSE;
		}
		BOOL m_loop;
		BOOL m_uiThread;
		//���߳��в���ֱ�Ӵ��б����Ƴ����������ֿ����ã����ƣ� ���´�ѭ�����Ƴ�
		BOOL m_stoped;
		DWORD m_interval;
		DWORD m_lastTime;
		CSharedPtr< pcutil::IAsyncFuncCall > m_call;
	};
public:
	static IGlobalTimer* GetInstance();
	BOOL AddTimer(pcutil::IAsyncFuncCall *callback, DWORD interval, BOOL loop = FALSE, BOOL uiThread = TRUE);
	void DeleteTimer(pcutil::IAsyncFuncCall *callback);
protected:
	void DeleteTimerImpl(void *object, void* callback);
	void Execute(pcutil::CSharedPtr< TTimerCallBack > callback);

private:
	void ThreadCall();
	void OnThreadTimer();
	CGlobalTimer();
	CLibThread* m_thread;
	ATL::CComAutoCriticalSection m_monitor;
	std::vector< pcutil::CSharedPtr< TTimerCallBack > > m_calls;
};
