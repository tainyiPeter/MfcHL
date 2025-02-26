#include <functional>

class IKeyEventSDK 
{
public:
    // 注册按键事件回调
    virtual void registerKeyEventCallback(std::function<void(int keyCode, bool isPressed)> callback) = 0;

    // 检测某个按键是否按下
    virtual bool isKeyPressed(int keyCode) = 0;

    // 启动按键检测
    virtual void startKeyDetection() = 0;

    // 停止按键检测
    virtual void stopKeyDetection() = 0;

    virtual ~IKeyEventSDK() = default;
};