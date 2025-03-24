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
	//callback Timer执行的callback 调用pcutil::bindAsyncFunc模版方法生成
	//interval Timer的间隔，毫秒
	//loop 是否为循环Timer，如果为false则表示只执行一次（指定interval后执行一次）
	//uiThread 默认为TRUE，表示在界面主线程执行回调， 否则在当前Timer线程执行回调（回调函数需要为短时间操作，否则会影响Timer的精度）
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
		//在线程中不能直接从列表中移除，否则会出现空引用（郁闷） 在下次循环里移除
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
