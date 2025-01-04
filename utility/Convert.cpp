//#include "stdafx.h"
#include "convert.h"
#include <windows.h>

namespace Utility
{
int wchar_2_multibyte(const wchar_t* pwszInput, std::string& strOut, int nCodePage/*=APP_DEFAULT_CODEPAGE*/)
{
    strOut.clear();

    if (NULL == pwszInput)
        return -1;

    char* str = NULL;
    int input_len = WideCharToMultiByte(nCodePage, 0, pwszInput, -1,
        str, 0, NULL, NULL);

    if (0 >= input_len)
    {
        return -2;
    }

    strOut.resize(input_len+1);
    str = (char*)strOut.data();
    memset(str, 0, input_len);

    if (0 == WideCharToMultiByte(nCodePage, 0, pwszInput, -1, str,
        input_len, NULL, FALSE))
    {
        return -3;
    }
    return 0;
}

int multibyte_2_wchar(const char* pszInput, std::wstring& wstrOut, int nCodePage/*=APP_DEFAULT_CODEPAGE*/)
{
    wstrOut.clear();

    if (NULL==pszInput)
        return -1;

    wchar_t* str_w = NULL;
    int input_len = MultiByteToWideChar(nCodePage, 0, pszInput, -1,str_w, 0);

    if (0 >= input_len)
    {
        return -2;
    }

    wstrOut.resize(input_len+1);
    str_w = (wchar_t*)wstrOut.data();
    memset(str_w, 0, sizeof(wchar_t)*(input_len));

    if (0 == MultiByteToWideChar(nCodePage, 0, pszInput, -1, str_w, input_len))
    {
        return -3;
    }
    return 0;
}

int wchar_2_wchar(const wchar_t* pszInput, std::wstring& wstrOut, int nCodePage/*=APP_DEFAULT_CODEPAGE*/)
{
    if (NULL == pszInput)
    {
        return -1;
    }
    if (pszInput != wstrOut.c_str())
    {
        wstrOut = pszInput;
    }
    return 0;
}

int multibyte_2_multibyte(const char* pszInput, std::string& strOut, int nCodePage/*=APP_DEFAULT_CODEPAGE*/)
{
    if (NULL == pszInput)
    {
        return -1;
    }
    if (pszInput != strOut.c_str())
    {
        strOut = pszInput;
    }
    return 0;
}

int gbk_2_utf8(const std::string& strSrc,std::string& strDest, int nCodePage/*=0 APP_DEFAULT_CODEPAGE*/)
{
    const char* src = strSrc.c_str();
    wchar_t * lpUnicodeStr = NULL;
    int nRetLen = 0;
    if(!src)  //如果GBK字符串为NULL则出错退出
    {
        return false;
    }
    nRetLen = ::MultiByteToWideChar(nCodePage,0,(char *)src,-1,NULL,NULL);  //获取转换到Unicode编码后所需要的字符空间长度
    lpUnicodeStr = new WCHAR[nRetLen + 1];  //为Unicode字符串空间
    nRetLen = ::MultiByteToWideChar(nCodePage,0,(char *)src,-1,lpUnicodeStr,nRetLen);  //转换到Unicode编码
    if(!nRetLen)  //转换失败则出错退出
    {
        return false;
    }

    nRetLen = ::WideCharToMultiByte(CP_UTF8,0,lpUnicodeStr,-1,NULL,0,NULL,NULL);  //获取转换到UTF8编码后所需要的字符空间长度

    strDest.resize(nRetLen + 1);

    nRetLen = ::WideCharToMultiByte(CP_UTF8,0,lpUnicodeStr,-1,(char *)strDest.c_str(), nRetLen, NULL, NULL);  //转换到UTF8编码

    if(lpUnicodeStr)
        delete []lpUnicodeStr;
    return true;
}

std::string gbk_2_utf8(const std::string& strSrc, int nCodePage/*=0 APP_DEFAULT_CODEPAGE*/)
{
    std::string strDest;
    gbk_2_utf8(strSrc, strDest, nCodePage);
    return strDest;
}

std::string gbk_2_utf8(const char* pszSrc, int nCodePage/*=0 APP_DEFAULT_CODEPAGE*/)
{
    if (pszSrc)
    {
        return gbk_2_utf8(std::string(pszSrc), nCodePage);
    }
    return "";
}

int utf8_2_gbk(const std::string& strSrc, std::string& strDest, int nCodePage/*=0 APP_DEFAULT_CODEPAGE*/)
{
    WCHAR *strTemp;
    char *szRes;
    int len;
    //获得临时变量的大小
    int i = MultiByteToWideChar(CP_UTF8, 0, strSrc.c_str(), -1, NULL, 0);
    if (i <=0)
    {
        return 0;
    }

    strTemp = new WCHAR[i+1];
    MultiByteToWideChar(CP_UTF8, 0, strSrc.c_str(), -1, strTemp, i);
    //获得临时变量的大小
    i = WideCharToMultiByte(nCodePage, 0, strTemp, -1, NULL, 0, NULL, NULL);
    if (i <= 0)
    {
        delete[]strTemp;
        return 0;
    }
    szRes = new char[i+1];
    WideCharToMultiByte(nCodePage, 0, strTemp, -1, szRes, i, NULL, NULL);
    len = (i+1)*sizeof(CHAR);
    strDest = szRes;
    delete []strTemp;
    delete []szRes;
    return len;
}

std::string utf8_2_gbk(const std::string& strSrc, int nCodePage/*=0 APP_DEFAULT_CODEPAGE*/)
{
    std::string strDest;
    if (strSrc.empty())
    {
        return strDest;
    }
    utf8_2_gbk(strSrc, strDest, nCodePage);
    return strDest;
}

std::string utf8_2_gbk(const char* pszSrc, int nCodePage/*=0 APP_DEFAULT_CODEPAGE*/)
{
    if (pszSrc)
    {
        return utf8_2_gbk(std::string(pszSrc), nCodePage);
    }
    return "";
}

std::wstring gbk_2_unicode(const std::string& sInput, int nCodePage/*=0 APP_DEFAULT_CODEPAGE*/)
{
    std::wstring strOutput;
    int nLen = MultiByteToWideChar(APP_DEFAULT_CODEPAGE, 0, sInput.c_str(), -1, NULL, 0);
    strOutput.resize(nLen + 1);
    MultiByteToWideChar(nCodePage, 0, sInput.c_str(), -1, (LPWSTR)strOutput.data(), nLen);
    return strOutput;
}

std::wstring gbk_2_unicode(const char* pszInput, int nCodePage/*=0 APP_DEFAULT_CODEPAGE*/)
{
    if (pszInput)
    {
        return gbk_2_unicode(std::string(pszInput), nCodePage);
    }
    return L"";
}

std::string unicode_2_gbk(const std::wstring& sInput, int nCodePage/*=0 APP_DEFAULT_CODEPAGE*/)
{
    std::string strOutput;
    int nLen = WideCharToMultiByte(nCodePage, 0, sInput.c_str(), -1, NULL, 0, NULL, NULL);
    strOutput.resize(nLen + 1);
    WideCharToMultiByte(nCodePage, 0, sInput.c_str(), -1, (LPSTR)strOutput.data(), nLen, NULL, NULL);
    return strOutput;
}

std::string unicode_2_gbk(const wchar_t* pszInput, int nCodePage/*=0 APP_DEFAULT_CODEPAGE*/)
{
    if (pszInput)
    {
        return unicode_2_gbk(std::wstring(pszInput), nCodePage);
    }
    return "";
}

std::wstring utf8_2_unicode(const std::string& sInput)
{
    std::wstring strOutput;
    DWORD dwLen = MultiByteToWideChar(CP_UTF8, 0, sInput.c_str(), -1, NULL, 0);
    if (dwLen <= 0)
    {
        return L"";
    }
    strOutput.resize(dwLen + 1);
    MultiByteToWideChar(CP_UTF8, 0, sInput.c_str(), -1, (LPWSTR)strOutput.data(), dwLen);
    return strOutput;
}

std::wstring utf8_2_unicode(const char* pszInput)
{
    if (pszInput)
    {
        return utf8_2_unicode(std::string(pszInput));
    }
    return L"";
}

std::string unicode_2_utf8(const std::wstring& sInput)
{
    std::string strOutput;
    int nLen = WideCharToMultiByte(CP_UTF8, 0, sInput.c_str(), -1, NULL, 0, NULL, NULL); 
    strOutput.resize(nLen + 1);
    WideCharToMultiByte (CP_UTF8, 0, sInput.c_str(), -1, (LPSTR)strOutput.data(), nLen, NULL,NULL); 
    return strOutput;
}

std::string unicode_2_utf8(const wchar_t* pszInput)
{
    if (pszInput)
    {
        return unicode_2_utf8(std::wstring(pszInput));
    }
    return "";
}

int atoi(const char* pInput, int nDefault/*=0*/)
{
    int nRet = nDefault;
    if (pInput)
    {
        nRet = ::atoi(pInput);
    }
    return nRet;
}

template <class _CharType>
void utilityConvAllocMemory(_CharType** ppBuff, int nLength, _CharType* pszFixedBuffer,int nFixedBufferLength)
{
    if (NULL==ppBuff || 0>nLength || NULL==pszFixedBuffer)
    {
        return;
    }

    //if buffer malloced, try to realloc.
    if (*ppBuff != pszFixedBuffer)
    {
        if( nLength > nFixedBufferLength )
        {
            _CharType* ppReallocBuf = static_cast< _CharType* >( _recalloc(*ppBuff, nLength,sizeof( _CharType ) ) );
            if (ppReallocBuf == NULL) 
            {
                return;
            }
            *ppBuff = ppReallocBuf;
        } 
        else
        {
            free(*ppBuff);
            *ppBuff=pszFixedBuffer;
        }

    }
    else //Buffer is not currently malloced.
    {
        if( nLength > nFixedBufferLength )
        {
            *ppBuff = static_cast< _CharType* >( calloc(nLength,sizeof( _CharType ) ) );
        } else
        {			
            *ppBuff = pszFixedBuffer;
        }
    }

    if (*ppBuff == NULL)
    {
    }

}

template <class _CharType>
void utilityConvFreeMemory(_CharType* pBuff,_CharType* pszFixedBuffer,int nFixedBufferLength)
{
    (nFixedBufferLength);
    if( pBuff != pszFixedBuffer )
    {
        free( pBuff );
    } 	
#ifdef _DEBUG
    else
    {		
        memset(pszFixedBuffer, 0, nFixedBufferLength*sizeof(_CharType));
    }
#endif

}

CW2A::CW2A(const wchar_t* psz) throw(...)
:m_psz(m_szBuffer)
{
    init(psz, APP_DEFAULT_CODEPAGE);
}

CW2A::CW2A(const wchar_t* psz, unsigned int nCodePage) throw(...)
:m_psz(m_szBuffer)
{
    init(psz, APP_DEFAULT_CODEPAGE);
}

CW2A::~CW2A() throw()
{
    utilityConvFreeMemory(m_psz, m_szBuffer, MAX_BUFF_LENGTH);
}

CW2A:: operator char*() const throw()
{
    return m_psz;
}

void CW2A::init(const wchar_t* pszIn, unsigned int nConvertCodePage) throw(...)
{
    if (NULL==pszIn)
    {
        m_psz = NULL;
        return;
    }
    int nLengthW = lstrlenW(pszIn)+1;
    int nLengthA = 4*nLengthW;
    utilityConvAllocMemory(&m_psz, nLengthA, m_szBuffer, MAX_BUFF_LENGTH);

    BOOL bFailed=(0 == ::WideCharToMultiByte( nConvertCodePage, 0, pszIn, nLengthW, m_psz, nLengthA, NULL, NULL ));
    if (bFailed)
    {
        if (GetLastError()==ERROR_INSUFFICIENT_BUFFER)
        {
            nLengthA = ::WideCharToMultiByte( nConvertCodePage, 0, pszIn, nLengthW, NULL, 0, NULL, NULL );
            utilityConvAllocMemory(&m_psz, nLengthA, m_szBuffer, MAX_BUFF_LENGTH);
            bFailed=(0 == ::WideCharToMultiByte( nConvertCodePage, 0, pszIn, nLengthW, m_psz, nLengthA, NULL, NULL ));
        }			
    }
    if (bFailed)
    {
    }
}

///////////////////////////////////////////////////////////////////////////////
// CA2W
///////////////////////////////////////////////////////////////////////////////
CA2W::CA2W(const char* psz) throw(...) 
:m_psz( m_szBuffer )
{
    init(psz, APP_DEFAULT_CODEPAGE);
}

CA2W::CA2W(const char* psz, unsigned int nCodePage) throw(...) 
:m_psz( m_szBuffer )
{
    init( psz, nCodePage );
}

CA2W::~CA2W() throw()
{
    utilityConvFreeMemory(m_psz, m_szBuffer, MAX_BUFF_LENGTH);
}

void CA2W::init(const char* psz, unsigned int nCodePage) throw(...)
{
    if (psz == NULL)
    {
        m_psz = NULL;
        return;
    }
    int nLengthA = lstrlenA(psz)+1;
    int nLengthW = nLengthA;

    utilityConvAllocMemory(&m_psz,nLengthW,m_szBuffer,MAX_BUFF_LENGTH);

    BOOL bFailed=(0 == ::MultiByteToWideChar(nCodePage, 0, psz, nLengthA, m_psz, nLengthW));
    if (bFailed)
    {
        if (GetLastError()==ERROR_INSUFFICIENT_BUFFER)
        {
            nLengthW = ::MultiByteToWideChar( nCodePage, 0, psz, nLengthA, NULL, 0);
            utilityConvAllocMemory(&m_psz,nLengthW, m_szBuffer, MAX_BUFF_LENGTH);
            bFailed=(0 == ::MultiByteToWideChar(nCodePage, 0, psz, nLengthA, m_psz, nLengthW));
        }			
    }
    if (bFailed)
    {
    }
}

///////////////////////////////////////////////////////////////////////////////
// CW2W
///////////////////////////////////////////////////////////////////////////////
CW2W::CW2W(const wchar_t* psz) throw(...)
:m_psz(m_szBuffer)
{
    init(psz);
}

CW2W::CW2W(const wchar_t* psz, unsigned int nCodePage) throw(...)
:m_psz(m_szBuffer)
{
    init(psz);
}

CW2W::~CW2W()
{
    utilityConvFreeMemory(m_psz, m_szBuffer, MAX_BUFF_LENGTH);
}

void CW2W::init(const wchar_t* psz) throw(...)
{
    if (NULL==psz)
    {
        m_psz = NULL;
        return;
    }
    int nLength = lstrlenW(psz)+1;
    utilityConvAllocMemory(&m_psz, nLength, m_szBuffer, MAX_BUFF_LENGTH);
    memcpy_s(m_psz, nLength*sizeof(wchar_t), psz, nLength*sizeof(wchar_t));
}

CA2A::CA2A(const char* psz) throw(...)
:m_psz(m_szBuffer)
{
    init(psz);
}

CA2A::CA2A(const char* psz, unsigned int nCodePage) throw(...)
:m_psz(m_szBuffer)
{
    init(psz);
}

CA2A::~CA2A()
{
    utilityConvFreeMemory(m_psz, m_szBuffer, MAX_BUFF_LENGTH);
}

void CA2A::init(const char* psz) throw(...)
{
    if (NULL==psz)
    {
        m_psz = NULL;
        return;
    }
    int nLength = strlen(psz)+1;
    utilityConvAllocMemory(&m_psz, nLength, m_szBuffer, MAX_BUFF_LENGTH);
    memcpy_s(m_psz, nLength*sizeof(char), psz, nLength*sizeof(char));
}


CW2AEX::CW2AEX(const wchar_t* psz) throw(...)
:m_psz(m_szBuffer)
{
    init(psz, APP_DEFAULT_CODEPAGE);
}

CW2AEX::CW2AEX(const wchar_t* psz, unsigned int nCodePage) throw(...)
:m_psz(m_szBuffer)
{
    init(psz, nCodePage);
}

CW2AEX::~CW2AEX() throw()
{
    utilityConvFreeMemory(m_psz, m_szBuffer, MAX_BUFF_LENGTH);
}

void CW2AEX::init(const wchar_t* pszIn, unsigned int nConvertCodePage) throw(...)
{
    if (NULL==pszIn)
    {
        m_psz = NULL;
        return;
    }
    int nLengthW = lstrlenW(pszIn)+1;
    int nLengthA = 4*nLengthW;
    utilityConvAllocMemory(&m_psz, nLengthA, m_szBuffer, MAX_BUFF_LENGTH);

    BOOL bFailed=(0 == ::WideCharToMultiByte( nConvertCodePage, 0, pszIn, nLengthW, m_psz, nLengthA, NULL, NULL ));
    if (bFailed)
    {
        if (GetLastError()==ERROR_INSUFFICIENT_BUFFER)
        {
            nLengthA = ::WideCharToMultiByte(nConvertCodePage, 0, pszIn, nLengthW, NULL, 0, NULL, NULL );
            utilityConvAllocMemory(&m_psz, nLengthA, m_szBuffer, MAX_BUFF_LENGTH);
            bFailed=(0 == ::WideCharToMultiByte(nConvertCodePage, 0, pszIn, nLengthW, m_psz, nLengthA, NULL, NULL ));
        }			
    }
    if (bFailed)
    {
    }
}

///////////////////////////////////////////////////////////////////////////////
// CA2WEX
///////////////////////////////////////////////////////////////////////////////
CA2WEX::CA2WEX(const char* psz) throw(...) 
:m_psz( m_szBuffer )
{
    init(psz, APP_DEFAULT_CODEPAGE);
}

CA2WEX::CA2WEX(const char* psz, unsigned int nCodePage) throw(...) 
:m_psz( m_szBuffer )
{
    init( psz, nCodePage );
}

CA2WEX::~CA2WEX() throw()
{
    utilityConvFreeMemory(m_psz, m_szBuffer, MAX_BUFF_LENGTH);
}

void CA2WEX::init(const char* psz, unsigned int nCodePage) throw(...)
{
    if (psz == NULL)
    {
        m_psz = NULL;
        return;
    }
    int nLengthA = lstrlenA(psz)+1;
    int nLengthW = nLengthA;

    utilityConvAllocMemory(&m_psz,nLengthW,m_szBuffer,MAX_BUFF_LENGTH);

    BOOL bFailed=(0 == ::MultiByteToWideChar(nCodePage, 0, psz, nLengthA, m_psz, nLengthW));
    if (bFailed)
    {
        if (GetLastError()==ERROR_INSUFFICIENT_BUFFER)
        {
            nLengthW = ::MultiByteToWideChar( nCodePage, 0, psz, nLengthA, NULL, 0);
            utilityConvAllocMemory(&m_psz,nLengthW, m_szBuffer, MAX_BUFF_LENGTH);
            bFailed=(0 == ::MultiByteToWideChar(nCodePage, 0, psz, nLengthA, m_psz, nLengthW));
        }			
    }
    if (bFailed)
    {
    }
}



typedef unsigned char BYTE;
inline BYTE toHex(const BYTE &x)
{
    return x > 9 ? x -10 + 'A': x + '0';
}

inline BYTE fromHex(const BYTE &x)
{
    return isdigit(x) ? x-'0' : x-'A'+10;
}

bool URLEncode(const char *pszInput, astring &strOutput)
{
    bool bRet = false;
    do 
    {
        strOutput.clear();
        if (NULL==pszInput)
        {
            break;
        }
        std::string strUtf8 = pszInput;
#ifndef _UNICODE
        strUtf8 = GBK2UTF8(pszInput);
#endif
        int nSize = strUtf8.length();
        for (int n=0; n<nSize; ++n)
        {
            if (isalnum((unsigned char)strUtf8[n]) ||
                (strUtf8[n] == '-') ||
                (strUtf8[n] == '_') ||
                (strUtf8[n] == '.') ||
                (strUtf8[n] == '~'))  
            {
                strOutput += strUtf8[n];  
            }
            else if (strUtf8[n] == ' ')
            {
                strOutput += "+";
            }
            else  
            {  
                strOutput += '%';
                strOutput += toHex((unsigned char)strUtf8[n] >> 4);  
                strOutput += toHex((unsigned char)strUtf8[n] % 16);  
            } 
        }
        bRet = true;
    } while (false);
    return bRet;
}

bool URLEncode(const wchar_t *pszInput, astring &strOutput)
{
    if (NULL==pszInput)
    {
        return false;
    }
    astring strTemp;
    wchar_2_multibyte(pszInput, strTemp, APP_DEFAULT_CODEPAGE);
    return URLEncode(strTemp.c_str(), strOutput);
}

//bool URLEncode(const char *pszInput, CString &strOutput)
//{
//    astring strTemp;
//    bool bRet = URLEncode(pszInput, strTemp);
//    strOutput.Empty();
//    if (bRet && !strTemp.empty())
//    {
//        strOutput = UA2CT(strTemp.c_str());
//    }
//    return bRet;
//}
//
//bool URLEncode(const wchar_t *pszInput, CString &strOutput)
//{
//    if (NULL==pszInput)
//    {
//        return false;
//    }
//    astring strTemp;
//    wchar_2_multibyte(pszInput, strTemp, APP_DEFAULT_CODEPAGE);
//    return URLEncode(strTemp.c_str(), strOutput);
//}

bool URLDecode(const char *pszInput, astring &strOutput)
{
    bool bRet = false;
    do 
    {
        if (NULL==pszInput)
        {
            break;
        }
        strOutput.clear();
        int nSize = strlen(pszInput);
        if (0==nSize)
        {
            bRet = true;
            break;
        }
        for (int n=0; n<nSize; ++n)
        {
            BYTE ch = 0;
            if ('%' == pszInput[n])
            {
                ch = (fromHex(pszInput[n+1])<<4);
                ch |= fromHex(pszInput[n+2]);
                n += 2;
            }
            else if ('+' == pszInput[n])
            {
                ch = ' ';
            }
            else 
            {
                ch = pszInput[n];
            }
            strOutput += (char)ch;
        }
        bRet = true;
    } while (false);
    return bRet;
}

bool URLDecode(const wchar_t *pszInput, astring &strOutput)
{
    if (NULL==pszInput)
    {
        return false;
    }
    astring strTemp;
    wchar_2_multibyte(pszInput, strTemp, APP_DEFAULT_CODEPAGE);
    return URLDecode(strTemp.c_str(), strOutput);
}

void ConverUTF8ToGBK(const std::string& str, char* szDst, int32_t bufSize)
{
    if (str.empty() || szDst == nullptr)
    {
        return;
    }
    strncpy_s(szDst, bufSize, UTF82GBK(str), _TRUNCATE);
}

//bool URLDecode(const char *pszInput, CString &strOutput)
//{
//    astring strTemp;
//    bool bRet = URLDecode(pszInput, strTemp);
//    strOutput.Empty();
//    if (bRet && !strTemp.empty())
//    {
//        strOutput = UA2CT(strTemp.c_str());
//    }
//    return bRet;
//}
//
//bool URLDecode(const wchar_t *pszInput, CString &strOutput)
//{
//    if (NULL==pszInput)
//    {
//        return false;
//    }
//    astring strTemp;
//    wchar_2_multibyte(pszInput, strTemp, APP_DEFAULT_CODEPAGE);
//    return URLDecode(strTemp.c_str(), strOutput);
//}
//
//int GetTextEncode(CString strTxtPath)
//{
//	int nType = -1;//error
//	//打开要判断的文件
//	FILE *pFile = NULL;  
//	errno_t dError = _tfopen_s(&pFile,strTxtPath,_T("r"));  
//	if ( 0 != dError )  
//	{  
//		if (pFile)
//		{
//			fclose(pFile);
//		}
//
//		return nType;  
//	}  
//	//这里要注意是用unsigned   char，不然的话读取到的数据会因为溢出而无法正确判断
//	unsigned char chFileFlag[3];
//	fread(chFileFlag,   1,   3,   pFile);
//
//	if(chFileFlag[0]   ==   0xEF   &&   chFileFlag[1]   ==   0xBB   &&   chFileFlag[2]   ==   0xBF)
//		nType = 1;//UTF-8
//	else if (chFileFlag[0]   ==   0xFF   &&   chFileFlag[1]   ==   0xFE)
//		nType = 2;//Unicode
//	else if (chFileFlag[0]   ==   0xFE   &&   chFileFlag[1]   ==   0xFF)
//		nType = 3;//Unicode big endian text
//	else  
//		nType = 4;//ASCII
//	fclose(pFile);
//	return nType;   
//}

/*
金额科学计数法显示标准：
【0元-1万)              显示：****元
【1万-10万）          显示：*.**万
【10万-100万）       显示：**.**万
【100万-1000万）   显示：***.*万
【1000万-1亿）      显示：****万
【1亿-10亿）          显示：*.**亿
【10亿-100亿）      显示：**.**亿
【100亿-1000亿）   显示：***.*亿
【1000亿+）           显示：****亿
*/
//void Double2String(const double& dInput, CString& strOutput, bool bYuan/* = true*/)
//{
//    double dValue = dInput;
//    CString strDecimal(_T(""));
//    if (dInput >= 0 && dInput < 10000)
//    {
//		// zdx 20200326 15:43:23 银科 有的数据单位不是元
//		if (bYuan)
//			strDecimal.Format(_T("%%.%dlf元"), 0);
//		else
//			strDecimal.Format(_T("%%.%dlf"), 0);
//    }
//    else if (dInput >= 10000 && dInput < 100000)
//    {
//        strDecimal.Format(_T("%%.%dlf万"), 2);
//    }
//    else if (dInput >= 100000 && dInput < 1000000)
//    {
//        strDecimal.Format(_T("%%.%dlf万"), 2);
//    }
//    else if (dInput >= 1000000 && dInput < 10000000)
//    {
//        strDecimal.Format(_T("%%.%dlf万"), 1);
//    }
//    else if (dInput >= 10000000 && dInput < 100000000)
//    {
//        strDecimal.Format(_T("%%.%dlf万"), 0);
//    }
//    else if (dInput >= 100000000 && dInput < 1000000000)
//    {
//        strDecimal.Format(_T("%%.%dlf亿"), 2);
//    }
//    else if (dInput >= 1000000000 && dInput < 10000000000)
//    {
//        strDecimal.Format(_T("%%.%dlf亿"), 2);
//    }
//    else if (dInput >= 10000000000 && dInput < 100000000000)
//    {
//        strDecimal.Format(_T("%%.%dlf亿"), 1);
//    }
//    else if (dInput >= 100000000000)
//    {
//        strDecimal.Format(_T("%%.%dlf亿"), 0);
//    }
//
//    while (true)
//    {
//        if (dValue / 10000 < 1)
//        {
//            break;
//        }
//
//        dValue /= 10000;
//    }
//    
//    strOutput.Format(strDecimal, dValue);
//}

}
