// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#include "capability_responser.h"

#include "addin_manager_client.h"
#include "GameContract/common.h"
#include "GameContract/FailureCodeConst.h"
#include "GameContract/AddinRequest.h"
#include "GameContract/AddinResponse.h"


bool ICapabilityResponse::MakeCapabilityResponse(vector<string> contracts,std::string& response) {
	CapabilityResponse capresp;
	addin_manager_client client;

	//hardwareinfo
	string sp_capabilityresponse;
	int error_code = FailureCodeConst::Contract_FailureCode::CONTRACT_SUCCESS;

	for (const auto& contract : contracts)
	{
		if (CapablityRequest(contract, sp_capabilityresponse, client) == false)
			continue;
		else 
		{
			try {
				AddinResponse Response = json::parse(sp_capabilityresponse);
				if (Response.ErrorCode == FailureCodeConst::CONTRACT_SUCCESS) {
					CapabilityResponse capability_response = json::parse(Response.Payload);
					capresp.capabilitylist.insert(capresp.capabilitylist.end(), capability_response.capabilitylist.begin(), capability_response.capabilitylist.end());
				}
			}
			catch (...) {

			}
		}
	}

	string capliststring = MakeResult<CapabilityResponse>(capresp);
	response = MakeResponse(ResponseType::Result,
		100,
		error_code,
		ContractFailure.at((FailureCodeConst::Contract_FailureCode)error_code),
		capliststring);

	return true;
}

bool ICapabilityResponse::MakeCapabilityResponse(vector<string> contracts,
	CapabilityResponse & capresp)
{
	addin_manager_client client;
	string sp_capabilityresponse;
	int error_code = FailureCodeConst::Contract_FailureCode::CONTRACT_SUCCESS;

	for (const auto& contract : contracts)
	{
		if (CapablityRequest(contract, sp_capabilityresponse, client) == false)
			continue;
		else
		{
			try {
				AddinResponse Response = json::parse(sp_capabilityresponse);
				if (Response.ErrorCode == FailureCodeConst::CONTRACT_SUCCESS) {
					CapabilityResponse capability_response = json::parse(Response.Payload);
					capresp.capabilitylist.insert(capresp.capabilitylist.end(), capability_response.capabilitylist.begin(), capability_response.capabilitylist.end());
				}
			}
			catch (...) {

			}
		}
	}
	return true;
}


bool ICapabilityResponse::CapablityRequest(const std::string& pluginName, 
	std::string& sp_capabilityResponse, addin_manager_client &client)
{
	bool result = false;
	AddinRequest crequest;
	crequest.Contract = pluginName;//FanManager
	crequest.Command = "Get-Capability";
	CapabilityType capType(CapbilityContactConst::CapbilityTypeMain);
	crequest.Payload = MakeResult<CapabilityType>(capType);
	string cmdrequest = MakeRequest(crequest);

	sp_capabilityResponse = client.send_contract(cmdrequest);
	if (sp_capabilityResponse.length() > 0)
	{
		result = true;
	}

	return result;
};