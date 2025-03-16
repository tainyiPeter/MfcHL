// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS // eliminate deprecation warnings for VS2005
#endif
#include <windows.h>

#include <string>  
#include <atlbase.h>
#include <atlstr.h>
#include <atlcom.h>
#include <atlwin.h>
#include <atlctl.h>
#include <atlhost.h>
#include <atlcoll.h>
#include <atlutil.h>
#include <atlenc.h>
#include <atlconv.h>
#include <atlfile.h>
#include <atltime.h>

#include <functional>
#include <shlobj.h>

using namespace std;


// TODO:  在此处引用程序需要的其他头文件
