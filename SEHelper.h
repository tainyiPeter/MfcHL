#ifndef SEHELPER_H
#define SEHELPER_H


#include <iostream>
#include <windows.h>

#define AGENT_PATH L"SEAgent.dll"
/*
SE安装路径获取：
Path = 计算机\HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Lenovo\SmartEngine
Key = LenovoInstallPath
*/
typedef void(__cdecl* fnSyncChrCallBack)(const char* action, const char* data, char** resp);
typedef void(__cdecl* fnSyncChrDelResp)(char* resp);

typedef int (*fn_se_syn_register) (int module, fnSyncChrCallBack callback, fnSyncChrDelResp delresp);
typedef int (*fn_se_syn_send)(int module, const char* action, const char* paramter, char* response, int length);
typedef int (*fn_se_msg_close)();

int seinvoke_init(fnSyncChrCallBack outcb);
int seinvoke_send(int module, const char* action, const char* paramter, char* response, int length);
int seinvoke_close();

#endif // SEHELPER_H
