#pragma once
//#include <Windows.h>
#include <windef.h>
#include <xstring>
#include <iomanip>
#include <tchar.h>
#include <vector>
//#include <verrsrc.h>
#if _MSC_VER >= 1000  
#pragma once  
#endif // _MSC_VER >= 1000  

using namespace std;

class CFileVersion  
{   
// Construction  
public:   
    CFileVersion();  
// Operations     
public:   
    BOOL    Open(LPCTSTR lpszModuleName);  
    void    Close();  
    wstring QueryValue(LPCTSTR lpszValueName, DWORD dwLangCharset = 0); 

    wstring GetFileDescription()  
    {
        return QueryValue(_T("FileDescription")); 
    }

    wstring GetFileVersion()      
    {
        return QueryValue(_T("FileVersion"));     
    };  

    wstring GetInternalName()     
    {
        return QueryValue(_T("InternalName"));    
    };  

    wstring GetCompanyName()      
    {
        return QueryValue(_T("CompanyName"));     
    };   

    wstring GetLegalCopyright()   
    {
        return QueryValue(_T("LegalCopyright"));  
    };  

    wstring GetOriginalFilename() 
    {
        return QueryValue(_T("OriginalFilename"));
    };  

    wstring GetProductName()      
    {
        return QueryValue(_T("ProductName"));     
    };  

    wstring GetProductVersion()   
    {
        return QueryValue(_T("ProductVersion"));  
    };  

    BOOL    GetFixedInfo(VS_FIXEDFILEINFO& vsffi);  
    wstring GetFixedFileVersion();  
    wstring GetFixedProductVersion();  
// Attributes  
protected:  
    LPBYTE  m_lpVersionData;   
    std::vector<DWORD>   m_dwLangCharset;   
// Implementation  
public:  
    ~CFileVersion();   
};   


