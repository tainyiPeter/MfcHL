
#pragma once
//com related api
#include <comdef.h>
#include <WbemCli.h>
#include <atlbase.h>
#pragma comment(lib, "WbemUuid.Lib")
#include <atlsafe.h>
#include <unordered_map>
#include <string>
#include <any>
#include <vector>
////#include <inc\DebugUtil.h>
#include "../inc/DebugUtil.h"

class wmi_class
{
public:
	wmi_class(const CComPtr<IWbemServices> &wmi_s, const std::wstring &class_name) :
		wmi_server_(wmi_s)
	{
		if (class_name.empty() || get_class_object_path(class_name) == false)
		{
			throw std::logic_error{ "bad cast" };
		}
	}

	~wmi_class();

	[[nodiscard]] std::unordered_map<std::wstring, std::any> exec_method(
		const std::wstring &method_name, const std::vector<std::wstring> &items) const
	{
		//DBG_PRINTF_FL((L"wmi_class::exec_method method_name=%s", method_name.c_str()));

		CComPtr<IWbemClassObject>   out_param = nullptr;
		if (WBEM_S_NO_ERROR == wmi_server_->ExecMethod(
				const_cast<BSTR>(_class_path.c_str()),
				const_cast<BSTR>(method_name.c_str()),
				0,
				nullptr,
				nullptr,
				&out_param,
				nullptr))
		{
			return get_output_parameters(out_param, items);
		}

		return {};
	}

	bool set(const std::wstring &method_name, const std::unordered_map<std::wstring, std::any> &in_args)
	{
		CComPtr<IWbemClassObject>   out_param = nullptr;
		CComPtr<IWbemClassObject>   in_param = nullptr;

		DBG_PRINTF_FL((L"wmi_class::execute method_name=%s", method_name.c_str()));

		if (!in_args.empty())
		{
			if (!set_input_parameters(method_name, in_args, in_param))
			{
				return false;
			}
			if (WBEM_S_NO_ERROR == wmi_server_->ExecMethod(
					const_cast<BSTR>(_class_path.c_str()),
					const_cast<BSTR>(method_name.c_str()),
					0,
					nullptr,
					in_param,
					&out_param,
					nullptr))
			{
				return true;
			}
		}

		return false;
	}

	bool execute(const std::wstring &method_name, 
		const std::unordered_map<std::wstring, std::any> &in_args, 
		const std::vector<std::wstring> &select_items, 
		std::unordered_map<std::wstring, std::any> &output)
	{
		CComPtr<IWbemClassObject>   out_param = nullptr;
		CComPtr<IWbemClassObject>   in_param = nullptr;

		DBG_PRINTF_FL((L"wmi_class::execute method_name=%s", method_name.c_str()));

		if (!in_args.empty())
		{
			if (!set_input_parameters(method_name, in_args, in_param))
			{
				return false;
			}
			HRESULT hr = wmi_server_->ExecMethod(
					const_cast<BSTR>(_class_path.c_str()),
					const_cast<BSTR>(method_name.c_str()),
					0,
					nullptr,
					in_param,
					&out_param,
					nullptr);
			if (WBEM_S_NO_ERROR == hr)
			{
				//if (!select_items.empty())
				//{
					output = get_output_parameters(out_param, select_items);
				//}
				return true;
			}
		}
		DBG_PRINTF_FL((L"wmi_class::execute method_name=%s reture false", method_name.c_str()));
		return false;
	}

