#ifndef CLOGPARSER_H
#define CLOGPARSER_H

#include "HighLightDef.h"
#include "ParseDef.h"


class CLogParser 
{
public:
    CLogParser();

public:
    void TestLog(const std::wstring& strFile);
    void ParseLog(const std::wstring& strFile);
    void ParseLog(const StringList& strList);
    ServicesVec& GetServices() { return m_Services; }

private:

    //解析单行字符串
    LineType ParseLineString(const std::string& strLine, LogData& data);
    LineType GetLineType(const std::string& strLine);

    //解析时间戳
    int64_t ParseTimeStamp(const std::string& strTime);

    int64_t ParseServiceStart(const std::string& strLine);

    /* 解析后处理字符
     */
    uint32_t ParseImageStart(const std::string& strLine);
    AreaInfo ParseImageDetail(const std::string& strLine, GameType& gameType);

    /* 取strKey后面的文字，一般如 areaID:64，则返回字符串 64
     */
    std::string FetchText(const std::string& strSrc, const std::string& strKey);
    int FetchTextToDigit(const std::string& strSrc, const std::string& strKey);

    int64_t GetTimestamp(const std::string& strTime);
private:
    ServicesVec  m_Services; //服务重启次数
    std::unordered_map<std::string, LineType> m_mapLineType;
};

#endif // CLOGPARSER_H
