#pragma once
#include <functional>
#include <thread>

class IRunnable
{
public:
	virtual ~IRunnable() {};
	virtual void Run() = 0;
};

class CRunnable0 : public IRunnable
{
public:
	CRunnable0(const std::function<void()>& f) : m_fun(f)
	{  }

	virtual void Run() override
	{
		m_fun();
	}
private:
	std::function<void()>  m_fun;
};

template < typename T1 >
class CRunnable1 : public IRunnable
{
public:
	CRunnable1(const std::function<void(T1)>& f, T1 p1) : m_fun(f),m_p1(p1)
	{  }

	virtual void Run() override
	{
		m_fun(m_p1);
	}

	T1 m_p1;
private:
	std::function<void(T1)>  m_fun;
};

template < typename T1, typename T2 >
class CRunnable2 : public IRunnable
{
	typedef  std::function<void(T1, T2)> CRunnable2Callback;
public:
	CRunnable2(
		const CRunnable2Callback& f, T1 p1, T2 p2) : m_fun(f),m_p1(p1),m_p2(p2)
	{  }

	virtual void Run() override
	{
		m_fun(m_p1, m_p2);
	}
	T1 m_p1;
	T2 m_p2;
private:
	CRunnable2Callback  m_fun;
};

extern HWND GetMainThreadWindow();

/*
使用Delay的处理需要注意， Timer到时会直接回调， 如果函数的对象已经析构 可能会崩溃
需要调用方自己保证
*/
inline void PostUITask(std::function<void()> uiTask, int delay = 0 ) {
	HWND msgWnd = GetMainThreadWindow();

	CRunnable0* uiRunnable = new CRunnable0(uiTask);
	if (msgWnd == NULL)
	{
		return uiRunnable->Run();
	}
	if ( delay <= 0 )
	{
		::PostMessage(msgWnd, 199, (WPARAM)uiRunnable, NULL);
	}
	else
	{
		::SetTimer(msgWnd, (WPARAM)uiRunnable, delay, NULL);
	}
}

/* Sample Code:
string response;
BOOL result = CLibHttpHelper::HttpGet(_T("http://www.baidu.com"), response);
PostUITask<BOOL>([&](BOOL result)
{
	if ( result )
	{
	this->ApplySkin();
	}
}, result,10000);
*/

template < typename T1>
inline void PostUITask( std::function<void(T1)> uiTask, T1 parmeter, int delay = 0 ) {
	HWND msgWnd = GetMainThreadWindow();

	CRunnable1<T1>* uiRunnable = new CRunnable1<T1>(uiTask, parmeter);
	if (msgWnd == NULL)
	{
		return uiRunnable->Run();
	}
	if (delay <= 0)
	{
		::PostMessage(msgWnd, 199, (WPARAM)uiRunnable, NULL);
	}
	else
	{
		::SetTimer(msgWnd, (WPARAM)uiRunnable, delay, NULL);
	}
}

template < typename T1, typename T2>
inline void PostUITask( std::function<void(T1,T2)> uiTask, T1 parmeter, T2 parmeter2 , int delay = 0 ) {
	HWND msgWnd = GetMainThreadWindow();

	CRunnable2<T1,T2>* uiRunnable = new CRunnable2<T1,T2>(uiTask, parmeter, parmeter2);
	if (msgWnd == NULL)
	{
		return uiRunnable->Run();
	}
	if (delay <= 0)
	{
		::PostMessage(msgWnd, 199, (WPARAM)uiRunnable, NULL);
	}
	else
	{
		::SetTimer(msgWnd, (WPARAM)uiRunnable, delay, NULL);
	}
}

inline void RunAsyncTask(std::function<void()> threadTask, std::function<void()> uiTask)
{
	std::thread asycTask([uiTask, threadTask]()
	{
		threadTask();
		PostUITask(uiTask);
	});
	asycTask.detach();
}

/* Sample Code:
RunAsyncTask<BOOL>([&]()
{
	//业务线程
	string response;
	return CLibHttpHelper::HttpGet(_T("http://www.baidu.com"), response);
}, [this](BOOL result)
{
	//界面线程
	if (result)
	{
		this->ApplySkin();
	}
});
*/

template < typename T1>
inline void RunAsyncTask( std::function<T1()> threadTask, std::function<void(T1)> uiTask )
{
	std::thread asycTask([uiTask, threadTask]()
	{
		T1 result = threadTask();
		PostUITask<T1>(uiTask, result);
	});
	asycTask.detach();
}

inline void RunAsyncTask(std::function<void()> threadTask)
{
	std::thread asycTask([threadTask]()
	{
		threadTask();
	});
	asycTask.detach();
}
