#pragma once

//#include "hid/hid_agent.h"
#include "../wmi/wmi_agent.h"
//#include "xtu/xtu_agent.h"

#include "ioc_container.h"

class agent_keys
{
public:
	static constexpr char keyboard_hid_agent[] = "keyboard_hid_agent";
	static constexpr char keyboard_event_agent[] = "keyboard_event_agent";
	static constexpr char gaming_wmi_agent[] = "gaming_wmi_agent";
	static constexpr char xtu_agent[] = "xtu_agent";
};

class hw_manager: public ioc_container
{
public:
	static hw_manager &get_instance()
	{
		static hw_manager hw_manager;
		return hw_manager;
	}
	 virtual ~hw_manager(void) = default;

	 void release()
	 {
		 ioc_container::clear();
	 }

private:
	hw_manager()
	{
		//ioc_container::register_type<hid_interface,hid_agent, std::wstring , std::wstring , unsigned short, unsigned short>(agent_keys::keyboard_hid_agent);
		//ioc_container::register_type<hid_interface,hid_agent, std::wstring , std::wstring , unsigned short, unsigned short>(agent_keys::keyboard_event_agent);
		//ioc_container::register_type<wmi_interface,wmi_agent>(agent_keys::gaming_wmi_agent);
		//ioc_container::register_type<xtu_interface, xtu_agent>(agent_keys::xtu_agent);


		ioc_container::register_type<wmi_interface, wmi_agent>(agent_keys::gaming_wmi_agent);
	}
};

