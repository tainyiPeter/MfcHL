#include "SEHelper.h"

fn_se_syn_register se_syn_register = NULL;
fn_se_syn_send se_syn_send = NULL;
fn_se_msg_close se_msg_close = NULL;
fnSyncChrCallBack g_chrcallback = NULL;

void SyncChrCallBack(const char* action, const char* data, char** resp)
{
    if (g_chrcallback)
    {
        g_chrcallback(action, data, resp);
    }
}

void SyncChrDelResp(char* resp)
{
    delete resp;
}

int seinvoke_send(int module, const char* action, const char* paramter, char* response, int length)
{
    if (se_syn_send)
    {
        return se_syn_send(module, action, paramter, response, length);
    }
    return -1;
}

int seinvoke_close()
{
    if (se_msg_close)
    {
        return se_msg_close();
    }
    return -1;
}

int seinvoke_init(fnSyncChrCallBack outcb)
{
    g_chrcallback = outcb;
    HMODULE hModule = LoadLibrary(AGENT_PATH);
    // check dll here
    if (hModule == NULL)
    {
        printf("LoadError: %d \n", GetLastError());
        return -1;
    }
    se_syn_register = (fn_se_syn_register)GetProcAddress(hModule, "se_syn_register");
    if (se_syn_register == NULL)
    {
        int e = GetLastError();
        printf("Fail to load se_syn_register(), error code is %d\n", e);
        return -1;
    }

    se_syn_send = (fn_se_syn_send)GetProcAddress(hModule, "se_syn_send");
    if (se_syn_send == NULL)
    {
        int e = GetLastError();
        printf("Fail to load se_syn_send(), error code is %d\n", e);
        return -1;
    }
    se_msg_close = (fn_se_msg_close)GetProcAddress(hModule, "se_msg_close");
    if (se_msg_close == NULL)
    {
        int e = GetLastError();
        printf("Fail to load se_msg_close(), error code is %d\n", e);
        return -1;
    }
    // If in consolue mode , you could break the process by get char
    // getchar();
    // Init the new interface for PCM , 20 is the init code here 50 is third part
    int nret = se_syn_register(50, SyncChrCallBack, SyncChrDelResp);

    return nret;
}
