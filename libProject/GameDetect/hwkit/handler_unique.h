#pragma once

#include <Windows.h>
#include <functional>
#include <memory>

class handler_unique
{
public:
	explicit handler_unique(HANDLE *handle):handle_(handle,[=](HANDLE *in_handle)
	{
		if(in_handle != nullptr)
		{
			CloseHandle(*in_handle);
			delete in_handle;
		}
	}){}
	handler_unique() = default;

	void reset(HANDLE *handle)
	{
		handle_  = std::unique_ptr<HANDLE, std::function<void(HANDLE *)>>(handle, [=](HANDLE *in_handle)
		{
			if (in_handle != nullptr)
			{
				CloseHandle(*in_handle);
				delete in_handle;
			}
		});
	}

	handler_unique& operator =(HANDLE* handle);

	bool operator !=(const handler_unique &other) const
	{
		return handle_ != other.handle_;
	}
	bool operator ==(const handler_unique &other) const
	{
		return handle_ == other.handle_;
	}
	[[nodiscard]]HANDLE get() const
	{
		return *(handle_.get());
	}
	void release()
	{
		handle_ = nullptr;
	}

	bool operator!=(std::nullptr_t null) const
	{
		return handle_ != nullptr;
	}
private:
	std::unique_ptr<HANDLE, std::function<void(HANDLE *)>> handle_;
};

inline handler_unique& handler_unique::operator=(HANDLE* handle)
{
	handle_ = std::unique_ptr<HANDLE, std::function<void(HANDLE*)>>(handle, [=](HANDLE* in_handle)
	{
		if (in_handle != nullptr)
		{
			CloseHandle(*in_handle);
			delete handle;
		}
	});
	return *this;
}
