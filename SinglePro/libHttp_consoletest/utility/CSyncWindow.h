#pragma once
#include <Windows.h>

 void SwitchToMainThread(void* arg, BOOL forcePost = 0, BOOL sendMessage = 0);
 HWND GetMainThreadWindow();