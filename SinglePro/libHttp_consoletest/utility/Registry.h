#pragma once
#include <atlstr.h>
//#include <atlbase.h>
#include <map>
#include <vector>
using namespace std;
class CRegHelper
{
public:
	CRegHelper(void);
	virtual ~CRegHelper(void);
	
	//static BOOL GetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, CString& szRegValue);
	static BOOL GetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, CString& szRegValue, BOOL bUseWOW = FALSE);
	static BOOL GetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, CString& szRegValue, DWORD& dwResutl, BOOL bUseWOW);

	static BOOL GetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, DWORD& dwRegValue, BOOL bUseWOW = FALSE);
	//static BOOL GetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, DWORD& dwRegValue, DWORD& dwResult);
	static BOOL GetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, DWORD64& dwRegValue, BOOL bUseWOW = FALSE);
	static BOOL GetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, LPVOID pRegValue, DWORD& dwSize);

	//regPath:HKEY_LOCAL_MACHINE\\SOFTWARE\\Lenovo\\SLBrowser\\exe
	static BOOL GetRegistryString(CString regPath, CString& result, bool wow = false);
	static BOOL GetRegistryInt(CString regPath, DWORD64& value, bool wow = false);


	static BOOL SetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, CString szRegValue, DWORD dwDataType = REG_SZ, BOOL bUseWOW64 = FALSE);
	static BOOL SetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, DWORD dwRegValue, BOOL bUseWOW64 = FALSE);
	static BOOL SetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, DWORD64 dwRegValue, bool bUseWOW = FALSE);
	static BOOL SetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, LPVOID pRegValue, DWORD dwSize);
	static BOOL SetRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, BOOL bRegValue);



	//for  current use------------------------------------------------------------------------------------------------------------


	static BOOL GetCurrentUserRegistryValue(const CString& szRegKeyPath, const CString& szRegKeyName, CString &szRegValue);	//@002A
	static BOOL GetCurrentUserRegistryValue(const CString& szRegKeyPath, const CString& szRegKeyName, DWORD& dwRegValue);	//@002A

	//界面程序使用
	static BOOL GetUserRegValue(LPCTSTR lpKeyName, LPCTSTR lpValueName, DWORD& dwValue);
	static BOOL SetUserRegValue(LPCTSTR lpKeyName, LPCTSTR lpValueName, DWORD dwValue);
	static BOOL GetUserRegValueS(LPCTSTR lpKeyName, LPCTSTR lpValueName, CString& szValue);
	
	

	static BOOL DeleteRegistryKey(HKEY hKey, const CString& szRegKeyPath, const CString& szKeyName, DWORD dwOptions= KEY_WRITE);
	static BOOL RegDeleteKeyRecursively(HKEY  hKey, LPCTSTR pszSubKey, REGSAM samWowFlag);
	static BOOL RegDeleteKeyRec(HKEY  hKey, LPCTSTR pszSubKey, BOOL bUseWOW64= FALSE);
	static BOOL DeleteRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szValueName,BOOL bWow64=FALSE);
	static BOOL DeleteRegistryValueEx(HKEY hKey, const CString& strRegKeyPath, const CString& strValueName, BOOL bWow64 = FALSE);
	static BOOL RenameRegistryKey(HKEY hkeySource, const CString& szSrcRegKeyPath, const CString& szSrcRegKeyName, const CString& szDstRegKeyName, DWORD dwOptions);


	static DWORD EnumSubValues(HKEY keyDomain, const CString& szRegKeyPath, std::map<CString, CString>& subValues, bool bWow64 = false);
	static DWORD EnumSubKeys(HKEY keyDomain, const CString& szRegKeyPath, std::vector<CString>& subKeys, bool bWow64 = false);

	//重定向 add by Y.K
	static BOOL SetSamRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, CString szRegValue, REGSAM dwSamEx);
	static BOOL GetSamRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, CString& szRegValue, REGSAM dwSamEx);
	static BOOL SetSamRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, DWORD dwRegValue, REGSAM dwSamEx);
	static BOOL GetSamRegistryValue(HKEY hKey, const CString& szRegKeyPath, const CString& szRegKeyName, DWORD& dwRegValue, REGSAM dwSamEx);




	//for  service use------------------------------------------------------------------------------------------------------------
	//服务程序使用...
	static BOOL GetCurUserRegValue(LPCTSTR lpKeyName, LPCTSTR lpValueName, DWORD& dwValue);
	static BOOL SetCurUserRegValue(LPCTSTR lpKeyName, LPCTSTR lpValueName, DWORD dwValue);
	static BOOL GetCurUserRegValueS(LPCTSTR lpKeyName, LPCTSTR lpValueName, CString& szValue);
	static BOOL SetCurUserRegValueS(LPCTSTR lpKeyName, LPCTSTR lpValueName, CString szValue);

	static BOOL GetUserSid(LPTSTR AccountName, PSID *ppSid);
	static void	 GetSidString(PSID pSid, CString& strSid);
	
	
};

