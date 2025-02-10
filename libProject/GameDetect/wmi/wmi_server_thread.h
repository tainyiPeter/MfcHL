#pragma once
#include <WbemCli.h>
#include <atlbase.h>
#include "worker_thread.h"

#pragma comment(lib, "WbemUuid.Lib")

class wmi_server_thread final : public worker_thread
{
public:
	wmi_server_thread(const std::wstring &namespace_name) :wmi_server_(nullptr)
	{
		//DBG_PRINTF_FL((L"wmi_server_thread::wmi_server_thread namespace_name=%s", namespace_name.c_str()));
		add_task_and_wait([&, namespace_name]()->bool {
			CoInitialize(nullptr); 
			connect_server(namespace_name); 
			return true; });
	}
	virtual ~wmi_server_thread() override
	{
		//DBG_PRINTF_FL((L"wmi_server_thread::~wmi_server_thread "));
		try {
			add_task_and_wait([=]()
				{
					wmi_server_ = nullptr;
					CoUninitialize();
					return true;
				});
		}
		catch (...)
		{
			//DBG_PRINTF_FL((L"wmi_server_thread::~wmi_server_thread exception"));

		}

		//DBG_PRINTF_FL((L"wmi_server_thread::~wmi_server_thread end"));
	}

	bool connect_server(const std::wstring &namespace_name)
	{
		CComPtr<IWbemLocator>		wmi_loc = nullptr;
		if (HRESULT result; SUCCEEDED(result = CoInitializeSecurity(
			nullptr,
			-1, // COM negotiates service
			nullptr, // Authentication services
			nullptr, // Reserved
			RPC_C_AUTHN_LEVEL_DEFAULT, // Default authentication 
			RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
			nullptr, // Authentication info
			EOAC_NONE, // Additional capabilities 
			nullptr)) || result == RPC_E_TOO_LATE) {
			if (SUCCEEDED(
				result = CoCreateInstance(CLSID_WbemLocator, nullptr,
					CLSCTX_INPROC_SERVER,
					IID_IWbemLocator,
					reinterpret_cast<LPVOID*>(&wmi_loc))
			))
			{
				if (SUCCEEDED(result = wmi_loc->ConnectServer(
					const_cast<BSTR>(namespace_name.c_str()),
					nullptr, nullptr,
					nullptr, 0, nullptr, nullptr,
					&wmi_server_)))
				{
					if (SUCCEEDED((result = CoSetProxyBlanket(wmi_server_,
						RPC_C_AUTHN_WINNT,
						RPC_C_AUTHZ_NONE,
						nullptr,
						RPC_C_AUTHN_LEVEL_CALL,
						RPC_C_IMP_LEVEL_IMPERSONATE,
						nullptr,
						EOAC_NONE))))
					{
						return true;
					}
				}
			}
		}
		return false;
	}
	const CComPtr<IWbemServices> &operator->() const {
		return wmi_server_;
	}
	const CComPtr<IWbemServices> &get() const {
		return wmi_server_;
	}
private:
	CComPtr<IWbemServices> wmi_server_;
};