	[[nodiscard]] static std::unordered_map<std::wstring, std::any> get_output_parameters(
		const CComPtr<IWbemClassObject> &out_parameters,
		const std::vector<std::wstring> &items)
	{
		std::unordered_map<std::wstring, std::any> ret_value = {};
		if (out_parameters == nullptr)
		{
			return ret_value;
		}

		if (out_parameters != nullptr)
		{
			CComSafeArray<BSTR> array_names;

			if (!items.empty())
			{
				for (auto &item : items)
				{
					array_names.Add(CComBSTR(item.c_str()));
				}
			}
			else
			{
				SAFEARRAY *safeArray = nullptr;
				out_parameters->GetNames(nullptr,
					WBEM_FLAG_NONSYSTEM_ONLY,
					nullptr,
					&safeArray);
				array_names.Attach(safeArray);
			}

			for (long i = 0; static_cast<long>(array_names.GetCount()) > i; i++)
			{
				CComVariant vt_prop{};
				HRESULT res = ERROR_SUCCESS;
				res = out_parameters->Get(static_cast<LPCWSTR>(array_names.GetAt(i)),
						0,
						&vt_prop,
						nullptr,
						nullptr);
				if (SUCCEEDED(res))
				{
					if (vt_prop.vt == VT_BSTR)
						ret_value[static_cast<LPCWSTR>(array_names.GetAt(i))] =
							std::wstring(static_cast<LPCWSTR>(vt_prop.bstrVal));
					else if (vt_prop.vt == VT_BOOL)
						ret_value[static_cast<LPCWSTR>(array_names.GetAt(i))] =
							static_cast<bool>(vt_prop.boolVal == VARIANT_TRUE ? true : false);
					else if (vt_prop.vt == (VT_ARRAY | VT_I4) ||
						vt_prop.vt == (VT_ARRAY | VT_UI1))
					{
						SAFEARRAY *psa = vt_prop.parray;
						vector<int> array_int;
						for (long li = 0; li < static_cast<long>(psa->rgsabound->cElements); li++)
						{
							int out = 0;
							::SafeArrayGetElement(psa, &li, &out);
							array_int.push_back(out);
						}
						ret_value[static_cast<LPCWSTR>(array_names.GetAt(i))] = array_int;
					}
					else
					{
						ret_value[static_cast<LPCWSTR>(array_names.GetAt(i))] = vt_prop.ulVal;
					}

					DBG_PRINTF_FL((L"wmi_class::get_output_parameters outData=%d", vt_prop.ulVal));
				}
			}
		}
		return ret_value;

	}
private:
	const CComPtr<IWbemServices>& wmi_server_;
	CComPtr<IWbemClassObject> _class_object = nullptr;
	CComPtr<IWbemClassObject> _in_class_object = nullptr;
	std::wstring _class_path;
	auto get_class_object_path(const std::wstring &class_name) -> bool
	{
		CComPtr<IEnumWbemClassObject> wmi_enum;
		CComPtr<IWbemClassObject> class_object;
		if (HRESULT res = wmi_server_->CreateInstanceEnum(CComBSTR(class_name.c_str()),
		                                                  WBEM_FLAG_RETURN_IMMEDIATELY | WBEM_FLAG_FORWARD_ONLY,
		                                                  nullptr,
		                                                  &wmi_enum); SUCCEEDED(res))
		{
			CComVariant out_path;
			DWORD u_returned;
			res = wmi_enum->Next(WBEM_INFINITE, 1, &class_object, &u_returned);
			if (res != WBEM_S_NO_ERROR)
			{
				return false;
			}

			res = class_object->Get(L"__PATH", 0,
					&out_path,
					nullptr,
					nullptr);
			if (FAILED(res))
			{
				return false;
			}

			try
			{
				wmi_server_->GetObject(CComBSTR(class_name.c_str()), 0, nullptr, &_class_object, nullptr);
				if (nullptr == _class_object)
				{
					DBG_PRINTF_FL((L"[wmi_class]: Failed to get class object of %s.\r\n", class_name.c_str()));
					return false;
				}
			}
			catch (...)
			{
				return false;
			}
			_class_path = out_path.bstrVal;
			return true;
		}

		return false;
	}

	bool set_input_parameters(const std::wstring &method_name,
		const std::unordered_map<std::wstring, std::any> &args,
		CComPtr<IWbemClassObject> &in_param)
	{
		if (nullptr == _class_object)
		{
			DBG_PRINTF_FL((L"[wmi_class]: class object is invalid.\r\n"));
			return false;
		}

		HRESULT	h_res = ERROR_SUCCESS;
		h_res = _class_object->GetMethod(method_name.c_str(),
				0,
				&_in_class_object,
				nullptr);
		if (WBEM_S_NO_ERROR == h_res)
		{
			if (_in_class_object != nullptr)
			{
				//	create instance copy
				if (WBEM_S_NO_ERROR == (h_res = _in_class_object->SpawnInstance(0, &in_param)))
				{
					for (auto it = args.begin(); it != args.end(); ++it)
					{
						CComVariant comVar;

						if (it->second.type() == typeid(int))
						{
							comVar = std::any_cast<int>(it->second);
						}
						else if (it->second.type() == typeid(UINT8))
						{
							comVar = std::any_cast<UINT8>(it->second);
						}
						else if (it->second.type() == typeid(long))
						{
							comVar = std::any_cast<long>(it->second);
						}
						else if (it->second.type() == typeid(unsigned long))
						{
							comVar = std::any_cast<unsigned long>(it->second);
						}
						else if (it->second.type() == typeid(std::wstring))
						{
							comVar = std::any_cast<std::wstring>(it->second).c_str();
						}
						else if (it->second.type() == typeid(std::vector<BYTE>))
						{
							auto v_data = std::any_cast<std::vector<BYTE>>(it->second);
							long element_count = static_cast<long>(v_data.size());
							SAFEARRAY *psa = SafeArrayCreateVector(VT_UI1, 0, element_count);
							// Insert into safe arrays, allocating memory as
							// we do so (destroying the safe array will destroy the allocated memory)
							for (long i = 0; i < element_count; i++)
							{
								long idx = i;
								UCHAR value = v_data[i];
								if (FAILED(SafeArrayPutElement(psa, &idx, &value)))
								{
									SafeArrayDestroy(psa);
									return false;
								}
							}
							comVar.vt = VT_ARRAY | VT_UI1;
							comVar.parray = psa;
						}
						else
						{
							comVar.vt = 0;
						}

						DBG_PRINTF_FL((L"wmi_class::set_input_parameters method_name=%s  comVar.lVal=%d", method_name.c_str(), comVar.ulVal));
						if (comVar.vt != 0)
						{
							h_res = in_param->Put(it->first.c_str(), 0, &comVar, 0);
							if (WBEM_S_NO_ERROR != h_res)
							{
								DBG_PRINTF_FL((L"wmi_class::set_input_parameters GetMethod(%s) put error=0x%x", method_name.c_str(), h_res));
								return false;
							}
						}
					}
					return true;
				}
			}
		}

		DBG_PRINTF_FL((L"wmi_class::set_input_parameters GetMethod(%s) return false", method_name.c_str()));

		return false;
	}


};

inline wmi_class::~wmi_class()
{
}
