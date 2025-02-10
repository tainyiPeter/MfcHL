// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include "..\hw_manager.h"

class kbd_hid_helper
{
public:
	kbd_hid_helper() :agent_(hw_manager::get_instance().resolve_shared<hid_interface>(agent_keys::keyboard_hid_agent, std::wstring(L"048D"),
		std::wstring(L"C977"),
		static_cast<unsigned short>(0xFF89),
		static_cast<unsigned short>(0x7)))
	{
	}
	 std::vector<unsigned char> set_feature_report(const std::vector<unsigned char> &input) const
	 {
		 return agent_->set_feature_report(input);
	 }

	 std::vector<unsigned char> get_feature_report(const std::vector<unsigned char> &input, bool is_needed_set) const
	 {
		 return agent_->get_feature_report(input,is_needed_set);
	 }

	 bool get_capability() const
	 {
		 return agent_->get_capability();
	 }

	 bool get_machine_type(std::string& type) const
	 {
		 return agent_->get_machine_type(type);
	 }

	 bool refresh_hid_handle() const
	 {
		 return agent_->refresh_handle();
	 }
private:
	std::shared_ptr<hid_interface> agent_;
};