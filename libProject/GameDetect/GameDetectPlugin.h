#include <vector>
#include <map>
#include <string>
#include <thread>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>


//#include "inc/RegUtil.h"
#include "inc/sub_addin_interface.h"
#include "inc/DebugUtil.h"

class addin_interface_default_impl : public sub_addin_interface
{
public:
	bool init() override
	{
		DBG_PRINTF_FL((L"addin_interface_default_impl: init"));
		Create();
		return true;
	}
	bool release() override
	{
		DBG_PRINTF_FL((L"addin_interface_default_impl: release"));
		UnloadRequest();
		return true;
	}
	std::string handle_request(std::string s_request, std::function<int(wchar_t*)> callback, HANDLE cancel_event) override
	{
		return Handle_AppRequest(std::move(s_request), std::move(callback), cancel_event);
	}
	std::string handle_event(std::string event_action) override
	{
		return Handle_Event(std::move(event_action));
	}

private:
	std::thread initthread_;

	void Create();

	bool UnloadRequest();

	std::string  Handle_Event(std::string EventInfoXML);

	std::string  Handle_AppRequest(std::string requestXML, std::function<int(wchar_t*)> callback, HANDLE cancelEvent);

};

extern "C"
{
	__declspec(dllexport) sub_addin_interface *get_instance();
}
