#pragma once

#include <set>
#include <list>
#include "Any.h"

template <class F>
F* create_delegate(F* f)
{
	return f;
}

#define _MEM_DELEGATES(_Q,_NAME)\
	template <class T, class R, class ... P>\
	struct _mem_delegate ## _NAME\
	{\
		T* m_t;\
		R  (T::*m_f)(P ...) _Q;\
		_mem_delegate ## _NAME(T* t, R  (T::*f)(P ...) _Q):m_t(t),m_f(f) {}\
		R operator()(P ... p) _Q\
		{\
			return (m_t->*m_f)(std::forward<P>(p) ...);\
		}\
	};\
	\
	template <class T, class R, class ... P>\
		_mem_delegate ## _NAME<T,R,P ...> create_delegate(T* t, R (T::*f)(P ...) _Q)\
	{\
		_mem_delegate ##_NAME<T,R,P ...> d(t,f);\
		return d;\
	}

_MEM_DELEGATES(, Z)
_MEM_DELEGATES(const, X)
_MEM_DELEGATES(volatile, Y)
_MEM_DELEGATES(const volatile, W)

namespace pcutil
{
	class CAsyncResult
	{
	public:
		CAsyncResult(int statusCode = 0) :callbackResult(NULL), callbackResult2(NULL)
		{
			m_statusCode = statusCode;
		}
		bool Successed()
		{
			return (m_statusCode == 0);
		}
		int GetStatusCode()
		{
			return m_statusCode;
		}
	public:
		CAny callbackResult;
		CAny callbackResult2;
	private:
		int m_statusCode;
	};

	class CEventObserver;
	class IEventHandler
	{
	public:
		virtual void CancelEvent(CEventObserver* object) = 0;
	};

	class IAsyncCallback
	{
	protected:
		IAsyncCallback() {};
	public:
		virtual void OnCallback(CAsyncResult* result) = 0;
		virtual void Cancel(BOOL destory = FALSE) = 0;
		virtual ~IAsyncCallback() {};
		//执行回调函数后，是否立刻删除callback
		bool deleteCallback = true;
	};

	class  CEventObserver
	{
	public:
		//此处为callback的逻辑
		void ConnectCallback(IAsyncCallback* callback)
		{
			m_callbacks.insert(callback);
		}
		void CancelCallback(IAsyncCallback* callback)
		{
			if (m_callbacks.find(callback) != m_callbacks.end())
				m_callbacks.erase(callback);
		}
		void CancelCallbacksAll()
		{
			std::set<IAsyncCallback*>::const_iterator it = m_callbacks.begin();
			while (it != m_callbacks.end())
			{
				if (*it != NULL)
				{
					(*it)->Cancel(TRUE);
				}
				++it;
			}
			m_callbacks.erase(m_callbacks.begin(), m_callbacks.end());
		}
	public:
		//此处为event的逻辑
		void ConnectObserver(IEventHandler* sender)
		{
			m_senders.insert(sender);
		}
		void CancelObserver(IEventHandler* sender)
		{
			if (m_senders.find(sender) != m_senders.end())
			{
				m_senders.erase(sender);
			}
		}
		void CalcelObserverAll()
		{
			std::set<IEventHandler*>::const_iterator it = m_senders.begin();
			while (it != m_senders.end())
			{
				(*it)->CancelEvent(this);
				++it;
			}
			m_senders.erase(m_senders.begin(), m_senders.end());
		}
	public:
		virtual ~CEventObserver()
		{
			CalcelObserverAll();
			CancelCallbacksAll();
		}
	private:
		std::set<IEventHandler*> m_senders;
		std::set<IAsyncCallback*> m_callbacks;
	};

	class IEventDelegate
	{
	public:
		virtual void OnCallBack(CAny  args) = 0;
		virtual void Cancel(IEventHandler* sender) = 0;
		virtual CEventObserver* GetObserver() = 0;
	};

	template< class desttype >
	class CEventDelegate
		:public IEventDelegate
	{
		typedef void(desttype::*FP_CALL)(CAny args);
	public:
		CEventDelegate(desttype* object, FP_CALL function)
		{
			m_object = object;
			m_function = function;
		}
		void OnCallBack(CAny  args)
		{
			(m_object->*m_function)(args);
		}
		void Cancel(IEventHandler* sender)
		{
			m_object->CancelObserver(sender);
		}
		~CEventDelegate()
		{
		}
		bool operator==(CEventDelegate<desttype> const & other)
		{
			bool ret = (other.m_object == m_object
				&& *(void**)(&other.m_function) == *(void**)(&m_function));
			return ret;
		}
		CEventObserver* GetObserver()
		{
			return (CEventObserver*)m_object;
		}
		desttype* m_object;
		FP_CALL m_function;
	};

