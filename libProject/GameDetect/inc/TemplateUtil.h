// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <windows.h>
#include <string>
#include <functional>

enum AutoReleaseType
{
	autofree,
	autoclosehandle
};
template<typename T>
unique_ptr<T, function<void(T*)>> GetAutoFreeUnique_Ptr(T *ptr, AutoReleaseType type)
{
	return unique_ptr<T, function<void(T*)>>(ptr, [type](T* tempptr)->void {
		if (type == autofree)
		{
			if (tempptr != NULL)
			    free(tempptr);
		}
		else if (type == autoclosehandle)
		{
			if (tempptr != NULL && tempptr != INVALID_HANDLE_VALUE)
				CloseHandle(tempptr);
		}
	});
}



