#ifndef LOGPARSERDEF_H
#define LOGPARSERDEF_H

#include "HighLightDef.h"
#include "Std.h"

#include <unordered_map>
#include <string>
#include <vector>

using time_duration_t = sys_clock::duration; // 时间间隔

/* 动作触发类型
 */
enum class TriggerType
{
    Unknown = 0,
    TrigUser,   //用户操作
    TrigHL,     //抓到高光
};

enum class LineType
{
    Unknow = 0,
    ServiceStart,       //服务重启了之后
    GameStart,          //新开一局游戏
    ImageStart,          //后处理分析开始
};


/** 区域信息
*/
struct AreaInfo
{
    uint32_t m_areaid = 0;
    std::wstring m_strInfo;
};

/* 一帧图片的信息
 */
struct ImageData
{
    //uint32_t  m_cnt = 0;
    GameType  m_gameType = GameType::UNKNOWN;
    std::vector<AreaInfo> m_vecAreaInfo;
};

struct LogData
{
    void SetType(LineType lType)
    {
        if (lType == LineType::ImageStart)
        {
            m_type = TriggerType::TrigHL;
        }
    }

    int64_t         m_time;
    TriggerType     m_type = TriggerType::Unknown;
    ImageData       m_imageData;
};

using LogInfoVec = std::vector<LogData>;

/* 服务次数
 */
struct ServiceTimer
{
    void Reset()
    {
        m_vecData.clear();
        m_timeGap = 0;
    }
    LogInfoVec  m_vecData;
    int64_t     m_timeGap = 0;
};

using ServicesVec = std::vector<ServiceTimer>;


using ActionMap = std::unordered_map<std::wstring, std::string>;


#endif // LOGPARSERDEF_H
