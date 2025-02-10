#pragma once
//com related api
#include <WbemCli.h>
#include <atlbase.h>
#pragma comment(lib, "WbemUuid.Lib")

class static_wmi_instance
{
public:
	static_wmi_instance(const CComPtr<IWbemServices> &wmi_s) : wmi_server_(wmi_s)
	{
	}

	auto query(const std::wstring& query_wql) const ->
	std::vector<std::unordered_map<std::wstring, std::any>>
	{
		DBG_PRINTF_FL((L"wmi_class::query method_name=%s", query_wql.c_str()));

		CComPtr<IEnumWbemClassObject> wmi_enum = nullptr;
		std::vector<std::unordered_map<std::wstring, std::any>> ret_value = {};
		const BSTR wql = SysAllocString(L"WQL");
		wmi_server_->ExecQuery(wql, const_cast<BSTR>(query_wql.c_str()),
			WBEM_FLAG_RETURN_IMMEDIATELY | WBEM_FLAG_FORWARD_ONLY,
			nullptr,
			&wmi_enum);
		if (wmi_enum != nullptr)
		{
			CComPtr<IWbemClassObject> one_instance;
			while (get_one_instance(wmi_enum, one_instance))
			{
				ret_value.push_back(wmi_class::get_output_parameters(one_instance, {}));
				one_instance = nullptr;
			}
		}

		return ret_value;
	}
private:
	const CComPtr<IWbemServices> &wmi_server_;
	static bool get_one_instance(const CComPtr<IEnumWbemClassObject> &wmi_enum,
		CComPtr<IWbemClassObject> &one_instance
	)
	{
		unsigned long return_count = 0;
		if (SUCCEEDED(wmi_enum->Next(WBEM_INFINITE, 1,
			&one_instance, &return_count)) && return_count != 0)
		{
			return true;
		}
		return false;
	}
};
