// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once

#include "hw_manager.h"

class xtu_helper: public xtu_interface
{
public:
	/*
	 * Parameters:
	 *	unsigned id, double value
	 *	Notes:
	 *	if (item.id == 34 || item.id == 79)
					item.value = item.value * 1000;
	 *	return bool
	 */
	bool SetXtuParams(unsigned int id, double value) override
	{
		const std::shared_ptr<xtu_interface> agent(hw_manager::get_instance().resolve_shared<xtu_interface>("xtu_agent"));
		return agent->SetXtuParams(id, value);
	}
	/*
	 * Parameters:
	 *	vector<OCSetInfo> parameters
	 *	OCSetInfo(id,value)
	 *	int id;
	 *	double value;
	 *	Notes:
	 *	if (item.id == 34 || item.id == 79)
					item.value = item.value * 1000;
	 *	return bool
	 */
	bool SetXtuParams(vector<OCSetInfo> parameters) override
	{
		const std::shared_ptr<xtu_interface> agent(hw_manager::get_instance().resolve_shared<xtu_interface>("xtu_agent"));
		return agent->SetXtuParams(std::move(parameters));
	}
	/*
	 * Parameters:
	 *	vector<OCSetInfo> parameters
	 *	OCSetInfo(id,value)
	 *	int id;
	 *	double value;
	 *	return bool
	 */
	bool SetXtuParams2(vector<OCSetInfo> parameters) override
	{
		const std::shared_ptr<xtu_interface> agent(hw_manager::get_instance().resolve_shared<xtu_interface>("xtu_agent"));
		return agent->SetXtuParams2(std::move(parameters));
	}
	bool IsFeatureEnable(unsigned id) override
	{
		const std::shared_ptr<xtu_interface> agent(hw_manager::get_instance().resolve_shared<xtu_interface>("xtu_agent"));
		return agent->IsFeatureEnable(id);
	}
	double GetActiveValue(unsigned id) override
	{
		const std::shared_ptr<xtu_interface> agent(hw_manager::get_instance().resolve_shared<xtu_interface>("xtu_agent"));
		return agent->GetActiveValue(id);
	}
	double GetMonitorValue(unsigned id) override
	{
		const std::shared_ptr<xtu_interface> agent(hw_manager::get_instance().resolve_shared<xtu_interface>("xtu_agent"));
		return agent->GetMonitorValue(id);
	}
};