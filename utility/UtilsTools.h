#pragma once

#include "Std.h"

#include <iterator>
#include <atlstr.h>
#include <cstdint>




class UtilsTools
{
public:
    /** ��ȡģ�鵱ǰ·��
    */
    static std::string GetModuleDir();

	/** �ַ����ָ�
    */
    static void SplitStr(std::wstring& strSrc, const std::wstring& delim, std::vector<std::wstring>& ret);
    static void SplitStr(std::string& strInStr, const char* delim, std::vector<std::string>& ret);

    /** �ַ����ָ�֮��ת�߹��¼�
    */
    static uint64_t SplitStrToBitEvents(const std::wstring& strSrc);

    /** ��Ⲣ����·��
    */
    static bool CheckAndCreateDir(const CString& strPath);

    /** ��ȡ��ǰʱ���ʽ
    */
    static std::string FormatCurrentTime();

    /** ���� DurationType ��ȡ�߹�ʱ�� ��λ ��
    */

    /** �ַ����滻
    */
    static std::string ReplaceString(const std::string& strSrc, const std::string& strOld, const std::string& strNew);
    static std::wstring ReplaceString(const std::wstring& strSrc, const std::wstring& strOld, const std::wstring& strNew);

    /** ��ʽ����ͳһ·����ʽ����Ҫ��Ϊ Keyֵ���  ��ʽΪ d:\\videoPath  ����"\\"��"/"��β
    */
    static std::wstring FormatVideoSavePath(const std::wstring& strSrc);

    /** ��չ���Ƿ���ͬ�������ִ�Сд
    */
    static bool MatchFileExt(const std::wstring& strFileName, const std::wstring& strExt);

    /** ���̱�������
    */
    static void SaveLocalData(const std::wstring& strLocalFile, const std::string& strData);
    static void LoadLocalData(const std::wstring& strLocalFile, std::string& strData);

	static std::string Trim(const std::string& src);
    static std::wstring Trim(const std::wstring& src);

  
    /** posλ��0��posֻ����һλ�����������
    */
    static uint64_t ClearBit(uint64_t src, uint16_t pos);

     /** ��ȡ����
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

    /** ʱ���ת��
    */
    static std::time_t GetTimestamp();
    static int64_t GetPastTime(int64_t ts);
    // 
	// ��SYSTEMTIMEת��Ϊʱ���
    static int64_t SystemtimeToTimestamp(SYSTEMTIME& st);

    //2024-10-17 16:28:58:123 ת��  SYSTEMTIME �ṹ��
    static SYSTEMTIME StringToSystemTime(const std::string& strTime);
    
    static time_t TimeStringToTimestamp(std::string strTime);

    static bool GetFileHash256(const char* filePath, BYTE* hashBuffer);

    static void GetMemInfo();

    static std::vector<std::wstring> GetUserLocalGroups();

    static int32_t GetDiskFreeSpace(); //����MB
};