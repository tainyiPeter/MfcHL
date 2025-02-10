// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <string>
#include <windows.h>

#include "GameContract/CapabilityResponse.h"
#include "GameContract/common.h"

class addin_manager_client;
using namespace GamingHSACommonContract;
class ICapabilityResponse {
public:
	static bool MakeCapabilityResponse(vector<string> contracts,
		std::string& responsexml);

	static bool MakeCapabilityResponse(vector<string> contracts,
		CapabilityResponse & capresp);

	static bool CapablityRequest(const std::string& pluginName,
		std::string& sp_capabilityResponse, addin_manager_client &);
};