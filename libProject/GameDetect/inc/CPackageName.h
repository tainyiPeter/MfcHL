// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once

#include <windows.h>
#include "LibraryCaller.h"
#include <string>
#include <appmodel.h>

using namespace std;


 wstring GetProcessPackageFullName(
	_In_ HANDLE ProcessHandle
);


 wstring GetProcessPackageFamilyName(
	_In_ HANDLE ProcessHandle
);

PACKAGE_ID*  GetProcessPackageId(
	_In_ HANDLE ProcessHandle,
	wstring& strIsHaveName
);

 BOOL CheckIsUWPProc(
	_In_ HANDLE ProcessHandle
);

