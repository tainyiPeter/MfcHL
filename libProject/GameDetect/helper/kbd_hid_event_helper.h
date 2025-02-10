// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include "..\hw_manager.h"
#include "..\hid\hid_interface.h"

class kbd_hid_event_helper
{
public:
	kbd_hid_event_helper() :agent_(hw_manager::get_instance().resolve_shared<hid_interface>(agent_keys::keyboard_event_agent,std::wstring(L"048D"),
		std::wstring(L"C968"),
		static_cast<unsigned short>(0xFF99),
		static_cast<unsigned short>(0x10)))
	{
	}
	bool get_input_report(const std::vector<unsigned char> &input,std::function<bool(std::vector<unsigned char>)> callback) const
	{
		return agent_->get_input_report(input, std::move(callback));
	}
	void cancel_input_report() const
	{
		agent_->cancel_input_event();
	}
	bool refresh_hid_handle() const
	{
		return agent_->refresh_handle();
	}
private:
	std::shared_ptr<hid_interface> agent_;
};