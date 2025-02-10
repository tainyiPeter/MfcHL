
#pragma once
//com related api
#include <WbemCli.h>
#include <atlbase.h>
#pragma comment(lib, "WbemUuid.Lib")

class wmi_server
{
public:
	wmi_server(const std::wstring &namespace_name):wmi_server_(nullptr), namespace_name_(namespace_name)
	{
		//connect_server(namespace_name);
	}
	~wmi_server()
	{

	}
	bool connect_server()
	{
		CComPtr<IWbemLocator>		wmi_loc = nullptr;
		HRESULT result = ERROR_SUCCESS;
		if (SUCCEEDED(result = CoInitializeSecurity(
			nullptr,
			-1,                          // COM negotiates service
			nullptr,                        // Authentication services
			nullptr,                        // Reserved
			RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
			RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
			nullptr,                        // Authentication info
			EOAC_NONE,                   // Additional capabilities 
			nullptr)) || result == RPC_E_TOO_LATE) {
			if (SUCCEEDED(
				result = CoCreateInstance(CLSID_WbemLocator, nullptr,
					CLSCTX_INPROC_SERVER,
					IID_IWbemLocator,
					(LPVOID *)(&wmi_loc))
			))
			{
				if (SUCCEEDED(result = wmi_loc->ConnectServer(
					const_cast<BSTR>(namespace_name_.c_str()),
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
private:
	CComPtr<IWbemServices> wmi_server_;
	std::wstring namespace_name_;
};
