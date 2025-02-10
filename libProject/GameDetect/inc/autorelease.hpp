// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#ifndef _AUTORELEASE_HPP_
#define _AUTORELEASE_HPP_

#include <functional>

using namespace std;


class AutoRelease
{
public:

	AutoRelease(std::function<void()> release_func):release_(std::move(release_func))
	{
	}

	~AutoRelease()
	{
		release_();
	}

private:
	std::function<void()> release_;

};
#endif