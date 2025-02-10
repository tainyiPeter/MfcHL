// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <GameContract/EventRegisterRequest.h>
#include <Windows.h>
#include <string>
#include <memory>
#include "Serializer.h"
#include <GameContract/CustomEvents.h>
#include "FileInfo.h"
#include "AddinResponse.h"

namespace GamingHSACommonContract
{
	class custom_event_status_changed;
}

using std::string;
using std::unique_ptr;

typedef int(*AddinRequestCallback)(const wchar_t *response);
typedef int (*HandleRequest)(const string &request,string &response, AddinRequestCallback callback);
typedef int(*SendRequestToService)(const string &contractName, const string command, const string payload, string &response);
typedef void(*fire_custom_event)(string event_reaction);
typedef std::string(*FHandleEvent)(string event_reaction);
using namespace GamingHSACommonContract;
class addin_caller {
public:
	const char *c_addin_manager_dll_name = "AddinManager.dll";
	addin_caller()
	{
		h_module_ = GetModuleHandleA(c_addin_manager_dll_name);
		if (h_module_ == nullptr) {
			//debug log
			throw std::exception();
		}
		Fn_HandleRequest = reinterpret_cast<HandleRequest>(GetProcAddress(h_module_, "HandleInternalRequest"));
		fn_handle_event = reinterpret_cast<FHandleEvent>(GetProcAddress(h_module_, "fire_internal_event"));
		fn_fire_custom_event = reinterpret_cast<fire_custom_event>(GetProcAddress(h_module_, "fire_custom_event"));
		fn_send_request_to_service = reinterpret_cast<SendRequestToService>(GetProcAddress(h_module_, "send_contract_to_service"));
	}
	~addin_caller()
	{
	}
	HandleRequest Fn_HandleRequest;
	FHandleEvent fn_handle_event;
	fire_custom_event fn_fire_custom_event;
	SendRequestToService fn_send_request_to_service;
private:
	HMODULE h_module_ = nullptr;
};

// if it can not load addinManager.dll, it will throw an exception.
class addin_manager_client
{
public:
	addin_manager_client(){
		addin_caller_ = std::make_unique<addin_caller>();
	}

	string send_contract(const string &request, AddinRequestCallback callback) const
	{
		string response;
		addin_caller_->Fn_HandleRequest(request, response, callback); //
		return response;
	}

	string send_contract_to_servcie(const string &contractName, const string &command, const string &payload) const
	{
		string response;
		addin_caller_->fn_send_request_to_service(contractName, command, payload, response);
		return response;
	}
	//internal
	[[nodiscard]] string fire_event(const string &event_action) const
	{
		return addin_caller_->fn_handle_event(event_action);
	}
	void fire_custom_event(const string &event_action) const
	{
		return addin_caller_->fn_fire_custom_event(event_action);
	}
	void fire_custom_event(const string &event_name,const int state) const
	{
		return addin_caller_->fn_fire_custom_event(Protocol::Serializer::Serialize<EventReaction>(
			EventReaction(event_name, 
				Protocol::Serializer::Serialize<custom_event_status_changed>(custom_event_status_changed(state)))));
	}

	[[nodiscard]] string send_contract(const string &request) const {
		string response;
		addin_caller_->Fn_HandleRequest(request,response,nullptr);
		return response;
	}

	void reg_addin_event(const string &event_name,const string event_par="",const string source="") const
	{
		EventReaction eventinfo;	
		EventRegContext eventpar;

		eventinfo.EventName = "request_event";
		eventpar.EventName = event_name;
		if (source == "")
			eventpar.SourceName = CFileInfo::GetCurrentModuleName();
		else
			eventpar.SourceName = source;
		eventpar.Parameter = event_par;
		eventinfo.Parameter = MakeResult<EventRegContext>(eventpar);
		addin_caller_->fn_fire_custom_event(MakeResult<EventReaction>(eventinfo));
	}

	void cancel_addin_event(const string &event_name, const string event_par = "", const string source = "") const
	{
		EventReaction eventinfo;
		EventRegContext eventpar;

		eventinfo.EventName = "cancel_event";
		eventpar.EventName = event_name;
		if (source == "")
			eventpar.SourceName = CFileInfo::GetCurrentModuleName();
		else
			eventpar.SourceName = source;
		eventpar.Parameter = event_par;
		eventinfo.Parameter = MakeResult<EventRegContext>(eventpar);
		addin_caller_->fn_fire_custom_event(MakeResult<EventReaction>(eventinfo));
	}

private:
	unique_ptr<addin_caller> addin_caller_;
};
