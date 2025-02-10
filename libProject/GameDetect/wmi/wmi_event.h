#pragma once

#include <string>
#include <functional>
#include <future>
#include <map>
#include "wmi_server.h"

class event_sink : public IWbemObjectSink
{
	LONG l_ref;

public:
	event_sink(std::function<bool(long, IWbemClassObject **)> callback) :callback_(std::move(callback))
	{
		l_ref = 0;
	}

	virtual ~event_sink() {  }

	ULONG STDMETHODCALLTYPE AddRef() override
	{
		return InterlockedIncrement(&l_ref);
	}

	ULONG STDMETHODCALLTYPE Release() override
	{
		LONG lRef = InterlockedDecrement(&l_ref);
		if (lRef == 0)
			delete this;
		return lRef;
	}

	HRESULT
		STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override
	{
		if (riid == IID_IUnknown || riid == IID_IWbemObjectSink)
		{
			*ppv = (IWbemObjectSink *)this;
			AddRef();
			return WBEM_S_NO_ERROR;
		}

		return E_NOINTERFACE;
	}

	HRESULT STDMETHODCALLTYPE Indicate(LONG lObjectCount,
	                                   IWbemClassObject __RPC_FAR *__RPC_FAR *apObjArray
	) override
	{
		try
		{
			if (callback_)
				callback_(lObjectCount, apObjArray);
		}
		catch (...)
		{
			return WBEM_S_NO_ERROR;
		}
		return WBEM_S_NO_ERROR;
	}

	HRESULT STDMETHODCALLTYPE SetStatus(
		/* [in] */ LONG lFlags,
		/* [in] */ HRESULT hResult,
		/* [in] */ BSTR strParam,
		/* [in] */ IWbemClassObject __RPC_FAR *pObjParam
	) override
	{
		UNREFERENCED_PARAMETER(strParam);
		UNREFERENCED_PARAMETER(pObjParam);
		//if (lFlags == WBEM_STATUS_COMPLETE)
		//{
		//}
		//else if (lFlags == WBEM_STATUS_PROGRESS)
		//{
		//}

		return WBEM_S_NO_ERROR;
	}

private:
	std::function<bool(long, IWbemClassObject **)>  callback_;
};
/*
 * if create thread failed will throw exception.
 */
class wmi_event
{
public:
	bool event_proc(long count, IWbemClassObject ** wbem_object)
	{
		for (int i = 0; i < count; i++)
		{
			std::lock_guard<std::mutex> lk(callbacks_mutex_);
			for (auto & callback : callbacks_)
				callback.second(wmi_class::get_output_parameters(wbem_object[i], {}));
		}
		return true;
	}
	wmi_event(const basic_string<wchar_t>& namespace_name, const basic_string<wchar_t>& event_sql):
		is_create_event_success_(false),
		is_stop_(false),
		namespace_name_(namespace_name),
		event_sql_(event_sql)
	{
		//debug log
	}

	bool create_event_thread(std::condition_variable& cv_complete_event_init)
	{
		t_ = std::async(std::launch::async, [&,this,event_sql = event_sql_]()
		{
			CoInitializeEx(nullptr, COINIT_MULTITHREADED);
			{
				CComPtr<IWbemObjectSink> stub_sink = nullptr;
				CComPtr<IUnknown>		stub_unk = nullptr;
				wmi_server				server(namespace_name_);
				if (server.connect_server())
				{
					CComPtr<IUnsecuredApartment> unsecured_apartment;
					if (SUCCEEDED(CoCreateInstance(CLSID_UnsecuredApartment, NULL,
						CLSCTX_LOCAL_SERVER, IID_IUnsecuredApartment,
						(void**)&unsecured_apartment)))
					{
						const CComPtr<event_sink> event_sink_ptr = new event_sink(std::bind(&wmi_event::event_proc, this, std::placeholders::_1, std::placeholders::_2));
						if (SUCCEEDED(unsecured_apartment->CreateObjectStub(event_sink_ptr, &stub_unk)))
						{
							if (SUCCEEDED(stub_unk->QueryInterface(IID_IWbemObjectSink,
								reinterpret_cast<void**>(&stub_sink))))
							{
								const HRESULT h_ret = server->ExecNotificationQueryAsync(bstr_t(L"WQL"),
									_bstr_t(event_sql.c_str()),
									WBEM_FLAG_SEND_STATUS,
									nullptr,
									stub_sink);

								if (FAILED(h_ret))
								{
									DBG_PRINTF_FL((L"[WMI_EVENT]: Failed to notify query '%s' with code %08x.\r\n", event_sql.c_str(), h_ret));
								}

								while (SUCCEEDED(h_ret))
								{
									std::unique_lock<std::mutex> lk(callbacks_mutex_);
									is_create_event_success_.store(true);
									cv_complete_event_init.notify_all();
									stop_cv_.wait(lk);
									if (is_stop_.load())
									{
										server->CancelAsyncCall(stub_sink);
										break;
									}
								}
							}
						}
					}
				}
				if (!is_create_event_success_.load())
				{
					cv_complete_event_init.notify_all();
				}
			}
			CoUninitialize();
			return true;
		});
		return true;
	}

	int register_event(std::function<bool(std::unordered_map<std::wstring, std::any>)>callback)
	{
		static int callback_id = 1;
		std::lock_guard<std::mutex> lk(callbacks_mutex_);
		callbacks_[callback_id] = std::move(callback);
		return callback_id++;
	}
	size_t unregister_event(int id)
	{
		std::lock_guard<std::mutex> lk(callbacks_mutex_);
		return callbacks_.erase(id);
	}
	~wmi_event()
	{
		try
		{
			is_stop_.store(true);
			stop_cv_.notify_all();
			t_.wait();
		}
		catch (...)
		{

		}

	}
	bool is_create_event_success()
	{
		return is_create_event_success_;
	}

private:
	std::atomic_bool is_create_event_success_;
	std::map<int,std::function<bool(std::unordered_map<std::wstring, std::any>)>> callbacks_;
	std::mutex callbacks_mutex_;
	std::condition_variable stop_cv_;
	std::atomic_bool is_stop_;
	std::future<bool> t_;
	wstring namespace_name_;
	wstring event_sql_;
};
