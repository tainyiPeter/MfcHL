#include "IKeyEventSDK.h"

#include <windows.h>
#include <iostream>
#include <map>

class KeyEventSDKImpl : public IKeyEventSDK 
{
private:
    HHOOK hKeyboardHook = nullptr;
    std::function<void(int keyCode, bool isPressed)> keyEventCallback;

    // 钩子回调函数
    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode == HC_ACTION) {
            KBDLLHOOKSTRUCT* pKeyInfo = (KBDLLHOOKSTRUCT*)lParam;
            bool isPressed = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);

            // 调用回调函数
            if (instance && instance->keyEventCallback) {
                instance->keyEventCallback(pKeyInfo->vkCode, isPressed);
            }
        }
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    // 单例实例
    static KeyEventSDKImpl* instance;

public:
    KeyEventSDKImpl() {
        instance = this;
    }

    ~KeyEventSDKImpl() {
        stopKeyDetection();
        instance = nullptr;
    }

    void registerKeyEventCallback(std::function<void(int keyCode, bool isPressed)> callback) override {
        keyEventCallback = callback;
    }

    bool isKeyPressed(int keyCode) override {
        return (GetAsyncKeyState(keyCode) & 0x8000) != 0;
    }

    void startKeyDetection() override 
    {
        if (hKeyboardHook == nullptr) 
        {
            hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
            if (hKeyboardHook == nullptr) {
                std::cerr << "Failed to install keyboard hook!" << std::endl;
            }
        }
    }

    void stopKeyDetection() override {
        if (hKeyboardHook != nullptr) {
            UnhookWindowsHookEx(hKeyboardHook);
            hKeyboardHook = nullptr;
        }
    }
};