	class CEventHandler : public IEventHandler
	{
	public:
		CEventHandler() {}
		std::list<IEventDelegate*> m_handles;
	public:
		template< class desttype >
		void Register(desttype* object, void(desttype::*pCallback)(CAny))
		{
			*this += new CEventDelegate< desttype >(object, pCallback);
		}
		template< class desttype >
		void operator +=(CEventDelegate<desttype>* item)
		{
			for (std::list<IEventDelegate*>::iterator it = m_handles.begin(); it != m_handles.end(); ++it)
			{
				//同一个对象挂同一个事件多次，应该没有这个需求吧
				if ((*it)->GetObserver() == item->GetObserver())
				{
					return;
				}
			}
			m_handles.push_back(item);
			item->m_object->ConnectObserver(this);
		}

		void CancelEvent(CEventObserver* object)
		{
			for (std::list<IEventDelegate*>::iterator it = m_handles.begin(); it != m_handles.end(); ++it)
			{
				if ((*it)->GetObserver() == (CEventObserver*)object)
				{
					delete *it;
					m_handles.erase(it);
					break;
				}
			}
		}

		template< class desttype >
		void operator -=(desttype* object)
		{
			CEventObserver* observer = dynamic_cast<CEventObserver*>(object);
			if (object == NULL) return;
			this->CancelEvent(observer);
			object->CancelObserver(this);
		}

		template< class desttype >
		void operator -=(CEventDelegate<desttype>* item)
		{
			for (std::list<IEventDelegate*>::iterator it = m_handles.begin(); it != m_handles.end(); ++it)
			{
				CEventDelegate<desttype>* delegate = static_cast<CEventDelegate<desttype>*>((*it));
				if ((*delegate) == (*item))
				{
					delete *it;
					m_handles.erase(it);
					break;
				}
			}
			item->m_object->CancelObserver(this);
		}

		void operator()(CAny args)
		{
			for (std::list<IEventDelegate*>::iterator it = m_handles.begin(); it != m_handles.end(); ++it)
			{
				(*it)->OnCallBack(args);
			}
		}
		void CancelConnectAll()
		{
			std::list<IEventDelegate*>::const_iterator itNext, it = m_handles.begin();
			while (it != m_handles.end())
			{
				(*it)->Cancel(this);
				++it;
			}
		}
		virtual ~CEventHandler()
		{
			CancelConnectAll();
		}
	private:
		void tempWork(CAny& args);
	};

	template< class desttype = detail::EmptyType >
	class CAsyncCallback
		:public IAsyncCallback
	{
		typedef void(desttype::*FP_CALL_BACK)(CAsyncResult* result);
		typedef void(desttype::*FP_CALL_BACK1)(CAsyncResult* result, CAny args1);
		typedef void(desttype::*FP_CALL_BACK2)(CAsyncResult* result, CAny args1, CAny args2);
		typedef void(desttype::*FP_CALL_BACK3)(CAsyncResult* result, CAny args1, CAny args2, CAny args3);
	public:
		//static CAsyncCallback* NULL_CALLBACK;
	public:
		void Cancel(BOOL destory = FALSE)
		{
			m_canceled = TRUE;
			m_destory = destory;
		}

		void OnCallback(CAsyncResult* result)
		{
			if (m_object == NULL)
			{
				return;
			}
			if (!m_destory)
			{
				m_object->CancelCallback(this);
			}
			if (m_canceled)
			{
				return;
			}
			if (m_function != NULL)
			{
				(m_object->*m_function)(result);
			}
			else if (m_function1 != NULL)
			{
				(m_object->*m_function1)(result, m_args1);
			}
			else if (m_function2 != NULL)
			{
				(m_object->*m_function2)(result, m_args1, m_args2);
			}
			else if (m_function3 != NULL)
			{
				(m_object->*m_function3)(result, m_args1, m_args2, m_args3);
			}
			delete result;
			if (deleteCallback)
			{
				delete this;
			}
		}

		virtual ~CAsyncCallback()
		{
		}

		CAsyncCallback(desttype* pObject = NULL, FP_CALL_BACK pCallback = NULL)
		{
			Init(pObject);
			m_function = pCallback;
		}

		CAsyncCallback(desttype* pObject, FP_CALL_BACK1 pCallback = NULL, CAny args1 = NULL)
		{
			Init(pObject);

			m_function1 = pCallback;
			m_args1 = args1;

		}

		CAsyncCallback(desttype* pObject, FP_CALL_BACK2 pCallback, CAny args1, CAny args2)
		{
			Init(pObject);
			m_function2 = pCallback;
			m_args1 = args1;
			m_args2 = args2;
		}

		CAsyncCallback(desttype* pObject, FP_CALL_BACK3 pCallback, CAny args1, CAny args2, CAny args3)
		{
			Init(pObject);
			m_function3 = pCallback;
			m_args1 = args1;
			m_args2 = args2;
			m_args3 = args3;

		}
		void Init(desttype* pObject)
		{
			m_function = NULL;
			m_function1 = NULL;
			m_function2 = NULL;
			m_function3 = NULL;
			m_destory = FALSE;
			m_canceled = FALSE;
			m_object = pObject;
			if (pObject != NULL)
			{
				pObject->ConnectCallback(this);
			}
		}
		CAny m_args1;
		CAny m_args2;
		CAny m_args3;
		desttype* m_object;
		FP_CALL_BACK	m_function;
		FP_CALL_BACK1	m_function1;
		FP_CALL_BACK2	m_function2;
		FP_CALL_BACK3	m_function3;
		BOOL			m_canceled;
		BOOL			m_destory;

	};

