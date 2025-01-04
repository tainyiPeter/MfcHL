/**
 * @file    convert.h
 * @brief   
 * @version 1.0
 * @date    2014.6.25
 */
#ifndef __convert_H__
#define __convert_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "std.h"

/// 辅助工具集合
namespace Utility
{
#define APP_DEFAULT_CODEPAGE    936

    int wchar_2_multibyte(const wchar_t* pwszInput, std::string& strOut, int nCodePage=0/*CP_ACP*/);
    int wchar_2_wchar(const wchar_t* pszInput, std::wstring& wstrOut, int nCodePage=0/*CP_ACP*/);

    int multibyte_2_wchar(const char* pszInput, std::wstring& wstrOut, int nCodePage=0/*CP_ACP*/);
    int multibyte_2_multibyte(const char* pszInput, std::string& wstrOut, int nCodePage=0/*CP_ACP*/);

    int gbk_2_utf8(const std::string& strSrc, std::string& strDest, int nCodePage=0/*CP_ACP*/);
    int gbk_2_utf8(const char* strSrc,std::string& strDest);
    int utf8_2_gbk(const std::string& strSrc, std::string& strDest, int nCodePage=0/*CP_ACP*/);
    int utf8_2_gbk(const char* strSrc,std::string& strDest);

    std::string gbk_2_utf8(const std::string& strSrc, int nCodePage=0/*CP_ACP*/);
    std::string gbk_2_utf8(const char* pszSrc, int nCodePage=0/*CP_ACP*/);
    std::string utf8_2_gbk(const std::string& strSrc, int nCodePage=0/*CP_ACP*/);
    std::string utf8_2_gbk(const char* pszSrc, int nCodePage=0/*CP_ACP*/);

    std::wstring gbk_2_unicode(const std::string& sInput, int nCodePage=0/*CP_ACP*/);
    std::wstring gbk_2_unicode(const char* pszInput, int nCodePage=0/*CP_ACP*/);
    std::string unicode_2_gbk(const std::wstring& sInput, int nCodePage=0/*CP_ACP*/);
    std::string unicode_2_gbk(const wchar_t* pszInput, int nCodePage=0/*CP_ACP*/);

    std::wstring utf8_2_unicode(const std::string& sInput);
    std::wstring utf8_2_unicode(const char* pszInput);
    std::string unicode_2_utf8(const std::wstring& sInput);
    std::string unicode_2_utf8(const wchar_t* pszInput);

    int atoi(const char* pInput, int nDefault=0);

    //bool URLEncode(const char *pszInput, CString &strOutput);
    //bool URLEncode(const wchar_t *pszInput, CString &strOutput);
    //bool URLDecode(const char *pszInput, CString &strOutput);
    //bool URLDecode(const wchar_t *pszInput, CString &strOutput);

    bool URLEncode(const char *pszInput, astring &strOutput);
    bool URLEncode(const wchar_t *pszInput, std::string &strOutput);
    bool URLDecode(const char *pszInput, std::string &strOutput);
    bool URLDecode(const wchar_t *pszInput, std::string &strOutput);

	//判断文件的编码类型，返回值1=UTF-8 4= ANSI 2,3为Unicode编码，暂时不支持2/3类型的读取
	//int GetTextEncode(CString strTxtPath);

    //void Double2String(const double& dInput, CString& strOutput, bool bYuan = true);

    void ConverUTF8ToGBK(const std::string& str, char* szDst, int32_t bufSize);


    /// 模拟ATL的转换实现
#define MAX_BUFF_LENGTH 256

    class CW2W
    {
    public:
        CW2W(const wchar_t* psz) throw(...);
        CW2W(const wchar_t* psz, unsigned int nCodePage) throw(...);
        ~CW2W();
        operator wchar_t*() const throw()
        {
            return m_psz;
        }
    protected:
    private:
        void init(const wchar_t* psz) throw(...);
        CW2W(const CW2W&) throw();
        CW2W& operator=(const CW2W&) throw();

    private:
        wchar_t*        m_psz;
        wchar_t         m_szBuffer[MAX_BUFF_LENGTH];
    };

    class CW2CW
    {
    public:
        CW2CW(const wchar_t* psz) throw(...)
            :m_psz(psz)
        {

        }
        CW2CW(const wchar_t* psz, unsigned int nCodePage) throw(...)
            :m_psz(psz)
        {

        }
        ~CW2CW() throw(){}
        operator const wchar_t*() const throw()
        {
            return m_psz;
        }
    protected:
    private:
        CW2CW(const CW2CW&) throw();
        CW2CW& operator=(const CW2CW&) throw();

