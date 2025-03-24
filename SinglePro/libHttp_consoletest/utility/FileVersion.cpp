//#include "stdafx.h"
#include <Windows.h>
#include <winver.h> 
#include "FileVersion.h"  
#include <sstream>
#include <atlstr.h>


#pragma comment(lib, "version")  

CFileVersion::CFileVersion() 
	:m_dwLangCharset{0,0x040904B0,0x40904E4 ,0x04090000 }
{   
    m_lpVersionData = NULL;  
    //m_dwLangCharset = 0;  
}  
CFileVersion::~CFileVersion()   
{   
    Close();  
}   
void CFileVersion::Close()  
{  
    delete[] m_lpVersionData;   
    m_lpVersionData = NULL;  
    m_dwLangCharset[0] = 0;  
}  
BOOL CFileVersion::Open(LPCTSTR lpszModuleName)  
{  
     
    // Get the version information size for allocate the buffer  
    DWORD dwHandle;       
    DWORD dwDataSize = ::GetFileVersionInfoSize((LPTSTR)lpszModuleName, &dwHandle);   
    if ( dwDataSize == 0 )   
        return FALSE;  
    // Allocate buffer and retrieve version information  
    m_lpVersionData = new BYTE[dwDataSize];   
    if (!::GetFileVersionInfo((LPTSTR)lpszModuleName, dwHandle, dwDataSize,   
                              (void**)m_lpVersionData) )  
    {  
        Close();  
        return FALSE;  
    }  
    // Retrieve the first language and character-set identifier  
    UINT nQuerySize;  
    DWORD* pTransTable;  
    if (!::VerQueryValue(m_lpVersionData, _T("\\VarFileInfo\\Translation"),  
                         (void **)&pTransTable, &nQuerySize) )  
    {  
        Close();  
        return FALSE;  
    }  
    // Swap the words to have lang-charset in the correct format  
    m_dwLangCharset[0] = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));  
    return TRUE;  
}  
wstring CFileVersion::QueryValue(LPCTSTR lpszValueName,   
                                 DWORD dwLangCharset /* = 0*/)  
{  
    // Must call Open() first    
    if ( m_lpVersionData == NULL )  
        return (wstring)_T("");  
  
    // Query version information value  
    UINT nQuerySize;  
    LPVOID lpData;  
    wstring strValue, strBlockName; 
	if (dwLangCharset)
	{
		wstringstream ss;
		ss << _T("\\StringFileInfo\\") << std::setfill(L'0') << std::setw(8) << std::hex << dwLangCharset << "\\" << lpszValueName;
		strBlockName = ss.str();
		if (::VerQueryValue((void **)m_lpVersionData, strBlockName.c_str(), &lpData, &nQuerySize))
		{
			strValue = (LPCTSTR)lpData;
			strValue = CString(strValue.c_str()).Trim();
		}
	}
	else
	{
		for (auto &item : m_dwLangCharset)
		{
			wstringstream ss;
			ss << _T("\\StringFileInfo\\") << std::setfill(L'0') << std::setw(8) << std::hex << item << "\\" << lpszValueName;
			strBlockName = ss.str();
			if (::VerQueryValue((void **)m_lpVersionData, strBlockName.c_str(), &lpData, &nQuerySize))
			{
				strValue = (LPCTSTR)lpData;
				strValue = CString(strValue.c_str()).Trim();
				break;
			}
		}
	}
	return strValue;
}  
BOOL CFileVersion::GetFixedInfo(VS_FIXEDFILEINFO& vsffi)  
{  
    // Must call Open() first  
    
    if ( m_lpVersionData == NULL )  
        return FALSE;  
    UINT nQuerySize;  
    VS_FIXEDFILEINFO* pVsffi;  
    if ( ::VerQueryValue((void **)m_lpVersionData, _T("\\"),  
                         (void**)&pVsffi, &nQuerySize) )  
    {  
        vsffi = *pVsffi;  
        return TRUE;  
    }  
    return FALSE;  
}  
wstring CFileVersion::GetFixedFileVersion()  
{  
    wstring strVersion;  
    VS_FIXEDFILEINFO vsffi;  
    if ( GetFixedInfo(vsffi) )  
    {  
		wstringstream ss;
		ss<<(DWORD)HIWORD(vsffi.dwFileVersionMS)<<","
            <<(DWORD)LOWORD(vsffi.dwFileVersionMS)<<","
            <<(DWORD)HIWORD(vsffi.dwFileVersionLS)<<","  
            <<(DWORD)LOWORD(vsffi.dwFileVersionLS);
		strVersion = ss.str();
    }  
    return strVersion;  
}  
wstring CFileVersion::GetFixedProductVersion()  
{  
    wstring strVersion;  
    VS_FIXEDFILEINFO vsffi;  
    if ( GetFixedInfo(vsffi) )  
    {  
		wstringstream ss;
       ss<<(DWORD)HIWORD(vsffi.dwProductVersionMS)<<","  
           <<(DWORD)LOWORD(vsffi.dwProductVersionMS)<<","
            <<(DWORD)HIWORD(vsffi.dwProductVersionLS)<<","
            <<(DWORD)LOWORD(vsffi.dwProductVersionLS);  
		strVersion = ss.str();
    }  
    return strVersion;  
}  
  
