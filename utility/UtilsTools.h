#pragma once

#include "Std.h"

#include <iterator>
#include <atlstr.h>
#include <cstdint>




class UtilsTools
{
public:
    /** 获取模块当前路径
    */
    static std::string GetModuleDir();

	/** 字符串分割
    */
    static void SplitStr(std::wstring& strSrc, const std::wstring& delim, std::vector<std::wstring>& ret);
    static void SplitStr(std::string& strInStr, const char* delim, std::vector<std::string>& ret);

    /** 字符串分割之后转高光事件
    */
    static uint64_t SplitStrToBitEvents(const std::wstring& strSrc);

    /** 检测并创建路径
    */
    static bool CheckAndCreateDir(const CString& strPath);

    /** 获取当前时间格式
    */
    static std::string FormatCurrentTime();

    /** 根据 DurationType 获取高光时长 单位 秒
    */

    /** 字符串替换
    */
    static std::string ReplaceString(const std::string& strSrc, const std::string& strOld, const std::string& strNew);
    static std::wstring ReplaceString(const std::wstring& strSrc, const std::wstring& strOld, const std::wstring& strNew);

    /** 格式化成统一路径格式，需要作为 Key值存放  格式为 d:\\videoPath  不以"\\"或"/"结尾
    */
    static std::wstring FormatVideoSavePath(const std::wstring& strSrc);

    /** 扩展名是否相同，不区分大小写
    */
    static bool MatchFileExt(const std::wstring& strFileName, const std::wstring& strExt);

    /** 落盘本地数据
    */
    static void SaveLocalData(const std::wstring& strLocalFile, const std::string& strData);
    static void LoadLocalData(const std::wstring& strLocalFile, std::string& strData);

	static std::string Trim(const std::string& src);
    static std::wstring Trim(const std::wstring& src);

  
    /** pos位置0，pos只能是一位，不能是组合
    */
    static uint64_t ClearBit(uint64_t src, uint16_t pos);

     /** 获取交集
    */
    template <typename T>
    static void GetIntersection(const std::vector<T>& a, const std::vector<T>& b, std::vector<T>& result) 
    {
        std::vector<T> inter;
        std::set_intersection(a.begin(), a.end(),
            b.begin(), b.end(),
            std::back_inserter(inter));
        result = inter;
    }

    /** 时间戳转换
    */
    static std::time_t GetTimestamp();
    static int64_t GetPastTime(int64_t ts);
    // 
	// 将SYSTEMTIME转换为时间戳
    static int64_t SystemtimeToTimestamp(SYSTEMTIME& st);

    //2024-10-17 16:28:58:123 转成  SYSTEMTIME 结构体
    static SYSTEMTIME StringToSystemTime(const std::string& strTime);
    
    static time_t TimeStringToTimestamp(std::string strTime);

    static bool GetFileHash256(const char* filePath, BYTE* hashBuffer);

    static void GetMemInfo();

    static std::vector<std::wstring> GetUserLocalGroups();

    static int32_t GetDiskFreeSpace(); //返回MB
};