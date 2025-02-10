// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <functional>
#include <string>
#include "DebugUtil.h"
#include "AddinResponse.h"
#include "AddinRequest.h"
#include "FailureCodeConst.h"


using namespace GamingHSACommonContract;
class contract_wrapper
{
public:

	void add_contract_handler(const string &contract_name, std::function<std::string(const std::string &)> contract_handler)
	{
		contract_handlers_no_callback_[contract_name] = contract_handler;
	}
	void add_contract_handler_no_callback_need_caller_pid(const string& contract_name, std::function<std::string(const std::string&,unsigned int)> contract_handler)
	{
		contract_handlers_no_callback_with_pid_[contract_name] = contract_handler;
	}
	void add_contract_handler_with_callback(const string &contract_name, const std::function<std::string(const std::string &, std::function<int(wchar_t*)>, HANDLE)>& contract_handler)
	{
		contract_handlers_with_callback_[contract_name] = contract_handler;
	}

	auto handle_contract(const std::string& json_request,
		std::function<int(wchar_t*)> callback) -> std::string
	{
		std::string result;
		int error_code = FailureCodeConst::Contract_FailureCode::CONTRACT_SUCCESS;

		try {
			const AddinRequest request = json::parse(json_request);
			DBG_PRINTF_FL((L"request: %S", request.Command.c_str()));
			if ( contract_handlers_no_callback_.contains(request.Command))
			{
				try
				{
					result = contract_handlers_no_callback_.at(request.Command)(request.Payload);
				}
				catch (std::out_of_range &e)
				{
					stringstream ss;
					ss << json_request.c_str() << "---out_of_range Exception:" << e.what();
					return ss.str();
				}
				catch (std::exception &e) {
					stringstream ss;
					ss << json_request.c_str() << "--- Exception:" << e.what();
					return ss.str();
				}
			}
			else if (contract_handlers_with_callback_.contains(request.Command))
			{
				if (const auto cancel_event_handle = OpenEventA(SYNCHRONIZE, false, request.cancel_event_id.c_str()))
				{
					DBG_PRINTF_FL((L"OpenEventA retrieve a valid handle\n"));
					result = contract_handlers_with_callback_.at(request.Command)(request.Payload, std::move(callback), cancel_event_handle);
					CloseHandle(cancel_event_handle);
				}
				else
					DBG_PRINTF_FL((L"OpenEventA cannot retrieve a valid handle\n"));
			}
			else if(contract_handlers_no_callback_with_pid_.contains(request.Command)) {
				result = contract_handlers_no_callback_with_pid_.at(request.Command)(request.Payload, request.callerPid);
			} else
			{
				result = "can not found the command.";
				error_code = FailureCodeConst::ERR_PLUGIN_INVALID_PARAMETER;
			}
		}
		catch (...) {
			result = "addin request parse error.";
			error_code = FailureCodeConst::ERR_PLUGIN_SERIAL;
		}

		DBG_PRINTF_FL((L"response: %S", result.c_str()));
		return MakeResponse(ResponseType::Result,
			100,
			error_code,
			ContractFailure.at(static_cast<const map<FailureCodeConst::Contract_FailureCode, string>::key_type>(error_code)),
			result);
		
	}
private:
	std::unordered_map<std::string, std::function<std::string(std::string)>> contract_handlers_no_callback_;
	std::unordered_map<std::string, std::function<std::string(std::string,unsigned int)>> contract_handlers_no_callback_with_pid_;
	std::unordered_map<std::string, std::function<std::string(const std::string &, std::function<int(wchar_t*)>, HANDLE)>> contract_handlers_with_callback_;
};
