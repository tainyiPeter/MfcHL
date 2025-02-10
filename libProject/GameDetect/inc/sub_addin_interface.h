#pragma once
#include <string>
#include <functional>

class sub_addin_interface
{
public:
	virtual ~sub_addin_interface() = default;
	virtual bool init() = 0;
	virtual bool release() = 0;
	virtual std::string handle_request(std::string request, std::function<int(wchar_t *)> callback, HANDLE cancel_event) = 0;
	virtual std::string handle_event(std::string event_action) = 0;
};