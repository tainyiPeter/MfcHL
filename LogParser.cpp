#include "LogParser.h"
#include "MacroDef.h"
#include "Convert.h"
#include "UtilsFile.h"
#include "UtilsTools.h"
#include "UtilsString.h"

#include <sstream>

#define     LogFileName     "SEGameHL.log"

CLogParser::CLogParser()
{
    m_mapLineType =
    {
        { LogStart,   LineType::ServiceStart },
        { LogImage,   LineType::ImageStart },
    };
}

void CLogParser::TestLog(const std::wstring &strFile)
{
   // QStringList strList;
   // QFile file(strFile);
   // if (file.open(QIODevice::ReadOnly | QIODevice::Text))
   // {
   //     while (!file.atEnd())
   //     {
   //         QByteArray line = file.readLine().trimmed(); //即可去除行尾换行符空格等。
   //         strList.append(line);

   //        // QString strLine(line);
   //     }
   // }

   // QString strInfo = "cnt:"+ QString::number(strList.size());
   // emit ApendMsgSignal(strInfo);
   //// return;

   // for (QStringList::const_iterator it = strList.constBegin();
   //      it != strList.constEnd();
   //      ++it)
   // {
   //    emit ApendMsgSignal(*it);
   // }
}

void CLogParser::ParseLog(const std::wstring &strFile)
{
    StringList strList;
    UtilsFile::GetStringByLine(strFile, strList);

    return ParseLog(strList);
}

void CLogParser::ParseLog(const StringList& strList)
{
    if (strList.empty()) return;

    ServiceTimer st;
    StringList::const_iterator it = strList.begin();
    for (; it != strList.end(); ++ it)
    {
        std::string strLine = *it;
        LogData info;

        LineType lType = ParseLineString(strLine, info);
        if(LineType::ServiceStart == lType)
        {
            if(st.m_timeGap > 0)
            {
                m_Services.push_back(st);
                st.Reset();
            }
            st.m_timeGap = UtilsTools::GetPastTime(info.m_time);
            st.m_vecData.push_back(info);
        }
        else if(LineType::ImageStart == lType)
        {
            uint32_t cnt = ParseImageStart(strLine);
            GameType gType = GameType::UNKNOWN;
            for(uint32_t idx = 0; idx < cnt; ++idx)
            {
                it++;
                if(it == strList.end())
                    break;
                strLine = *it;
                AreaInfo vi = ParseImageDetail(strLine, gType);
                if(vi.m_areaid > 0)
                {
                    info.m_imageData.m_vecAreaInfo.push_back(vi);
                }
            }

            info.m_imageData.m_gameType = gType;
            if(info.m_imageData.m_gameType != GameType::UNKNOWN)
            {
                info.SetType(lType);
                st.m_vecData.push_back(info);
            }
        }
    }

    if(st.m_timeGap > 0)
    {
        m_Services.push_back(st);
    }

}

LineType CLogParser::ParseLineString(const std::string &strLine, LogData& data)
{
    int pos = strLine.find(TimeSplit);
    if(pos == std::string::npos)
        return LineType::Unknow;

    std::string strTime = strLine.substr(0, pos);
    std::string strInfo = strLine.substr(pos + strlen(TimeSplit));

    //parse time
    data.m_time = ParseTimeStamp(strTime);

    //parse Info
    LineType lType = GetLineType(strInfo);

    return lType;
}

LineType CLogParser::GetLineType(const std::string &strLine)
{
    if(strLine.empty()) return LineType::Unknow;

    for (auto& it : m_mapLineType)
    {
        int pos = strLine.find(it.first);
        if ( pos != std::string::npos)
        {
            return it.second;
        }
    }

    return LineType::Unknow;
}

int64_t CLogParser::ParseTimeStamp(const std::string& strTime)
{
    std::string strFullTime = UtilsString::FormatString("%s-%s", CurYear, strTime.c_str());

    return GetTimestamp(strFullTime);
}

int64_t CLogParser::ParseServiceStart(const std::string &strLine)
{
    return ParseTimeStamp(strLine);
}

uint32_t CLogParser::ParseImageStart(const std::string& strLine)
{
    int pos = strLine.find(LogImage);
    if(pos == std::string::npos) return 0;

    std::string strText = strLine.substr(pos+strlen(LogImage));
    StringList strList;
    UtilsString::SplitStr(strText, ',', strList);
    if(strList.size() <2) return 0;

    std::string strCnt = strList[0];
    return atoi(strCnt.c_str());
}

AreaInfo CLogParser::ParseImageDetail(const std::string &strLine, GameType& gameType)
{
    AreaInfo ai;

    StringList strList;
    UtilsString::SplitStr(strLine, ',', strList);
    if(strList.size() < 3) return ai;
    uint32_t gType = FetchTextToDigit(strList[0], LogImageDetail);
    gameType = GameType(gType);
    ai.m_areaid = FetchTextToDigit(strList[1], PostAreaId);
    std::string strInfo =  FetchText(strList[2], PostText);
    ai.m_strInfo = Utility::utf8_2_unicode(strInfo);
    return ai;
}

std::string CLogParser::FetchText(const std::string& strSrc,const std::string& strKey)
{
    int pos = strSrc.find(strKey);
    if(pos == std::string::npos) return "";
    return strSrc.substr(pos+strKey.length());
}

int CLogParser::FetchTextToDigit(const std::string &strSrc, const std::string& strKey)
{
    std::string strText = FetchText(strSrc, strKey);
    return atoi(strText.c_str());
}

int64_t CLogParser::GetTimestamp(const std::string& strTime)
{
    SYSTEMTIME st = UtilsTools::StringToSystemTime(strTime);
    int64_t oldTs = UtilsTools::SystemtimeToTimestamp(st);
    return oldTs - (int64_t)8 * 60 * 60 * 1000;
}