    private:
        const wchar_t*      m_psz;
    };

    class CA2A
    {
    public:
        CA2A(const char* psz) throw(...);
        CA2A(const char* psz, unsigned int nCodePage) throw(...);
        ~CA2A();
        operator char*() const throw()
        {
            return m_psz;
        }
    protected:
    private:
        void init(const char* psz) throw(...);
        CA2A(const CA2A&) throw();
        CA2A& operator=(const CA2A&) throw();

    private:
        char*       m_psz;
        char        m_szBuffer[MAX_BUFF_LENGTH];
    };

    class CA2CA
    {
    public:
        CA2CA(const char* psz) throw(...)
            :m_psz(psz)
        {

        }
        CA2CA(const char* psz, unsigned int nCodePage) throw(...)
            :m_psz(psz)
        {

        }
        ~CA2CA() throw(){}
        operator const char*() const throw()
        {
            return m_psz;
        }
    protected:
    private:
        CA2CA(const CA2CA&) throw();
        CA2CA& operator=(const CA2CA&) throw();

    private:
        const char*     m_psz;
    };

    class CW2A
    {
    public:
        CW2A(const wchar_t* psz) throw(...);
        CW2A(const wchar_t* psz, unsigned int nCodePage) throw(...);
        ~CW2A() throw();

        operator char*() const throw();
    protected:
    private:
        CW2A(const CW2A&) throw();
        CW2A& operator=(const CW2A&) throw();

        void init(const wchar_t* pszIn, unsigned int nConvertCodePage) throw(...);
    private:
        char*       m_psz;
        char        m_szBuffer[MAX_BUFF_LENGTH];
    };

    class CW2AEX
    {
    public:
        CW2AEX(const wchar_t* psz) throw(...);
        CW2AEX(const wchar_t* psz, unsigned int nCodePage) throw(...);
        ~CW2AEX() throw();

        operator char*() const throw()
        {
            return m_psz;
        }
    protected:
    private:
        CW2AEX(const CW2AEX&) throw();
        CW2AEX& operator=(const CW2AEX&) throw();

        void init(const wchar_t* pszIn, unsigned int nConvertCodePage) throw(...);
    private:
        char*       m_psz;
        char        m_szBuffer[MAX_BUFF_LENGTH];
    };

    class CA2W
    {
    public:
        CA2W(const char* psz) throw(...);
        CA2W(const char* psz, unsigned int nCodePage ) throw(...);
        ~CA2W() throw();

        operator wchar_t*() const throw()
        {
            return m_psz;
        }

    private:
        void init(const char* psz, unsigned int nCodePage) throw(...);

    private:
        wchar_t*    m_psz;
        wchar_t     m_szBuffer[MAX_BUFF_LENGTH];

    private:
        CA2W(const CA2W&) throw();
        CA2W& operator=(const CA2W&) throw();
    };

    class CA2WEX
    {
    public:
        CA2WEX(const char* psz) throw(...);
        CA2WEX(const char* psz, unsigned int nCodePage ) throw(...);
        ~CA2WEX() throw();

        operator wchar_t*() const throw()
        {
            return m_psz;
        }

    private:
        void init(const char* psz, unsigned int nCodePage) throw(...);

    private:
        wchar_t*    m_psz;
        wchar_t     m_szBuffer[MAX_BUFF_LENGTH];

    private:
        CA2WEX(const CA2WEX&) throw();
        CA2WEX& operator=(const CA2WEX&) throw();
    };
}

#ifdef _UNICODE
#define tchar_2_multibyte               Utility::wchar_2_multibyte
#define tchar_2_wchar                   Utility::wchar_2_wchar 
#define multibyte_2_tchar               Utility::multibyte_2_wchar
#define wchar_2_tchar                   Utility::wchar_2_wchar
#define GBK2TEX(param, codePage)        Utility::gbk_2_unicode(param, codePage).c_str()
#define GBK2T(param)                    Utility::gbk_2_unicode(param, APP_DEFAULT_CODEPAGE).c_str()
#define T2GBKEX(param, codePage)        Utility::unicode_2_gbk(param, codePage).c_str()
#define T2GBK(param)                    Utility::unicode_2_gbk(param, APP_DEFAULT_CODEPAGE).c_str()
#define UTF82TEX(param, codePage)       Utility::utf8_2_unicode(param).c_str()
#define UTF82T(param)                   Utility::utf8_2_unicode(param).c_str()
#define UTF82GBK(param)                 Utility::utf8_2_gbk(param).c_str()
#define T2UTF8(param)                   Utility::unicode_2_utf8(param).c_str()
#define T2UTF8EX(param, codePage)       Utility::unicode_2_utf8(param).c_str()
#define Unicode2TEX(param, codePage)    param
#define Unicode2T(param)                param
#define T2UnicodeEX(param, codePage)    param
#define T2Unicode(param)                param
#define U2GBKEX(param, codePage)        Utility::unicode_2_gbk(param, codePage).c_str()
#define U2GBK(param)                    Utility::unicode_2_gbk(param, APP_DEFAULT_CODEPAGE).c_str()
#define GBK2UEX(param, codePage)        Utility::gbk_2_unicode(param, codePage).c_str()
#define GBK2U(param)                    Utility::gbk_2_unicode(param, APP_DEFAULT_CODEPAGE).c_str()


