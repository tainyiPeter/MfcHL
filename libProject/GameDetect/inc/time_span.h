// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary
#pragma once

#include <chrono>
#include <string>
#include "DebugUtil.h"

using std::string;

class time_span
{
public:
	time_span(std::string stop_message):stop_message_(std::move(stop_message))
	{

	}
	~time_span()
	{
		DBG_PRINTF_FL((L"%S, time cost - %d", stop_message_.c_str(), std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::steady_clock::now() - start).count()));
	}
private:
	std::string stop_message_;
	std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
};