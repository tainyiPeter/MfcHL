// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once

#include "pdh_query.h"

#pragma comment(lib, "pdh.lib")

class cpu_usage : public pdh_usage
{
public:
	cpu_usage() :pdh_usage(L"\\Processor Information(_Total)\\% Processor Utility")
	{
	}
	int get_usage()
	{
		auto usage = pdh_usage::get_total_usage();


		if (usage > 100)
		{
			DBG_PRINTF_FL((L"cpu usage more than 100 - %d", usage));
			return 100;
		}

		//let the cpu usage at least display 1%
		if (usage < 1)
			usage = 1;

		return static_cast<int>(usage);
	}
};