#define UW2T        Utility::CW2W
#define UW2TEX      Utility::CW2WEX
#define UW2CT       Utility::CW2CW
#define UW2CTEX     Utility::CW2CWEX
#define UT2W        Utility::CW2W
#define UT2WEX      Utility::CW2WEX
#define UT2CW       Utility::CW2CW
#define UT2CWEX     Utility::CW2CWEX

#define UA2T        Utility::CA2W
#define UA2TEX      Utility::CA2WEX
#define UA2CT       Utility::CA2W
#define UA2CTEX     Utility::CA2WEX
#define UT2A        Utility::CW2A
#define UT2AEX      Utility::CW2AEX
#define UT2CA       Utility::CW2A
#define UT2CAEX     Utility::CW2AEX
#else
#define tchar_2_multibyte               Utility::multibyte_2_multibyte
#define tchar_2_wchar                   Utility::multibyte_2_wchar 
#define multibyte_2_tchar               Utility::multibyte_2_multibyte
#define wchar_2_tchar                   Utility::wchar_2_multibyte
#define GBK2TEX(param, codePage)        param
#define GBK2T(param)                    param
#define T2GBKEX(param, codePage)        param
#define T2GBK(param)                    param
#define Unicode2TEX(param, codePage)    Utility::unicode_2_gbk(param, codePage).c_str()
#define Unicode2T(param)                Utility::unicode_2_gbk(param, APP_DEFAULT_CODEPAGE).c_str()
#define T2UnicodeEX(param, codePage)    Utility::gbk_2_unicode(param, codePage).c_str()
#define T2Unicode(param)                Utility::gbk_2_unicode(param, APP_DEFAULT_CODEPAGE).c_str()
#define UTF82TEX(param, codePage)       Utility::utf8_2_gbk(param, codePage).c_str()
#define UTF82GBK(param)                 Utility::utf8_2_gbk(param).c_str()
#define UTF82T(param)                   Utility::utf8_2_gbk(param, APP_DEFAULT_CODEPAGE).c_str()
#define T2UTF8EX(param, codePage)       Utility::gbk_2_utf8(param, codePage).c_str()
#define T2UTF8(param)                   Utility::gbk_2_utf8(param, APP_DEFAULT_CODEPAGE).c_str()
#define U2GBKEX(param, codePage)        param
#define U2GBK(param)                    param
#define GBK2UEX(param, codePage)        Utility::gbk_2_unicode(param, codePage).c_str()
#define GBK2U(param)                    Utility::gbk_2_unicode(param, APP_DEFAULT_CODEPAGE).c_str()
#define GBK2UTF8EX(param, codePage)     Utility::gbk_2_utf8(param, codePage).c_str()
#define GBK2UTF8(param)                 GBK2UTF8EX(param, APP_DEFAULT_CODEPAGE)


#define UW2T        Utility::CW2A
#define UW2TEX      Utility::CW2AEX
#define UW2CT       Utility::CW2A
#define UW2CTEX     Utility::CW2AEX
#define UT2W        Utility::CA2W
#define UT2WEX      Utility::CA2WEX
#define UT2CW       Utility::CA2W
#define UT2CWEX     Utility::CA2WEX

#define UA2T        Utility::CA2A
#define UA2TEX      Utility::CA2AEX
#define UA2CT       Utility::CA2CA
#define UA2CTEX     Utility::CA2CAEX
#define UT2A        Utility::CA2A
#define UT2AEX      Utility::CA2AEX
#define UT2CA       Utility::CA2CA
#define UT2CAEX     Utility::CA2CAEX
#endif // _UNICODE


#endif /*__convert_H__*/