	template<class desttype>
	IAsyncCallback* CreateAsyncBack(desttype* pObject, void(desttype::*pCallback)(CAsyncResult*))
	{
		return new CAsyncCallback< desttype >(pObject, pCallback);
	}

	template<class desttype>
	IAsyncCallback* CreateAsyncBack(desttype* pObject, void(desttype::*pCallback)(CAsyncResult*, CAny), CAny args1)
	{
		return new CAsyncCallback< desttype >(pObject, pCallback, args1);
	}

	template<class desttype>
	IAsyncCallback* CreateAsyncBack(desttype* pObject, void(desttype::*pCallback)(CAsyncResult*, CAny, CAny), CAny args1, CAny args2)
	{
		return new CAsyncCallback< desttype >(pObject, pCallback, args1, args2);
	}

	template<class desttype>
	IAsyncCallback* CreateAsyncBack(desttype* pObject, void(desttype::*pCallback)(CAsyncResult*, CAny, CAny, CAny), CAny args1, CAny args2, CAny arg3)
	{
		return new CAsyncCallback< desttype >(pObject, pCallback, args1, args2, arg3);
	}

#define EXECUTE_CALL_BACK_MAINTHREAD(callback,result)\
if( NULL != callback ){\
	CAsyncResult* asyncResult = new CAsyncResult( 0 );\
	asyncResult->callbackResult=result;\
	callback->deleteCallback=true;\
	SwitchToMainThread(bindAsyncFunc(&IAsyncCallback::OnCallback, callback, asyncResult));\
}


#define EXECUTE_STATUS_CALL_BACK_MAINTHREAD(uicallback,statusCode,result)\
if( NULL != callback ){\
	CAsyncResult* asyncResult = new CAsyncResult( statusCode );\
	asyncResult->callbackResult=result;\
	uicallback->deleteCallback=true;\
	SwitchToMainThread(bindAsyncFunc(&IAsyncCallback::OnCallback, uicallback, asyncResult));\
}\


#define EXECUTE_CALL_BACK_FAILED_MAINTHREAD(callback,statusCode)\
	CAsyncResult* asyncResult = new CAsyncResult( statusCode );\
	callback->deleteCallback=true;\
	SwitchToMainThread(bindAsyncFunc(&IAsyncCallback::OnCallback, callback, asyncResult));\


//多次执行callback的情况调用，不会析构callback
//目前此方法只使用在http断点续传处
//一般情况禁止使用此方法，除非自己能够维护好callbak的管理，防止内存泄露
#define EXECUTE_MULTI_CALL_BACK(callback,result)\
	if( NULL != callback ){\
	callback->deleteCallback=false;\
	callback->OnCallback( result );\
	}

#define EXECUTE_CALL_BACK(callback,result)\
if( NULL != callback ){\
callback->deleteCallback=true;\
callback->OnCallback( result );\
}

#define EXECUTE_STATUS_CODE_CALL_BACK(callback,statusCode)\
if( NULL != callback ){\
CAsyncResult* result = new CAsyncResult( statusCode );\
callback->deleteCallback=true;\
callback->OnCallback( result );\
}

}





