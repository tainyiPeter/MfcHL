#ifndef GUIINTERFACE_H
#define GUIINTERFACE_H

#include "Std.h"

#include <cstdint>
#include <string>

//视频质量
enum class VideoQuality
{
    Unknow = 0,
    Low,				//低 (720P 30fps）
    High,				//高 (1080P 60fps)
};

//视频时长类型
enum class DurationType
{
    Unknow = 0,
    Sec_5,			//5秒时长
    Sec_10,			//10秒时长
    Sec_15,			//15秒时长
};

//风格类型
enum class StyleType
{
    Unknow = 0,
    Auto,					//自动
    HighBurn,				//高燃
    GhostAnimal,			//鬼畜
    Quadratic,				//二次元
    SuperReal,				//超现实
    Dream,					//梦幻
};

struct HLInfo
{
    bool					m_noShowTip = true; // 是否不显示高光提示
    //bool					m_enable = false; // 是否开启高光
    StyleType				m_styleType = StyleType::Auto; //视频风格
    uint64_t				m_gameEvents = 0;
    VideoQuality			m_videoQuality = VideoQuality::High; // 视频质量
    DurationType			m_durType = DurationType::Sec_10; //视频时长
    std::wstring   			m_strVideoSavePath;		//视频保存路径
};

/** 视频基本信息
*/
struct VideoInfo
{
	VideoInfo() {};
	VideoInfo(const std::wstring& strUrl, const std::string& strTime, StyleType sType, uint32_t gType)
		: m_strUrl(strUrl)
		, m_strRecordTime(strTime)
		, m_style(sType)
		, m_gameType(gType)
	{
	}

	std::wstring	m_strUrl;
	std::string 	m_strRecordTime;
	StyleType		m_style = StyleType::Unknow;
	uint32_t		m_gameType = 0;
};

/** 视频生成状态
*/
enum class VideoGenStatus
{
	Waiting = 0,
	Gening,
};

/** 视频生成信息
*/
struct VideoGenInfo
{
	VideoGenInfo() {};
	VideoGenInfo(const VideoInfo& info, VideoGenStatus status)
		: m_info(info)
		, m_status(status)
	{

	}
	VideoInfo	m_info;
	VideoGenStatus	m_status = VideoGenStatus::Waiting;
};

using VideoGenInfoVec = std::vector<VideoGenInfo>;














#endif // GUIINTERFACE_H
