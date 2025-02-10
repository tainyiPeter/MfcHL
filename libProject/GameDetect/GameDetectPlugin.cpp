#include "GameStatusMonitor.hpp"
#include "GameDetectPlugin.h"
#include <ShlObj.h>
#include "inc/MacroDef.h"
#include "ProcessMonitor.h"



void addin_interface_default_impl::Create()
{
	initthread_ = std::thread([]()
		{ 
			CProcessMonitor::getinstance()->StartMonitorProcess(GZ_GAME_PROPERTY_FLAG_DX_FILE | GZ_GAME_PROPERTY_FLAG_GL_FILE | GZ_GAME_PROPERTY_FLAG_GAME_SIZE | GZ_GAME_PROPERTY_FLAG_GPU_MEMORY | GZ_GAME_PROPERTY_FLAG_GPU_LOAD_INTERVAL);
		});
}

bool   addin_interface_default_impl::UnloadRequest()
{
	if (initthread_.joinable())
		initthread_.join();
	//DBG_PRINTF_FL((L"GameDetectAddin::UnloadRequest "));
	CProcessMonitor::getinstance()->StopMonitorProcess();
	Sleep(300);
	CProcessMonitor::getinstance(true);
	//DBG_PRINTF_FL((L"GameDetectAddin::UnloadRequest end"));
	return TRUE;
}


std::string addin_interface_default_impl::Handle_Event(std::string EventInfoXML)
{
	//EventReaction eventReaction;
	//try {
	//	eventReaction = json::parse(EventInfoXML);
	//}
	//catch (...) {
	//	return "fail";
	//}

	//if (eventReaction.EventName == EVENT_SYSRESUME)
	//{
	//	CProcessMonitor::getinstance()->ReNotifyGameEvent();
	//}

	return "done";
}


std::string addin_interface_default_impl::Handle_AppRequest(std::string requestXML, std::function<int(wchar_t*)> callback, HANDLE cancelEvent)
{
	//string result;
	//ResponseType resulttype = ResponseType::Result;
	//int error_code = FailureCodeConst::Contract_FailureCode::CONTRACT_SUCCESS;
	//string responseXML;
	//int isdone = 1;
	//cout << "HandleAppRequest: " << endl;

	//string strRequestCommand = "";
	//string parameter = "";
	//string contractversion = "";
	//AddinRequest contractRequest;
	//try {
	//	contractRequest = json::parse(requestXML);
	//	strRequestCommand = contractRequest.Command;
	//}
	//catch (...) {
	//	result = "Failed";
	//	responseXML = MakeResponse(resulttype,
	//		100,
	//		FailureCodeConst::Contract_FailureCode::ERR_PLUGIN_INVALID_PARAMETER,
	//		ContractFailure.at(FailureCodeConst::Contract_FailureCode::ERR_PLUGIN_INVALID_PARAMETER),
	//		result);
	//	return responseXML;
	//}

	//if (strRequestCommand == GameDetectPluginContractConst::CommandNameGetCapability) {
	//	CapabilityResponse capresponse = GetCapability();
	//	result = MakeResult<CapabilityResponse>(capresponse);

	//}
	//else if (strRequestCommand == GameDetectPluginContractConst::CommandNameGetRunningGamelist) {
	//	GetRunningGamelistResponse response = GetRunningGamelist();
	//	result = MakeResult<GetRunningGamelistResponse>(response);
	//}
	//else if (strRequestCommand == GameDetectPluginContractConst::CommandNameSetGameDetectState) {
	//	SetGameDetectStateRequest setRequest;
	//	try {
	//		setRequest = json::parse(contractRequest.Payload);
	//	}
	//	catch (...) {
	//		result = "Failed";
	//		responseXML = MakeResponse(resulttype,
	//			100,
	//			FailureCodeConst::Contract_FailureCode::ERR_PLUGIN_INVALID_PARAMETER,
	//			ContractFailure.at(FailureCodeConst::Contract_FailureCode::ERR_PLUGIN_INVALID_PARAMETER),
	//			result);
	//		return responseXML;
	//	}

	//	GamingZoneCommonResponse response = SetGameDetectState(setRequest);
	//	result = MakeResult<GamingZoneCommonResponse>(response);
	//}
	//else
	//{

	//	result = "not support function.";
	//	error_code = FailureCodeConst::Contract_FailureCode::ERR_PLUGIN_NOT_FOUND_FUNCTION;
	//}
	//responseXML = MakeResponse(resulttype,
	//	100,
	//	error_code,
	//	ContractFailure.at((FailureCodeConst::Contract_FailureCode)error_code),
	//	result);
	//return 	responseXML;

	return "";
}

extern "C"
{
	__declspec(dllexport) sub_addin_interface *get_instance()
	{
		static addin_interface_default_impl instance;
		return &instance;
	}
}

