#pragma once

#include <shlobj.h>
#include <io.h>
#include "autoinitcom.h"
#include "atlsecurity.h"
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Version.lib")
#include <shellapi.h>
#pragma comment(lib, "shell32.lib")

#ifndef __cplusplus_cli
#include <filesystem>
#include <regex>
namespace fs = std::filesystem;
#define ADVANCE_API
#endif

#include <string>
#include <vector>
#include <unordered_map>
#include "StringUtility.h"
using namespace std;

#define FileInfo_Description L"FileDescription"
#define FileInfo_ProductName L"ProductName"
#define FileInfo_CompanyName L"CompanyName"




class CFileInfo
{
public:

	BOOL GetCurrentProcessPath( WCHAR *filepath, ULONG length )
	{
		if (filepath != NULL)
		{
			HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId());
			if (hProcess != NULL)
			{
				DWORD dwLen = length;
				BOOL bRet = ::QueryFullProcessImageName(hProcess, 0, filepath, &dwLen);
				::CloseHandle(hProcess);
				return bRet;
			}
		}

		return FALSE;
	}

	//CSIDL_SYSTEM CSIDL_WINDOWS CSIDL_PROGRAM_FILES
	wstring GetLocalPath(int type)
	{
		wchar_t wcDefaultDir[MAX_PATH] = { 0 };
		if (!SHGetSpecialFolderPath(NULL, wcDefaultDir, type, false))
			return L"";
		return wcDefaultDir;
	}

	void GetFiles(LPCTSTR pszFolder,vector<CString>  &VectorFiles)
	{
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = INVALID_HANDLE_VALUE;

		CString strFolder(pszFolder);
		strFolder.Append(_T("\\*"));

		hFind = FindFirstFile(strFolder, &FindFileData);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (_tcsicmp(_T("."), FindFileData.cFileName) == 0
					|| _tcsicmp(_T(".."), FindFileData.cFileName) == 0)
				{
					continue;
				}
				if (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
				{
					CString strResult = pszFolder;
					LPCTSTR pszResult;
					LPCTSTR pchLast;
					pszResult = strResult;
					pchLast = _tcsdec(pszResult, pszResult + strResult.GetLength());
					ATLASSERT(pchLast != NULL);
					if ((*pchLast != _T('\\')) && (*pchLast != _T('/')))
						strResult += _T('\\');
					strResult += FindFileData.cFileName;

					GetFiles(strResult, VectorFiles);
				}
				else
				{
					CString strInstallManagerPath;
					CString strResult = pszFolder;
					LPCTSTR pszResult;
					LPCTSTR pchLast;
					pszResult = strResult;
					pchLast = _tcsdec(pszResult, pszResult + strResult.GetLength());
					ATLASSERT(pchLast != NULL);
					if ((*pchLast != _T('\\')) && (*pchLast != _T('/')))
						strResult += _T('\\');
					strResult += FindFileData.cFileName;

					VectorFiles.push_back(strResult);
				}

			} while (FindNextFile(hFind, &FindFileData) != 0);
			FindClose(hFind);
		}
	}

	BOOL GetFileShortName(LPCTSTR lpFileName, CString &shortname,BOOL isWithExt=TRUE)
	{
		if (NULL == lpFileName)
			return FALSE;

		CString exefilename;
		if (!GetFileFullPath(lpFileName, exefilename))
			return FALSE;
		WCHAR fileshortname[_MAX_FNAME] = {0};
		WCHAR filename[_MAX_FNAME] = {0};
		WCHAR fileext[_MAX_EXT] = {0};
		_wsplitpath_s(exefilename.GetBuffer(),
			NULL,
			0,
			NULL,
			0,
			filename,
			_MAX_FNAME,
			fileext,
			_MAX_EXT);
		if (isWithExt==FALSE)
			_wmakepath_s(fileshortname, NULL, NULL, filename, NULL);
		else
			_wmakepath_s(fileshortname, NULL, NULL, filename, fileext);
		shortname = fileshortname;

		return TRUE;
	}

	static BOOL IsLnkPostFile(LPCTSTR lpFileName)
	{
		LPCTSTR		lpPostFix = GetFilePostFix(lpFileName);

		if (lpPostFix == NULL)
			return FALSE;

		return (_tcsicmp(lpPostFix, _T("lnk")) == 0);
	}

	static BOOL QueryLnk(LPCTSTR lpLinkPath, CString &strExePath, CString &strExeParam)
	{
		IShellLink*	pShellLink;
		BOOL bReturn;

		CAutoCoInit	autoCO(NULL, COINIT_MULTITHREADED);
		{
			bReturn = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
				IID_IShellLink, (void **)&pShellLink) >= 0;

			if (bReturn)
			{
				IPersistFile *ppf;
				bReturn = pShellLink->QueryInterface(IID_IPersistFile, (void **)&ppf) >= 0;
				if (bReturn)
				{
					bReturn = ppf->Load(lpLinkPath, FALSE) >= 0;
					if (bReturn)
					{
						CString		strIePath;
						CString		strParam;
						pShellLink->GetPath(strIePath.GetBuffer(MAX_PATH + 1), MAX_PATH, NULL, SLGP_UNCPRIORITY);
						pShellLink->GetArguments(strParam.GetBuffer(MAX_PATH + 1), MAX_PATH);
						strIePath.ReleaseBuffer();
						strParam.ReleaseBuffer();

						if (!strIePath.IsEmpty())
						{
							strExePath = (LPCTSTR)strIePath;

							if (!strParam.IsEmpty())
								strExeParam = strParam;

							bReturn = TRUE;
						}
						else
							bReturn = FALSE;
					}
					ppf->Release();
				}
				pShellLink->Release();
			}
		}

		return bReturn;
	}

	

	BOOL GetFileFullPath(LPCTSTR lpFileName, CString &exepath)
	{
		int splitid  = 0;
		CString lowstr;
		if (NULL == lpFileName)
			return FALSE;

		//LPCTSTR lpstr = _tcsrchr(lpFileName, _T('\:'));

		//if (lpstr == NULL)
		//	return false;

		//lowstr = lpstr - 1;
		lowstr = lpFileName;

		exepath = lowstr.MakeLower();

		splitid = exepath.Find(L".exe",0);

		if (splitid == -1)
			splitid = exepath.Find(L".dll");

		if (splitid == -1)
			splitid = exepath.Find(L".cmd");

		if (splitid == -1)
			splitid = exepath.Find(L".bat");

		if (splitid == -1)
			splitid = exepath.Find(L".lnk");

		if (splitid == -1)
			splitid = exepath.Find(L".msi");

		if (splitid == -1)
			return FALSE;

		exepath.Delete(splitid+4, exepath.GetLength() - splitid - 4);

		if (exepath.GetAt(0) == L'\"')
			exepath.Delete(0,1);
		if (exepath.GetAt(0) == L'\\')
			exepath.Delete(0, 1);
		return TRUE;
	
	}


	
	// Get File Description ;;_FilePathName = full path name ,_strRetDescript =  File Description 
	static BOOL GetFileDescription(CString _strFilePathName,CString &_strRetDescript)
	{
		DWORD dwRet = 0;   
		DWORD InfoSize = 0;
		WCHAR *pValue = NULL;
		UINT ValueLen;
		struct LANGANDCODEPAGE {	//  get multi language 
			WORD wLanguage;
			WORD wCodePage;
		} *lpTranslate;
		CString FileInfo = L"\\StringFileInfo\\";

		InfoSize = ::GetFileVersionInfoSize(_strFilePathName, &dwRet);
		if (InfoSize > 0)
		{ 
			BYTE *VersionInfo=new BYTE[InfoSize ];

			if(  ::GetFileVersionInfo(_strFilePathName,dwRet,InfoSize,VersionInfo)) // Get version success
			{ 

				//default 0x0804 \StringFileInfo\lang-codepage\string-name
				if (!::VerQueryValue(VersionInfo, L"\\VarFileInfo\\Translation", (LPVOID *)&lpTranslate, &ValueLen))
				{
					delete[] VersionInfo;
					return FALSE;
				}

				if ( lpTranslate != NULL &&  (ValueLen / sizeof(LANGANDCODEPAGE) >= 1) )
				{
					// so important
					FileInfo.Format( L"\\StringFileInfo\\%04x%04x\\FileDescription",lpTranslate[0].wLanguage , lpTranslate[0].wCodePage );
					if (::VerQueryValue(VersionInfo, FileInfo, (LPVOID *)&pValue, &ValueLen)) // get info
					{
						_strRetDescript = pValue; // software info 
						delete[] VersionInfo;
						return TRUE;
					}
				}
			}
			delete[] VersionInfo;
		}


		return FALSE;
	}


	// Get File Product Description ;;_FilePathName = full path name ,_strRetDescript =  File Product Description 
	BOOL GetFileProductName(CString _strFilePathName, CString &_strRetDescript)
	{
		DWORD dwRet = 0;
		DWORD InfoSize = 0;
		WCHAR *pValue = NULL;
		UINT ValueLen;
		struct LANGANDCODEPAGE {	//  get multi language 
			WORD wLanguage;
			WORD wCodePage;
		} *lpTranslate;
		CString FileInfo = L"\\StringFileInfo\\";

		InfoSize = ::GetFileVersionInfoSize(_strFilePathName, &dwRet);
		if (InfoSize > 0)
		{
			BYTE *VersionInfo = new BYTE[InfoSize];
			if (::GetFileVersionInfo(_strFilePathName, dwRet, InfoSize, VersionInfo)) // Get version success
			{

				//default 0x0804 \StringFileInfo\lang-codepage\string-name
				if (!::VerQueryValue(VersionInfo, L"\\VarFileInfo\\Translation", (LPVOID *)&lpTranslate, &ValueLen))
				{
					delete[] VersionInfo;
					return FALSE;
				}

				if (lpTranslate != NULL && (ValueLen / sizeof(LANGANDCODEPAGE) >= 1))
				{
					// so important
					FileInfo.Format(L"\\StringFileInfo\\%04x%04x\\ProductName", lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
					if (::VerQueryValue(VersionInfo, FileInfo, (LPVOID *)&pValue, &ValueLen)) // get info
					{
						_strRetDescript = pValue; // software info 
						delete[] VersionInfo;
						return TRUE;
					}
				}
			}
			delete[] VersionInfo;
		}


		return FALSE;
	}

	// Get File Description ;;_FilePathName = full path name ,_strRetDescript =  File Description 
	BOOL GetFileCompanyName(CString _strFilePathName, CString &_strRetCompanyName)
	{
		DWORD dwRet = 0;
		DWORD InfoSize = 0;
		WCHAR *pValue = NULL;
		UINT ValueLen;
		struct LANGANDCODEPAGE {	//  get multi language 
			WORD wLanguage;
			WORD wCodePage;
		} *lpTranslate;
		CString FileInfo = L"\\StringFileInfo\\";

		InfoSize = ::GetFileVersionInfoSize(_strFilePathName, &dwRet);
		if (InfoSize > 0)
		{
			BYTE *VersionInfo = new BYTE[InfoSize];
			if (::GetFileVersionInfo(_strFilePathName, dwRet, InfoSize, VersionInfo)) // Get version success
			{

				//default 0x0804 \StringFileInfo\lang-codepage\string-name
				if (!::VerQueryValue(VersionInfo, L"\\VarFileInfo\\Translation", (LPVOID *)&lpTranslate, &ValueLen))
				{
					delete[] VersionInfo;
					return FALSE;
				}

				if (lpTranslate != NULL && (ValueLen / sizeof(LANGANDCODEPAGE) >= 1))
				{
					// so important
					FileInfo.Format(L"\\StringFileInfo\\%04x%04x\\CompanyName", lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
					if (::VerQueryValue(VersionInfo, FileInfo, (LPVOID *)&pValue, &ValueLen)) // get info
					{
						_strRetCompanyName = pValue; // software info 
						delete[] VersionInfo;
						return TRUE;
					}
				}
			}
			delete[] VersionInfo;
		}

		return FALSE;
	}

	BOOL GetAllInfo(CString _strFilePathName, unordered_map<wstring, wstring> &mapInfo)
	{
		mapInfo.clear();
		mapInfo[FileInfo_CompanyName]=L"";
		mapInfo[FileInfo_Description] = L"";
		mapInfo[FileInfo_ProductName] = L"";

		DWORD dwRet = 0;
		DWORD InfoSize = 0;
		WCHAR *pValue = NULL;
		UINT ValueLen;
		struct LANGANDCODEPAGE {	//  get multi language 
			WORD wLanguage;
			WORD wCodePage;
		} *lpTranslate;
		CString FileInfo = L"\\StringFileInfo\\";

		InfoSize = ::GetFileVersionInfoSize(_strFilePathName, &dwRet);
		if (InfoSize > 0)
		{
			BYTE *VersionInfo = new BYTE[InfoSize];
			if (::GetFileVersionInfo(_strFilePathName, dwRet, InfoSize, VersionInfo)) // Get version success
			{

				//default 0x0804 \StringFileInfo\lang-codepage\string-name
				if (!::VerQueryValue(VersionInfo, L"\\VarFileInfo\\Translation", (LPVOID *)&lpTranslate, &ValueLen))
				{
					delete[] VersionInfo;
					return FALSE;
				}

				if (lpTranslate != NULL && (ValueLen / sizeof(LANGANDCODEPAGE) >= 1))
				{
					for (auto & item : mapInfo)
					{
						FileInfo.Format(L"\\StringFileInfo\\%04x%04x\\%s", lpTranslate[0].wLanguage, lpTranslate[0].wCodePage,
							item.first.c_str());
						if (::VerQueryValue(VersionInfo, FileInfo, (LPVOID *)&pValue, &ValueLen)) // get info
						{
							item.second = pValue;
						}
					}			
				}
			}
			delete[] VersionInfo;
		}

		return FALSE;
	}

	static void Split(CString a, CString *b, int &c, CString d)
	{
		int d_len = d.GetLength();
		int j = 0;
		int n = 0;
		int m_pos;
		while (1)
		{
			m_pos = a.Find(d, j);
			if (m_pos == -1 && j == 0)
			{
				c = -1;
				break;
			}

			if (m_pos == -1 && j != 0)
			{
				b[n] = a.Mid(j, a.GetLength() - j);
				c = n;
				break;
			}

			if (j == 0)
			{
				b[n] = a.Mid(0, m_pos);
				j = m_pos + d_len;
			}
			else
			{
				b[n] = a.Mid(j, m_pos - j);
				j = m_pos + d_len;
			}
			n++;
		}
	}

	ULONG GetFilePathLevel(WCHAR* filepath)
	{
		ULONG levelcount = 0;
		CString strfilepath = filepath;
		for (; strfilepath != "";)
		{
			strfilepath = strfilepath.Left(strfilepath.ReverseFind(L'\\'));
			levelcount++;
		}
		return levelcount;
	}

	void SplitFileandPath(WCHAR* lpsPath, WCHAR *filedir, WCHAR *filename, BOOL bwithExt=TRUE)
	{
		WCHAR filedrv[_MAX_DRIVE];
		WCHAR filepath[_MAX_DIR];
		WCHAR filesname[_MAX_FNAME];
		WCHAR fileext[_MAX_EXT];
		_wsplitpath_s(lpsPath,
			filedrv,
			_MAX_DRIVE,
			filepath,
			_MAX_DIR,
			filesname,
			_MAX_FNAME,
			bwithExt?fileext:NULL,
			bwithExt ? _MAX_EXT:0);
		if (filedir)
			_wmakepath_s(filedir, MAX_PATH, filedrv, filepath, NULL, NULL);
		if (filename)
			_wmakepath_s(filename, MAX_PATH, NULL, NULL, filesname, bwithExt?fileext:NULL);

		return;
	}

	template<size_t nSize>
	void ModifyPathSpec(TCHAR(&szDst)[nSize], BOOL  bAddSpec)
	{
		size_t nLen = _tcslen(szDst);
		ATLASSERT(nLen > 0);
		TCHAR  ch = szDst[nLen - 1];
		if ((ch == _T('\\')) || (ch == _T('/')))
		{
			if (!bAddSpec)
			{
				while (ch == _T('\\') || ch == _T('/'))
				{
					szDst[nLen - 1] = _T('\0');
					--nLen;
					ch = szDst[nLen - 1];
				}
			}
		}
		else
		{
			if (bAddSpec)
			{
				szDst[nLen] = _T('\\');
				szDst[nLen + 1] = _T('\0');
			}
		}
	}

	BOOL CreateDirectoryNested(LPCTSTR  lpszDir)
	{
		if (lpszDir == NULL)
		{
			return FALSE;
		}
		if (::PathIsDirectory(lpszDir))
			return TRUE;

		if (_tcslen(lpszDir) >= MAX_PATH)
			return FALSE;

		TCHAR   szPreDir[MAX_PATH];
		_tcscpy_s(szPreDir, lpszDir);
		//Ensure that the path is not the end of the backslash
		ModifyPathSpec(szPreDir, FALSE);

		//Gets the parent directory
		BOOL  bGetPreDir = ::PathRemoveFileSpec(szPreDir);
		if (!bGetPreDir) return FALSE;

		//If the parent directory does not exist, create a parent directory recursively
		if (!::PathIsDirectory(szPreDir))
		{
			CreateDirectoryNested(szPreDir);
		}

		return ::CreateDirectory(lpszDir, NULL);
	}

	string GetSystemAddinDataPath()
	{
		string dbrootpath = "";
		char szPath[MAX_PATH];
		if (SHGetSpecialFolderPathA(NULL, szPath, CSIDL_COMMON_APPDATA, false))
		{
			dbrootpath = szPath;
			dbrootpath += "\\Lenovo\\Vantage\\AddinData\\LenovoGamingSystemAddin";
		}
		return dbrootpath;
	}

	string GetUserAddinDataPath()
	{
		CStringA dbrootpath = "";
		char szPath[MAX_PATH];
		if (SHGetSpecialFolderPathA(NULL, szPath, CSIDL_APPDATA, false))
		{
			dbrootpath = szPath;
			dbrootpath = dbrootpath.Mid(0, dbrootpath.GetLength() - 7);
			dbrootpath += "Local\\Lenovo\\Vantage\\AddinData\\LenovoGamingUserAddin";
		}
		return dbrootpath.GetBuffer();
	}
	vector<string> GetfilelistInDir(string folderpath)
	{
		vector<string> filelist;
		char dir[MAX_PATH] = { 0 };
		char str[] = "\\*.*";
		strcpy_s(dir, MAX_PATH, folderpath.c_str());
		strcat_s(dir, MAX_PATH, str);

		intptr_t hFile;
		_finddata_t fileinfo;
		if ((hFile = _findfirst(dir, &fileinfo)) != -1)
		{
			do
			{
				if (!(fileinfo.attrib & _A_SUBDIR))
				{
					filelist.push_back(fileinfo.name);
				}
				else
				{
					if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					{

					}
				}
			} while (_findnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
			return filelist;
		}
		return {};
	}

	vector<string> GetSubDirlistInDir(string folderpath)
	{
		vector<string> filelist;
		char dir[MAX_PATH] = { 0 };
		char str[] = "\\*.*";
		strcpy_s(dir, MAX_PATH, folderpath.c_str());
		strcat_s(dir, MAX_PATH, str);

		intptr_t hFile;
		_finddata_t fileinfo;
		if ((hFile = _findfirst(dir, &fileinfo)) != -1)
		{
			do
			{
				if ((fileinfo.attrib & _A_SUBDIR)
					&& (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0))
				{
					filelist.push_back(fileinfo.name);
				}
			} while (_findnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
			return filelist;
		}
		return {};
	}

	BOOL CreateDirEx(const char *lpFullPath)
	{
		if (lpFullPath == NULL)
		{
			return FALSE;
		}

		USES_CONVERSION;
		return CreateDirectoryNested(CA2CT(lpFullPath));
	}

	BOOL CreateDirExW(const wchar_t *lpFullPath)
	{
		if (lpFullPath == NULL)
		{
			return FALSE;
		}

		return CreateDirectoryNested(lpFullPath);
	}


	VOID GetSameDirFilePath(WCHAR *filename, WCHAR *filepath)
	{
		WCHAR path_buffer[_MAX_PATH] = { 0 };
		WCHAR drive[_MAX_DRIVE] = { 0 };
		WCHAR dir[_MAX_DIR] = { 0 };
		if (!filename || !filepath)
			return;
		SecureZeroMemory(filepath, sizeof(WCHAR)*MAX_PATH);
		//use QueryFullProcessImageName instead of GetModuleFileName
		auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId());
		if (hProcess == nullptr)
			return;
		unsigned long dwLen = _MAX_PATH;
		QueryFullProcessImageName(hProcess, 0, path_buffer, &dwLen);
		CloseHandle(hProcess);
#pragma warning(suppress: 6102)
		_wsplitpath_s(path_buffer,
			drive,
			_MAX_DRIVE,
			dir,
			_MAX_DIR,
			NULL,
			0,
			NULL,
			0);
		_wmakepath_s(filepath, _MAX_PATH, drive, dir, NULL, NULL);
		wcscat_s(filepath, _MAX_PATH, filename);

	}

	static VOID GetSameModuleFilePath(const WCHAR *filename, WCHAR *filepath)
	{
		WCHAR path_buffer[_MAX_PATH] = { 0 };
		WCHAR drive[_MAX_DRIVE] = { 0 };
		WCHAR dir[_MAX_DIR] = { 0 };
		if (!filename || !filepath)
			return;
		HMODULE hmodule = NULL;				
		PVOID addr = (PVOID)&(GetSameModuleFilePath);
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
							|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
							(WCHAR*)addr,
							&hmodule);
		if (hmodule == NULL)
			return;
		SecureZeroMemory(filepath, sizeof(WCHAR)*MAX_PATH);
		GetModuleFileName(hmodule, path_buffer, _MAX_PATH);
#pragma warning(suppress: 6102)
		_wsplitpath_s(path_buffer,
			drive,
			_MAX_DRIVE,
			dir,
			_MAX_DIR,
			NULL,
			0,
			NULL,
			0);
		_wmakepath_s(filepath, _MAX_PATH, drive, dir, NULL, NULL);
		wcscat_s(filepath, _MAX_PATH, filename);

	}

	static string GetCurrentModuleName()
	{
		CHAR path_buffer[_MAX_PATH] = { 0 };
		CHAR modulename[_MAX_FNAME] = { 0 };
		HMODULE hmodule = NULL;
		PVOID addr = (PVOID)&(GetCurrentModuleName);
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
			| GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			(WCHAR*)addr,
			&hmodule);
		if (hmodule == NULL)
			return "";
		GetModuleFileNameA(hmodule, path_buffer, _MAX_PATH);
#pragma warning(suppress: 6102)
		_splitpath_s(path_buffer,
			NULL,
			0,
			NULL,
			0,
			modulename,
			_MAX_FNAME,
			NULL,
			0);
		return modulename;
	}

	static bool GetCurrentModuleFilePath(string &infilepath)
	{
		WCHAR path_buffer[_MAX_PATH] = { 0 };
		HMODULE hmodule = NULL;

		PVOID addr = (PVOID)&(GetSameModuleFilePath);
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
			| GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			(WCHAR*)addr,
			&hmodule);
		if (hmodule == NULL)
			return false;
		GetModuleFileName(hmodule, path_buffer, _MAX_PATH);
		wstring ws_path = path_buffer;
		Wchar_tToUtf8(ws_path.c_str(),infilepath);
		return true;
	}
	static VOID GetCurrentModuleFilePath(WCHAR *filepath)
	{
		WCHAR path_buffer[_MAX_PATH] = { 0 };
		HMODULE hmodule = NULL;
		if (filepath == NULL)
			return;
		PVOID addr = (PVOID)&(GetSameModuleFilePath);
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
			| GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			(WCHAR*)addr,
			&hmodule);
		if (hmodule == NULL)
			return;
		SecureZeroMemory(filepath, sizeof(WCHAR)*MAX_PATH);
		GetModuleFileName(hmodule, path_buffer, _MAX_PATH);
		wcscpy_s(filepath, _MAX_PATH,path_buffer);

	}

	VOID GetModuleDirFilePath(WCHAR *filename, WCHAR* modulename, WCHAR *filepath)
	{
		WCHAR path_buffer[_MAX_PATH] = { 0 };
		WCHAR drive[_MAX_DRIVE] = { 0 };
		WCHAR dir[_MAX_DIR] = { 0 };
		if (!filename || !filepath)
			return;
		SecureZeroMemory(filepath, sizeof(WCHAR)*MAX_PATH);
		HMODULE hmodule =  GetModuleHandle(modulename);
		if (hmodule == NULL)
			return;
		GetModuleFileName(hmodule, path_buffer, _MAX_PATH);
#pragma warning(suppress: 6102)
		_wsplitpath_s(path_buffer,
			drive,
			_MAX_DRIVE,
			dir,
			_MAX_DIR,
			NULL,
			0,
			NULL,
			0);
		_wmakepath_s(filepath, _MAX_PATH, drive, dir, NULL, NULL);
		wcscat_s(filepath, _MAX_PATH, filename);

	}

#ifdef ADVANCE_API

	list<wstring> SplitPath2ListW(wstring fullpath)
	{
		fs::path path = fullpath;
		string drv = path.root_path().string();
		fs::path relatedpath = path.relative_path().string();
	
		list<wstring> ptinfo;	

		while (relatedpath.string()!="")
		{
			ptinfo.push_front(relatedpath.filename());
			relatedpath = relatedpath.parent_path();
		}
	
		return ptinfo;
	}

	list<string> SplitPath2ListA(string fullpath)
	{
		fs::path path = fullpath;
		string drv = path.root_path().string();
		fs::path relatedpath = path.relative_path().string();

		list<string> ptinfo2;

		while (relatedpath.string() != "")
		{
			ptinfo2.push_front(relatedpath.filename().string());
			relatedpath = relatedpath.parent_path();
		}

		string filename = path.filename().string();
		string searchstr = relatedpath.string();
		std::regex r(R"([^\0-9a-zA-Z])");
		std::smatch sm;
		list<string> ptinfo;
		while (regex_search(searchstr, sm, r))
		{
			ptinfo.push_back(sm.prefix().str());
			searchstr = sm.suffix();
		}
		ptinfo.push_back(filename);


		if (ptinfo != ptinfo2)
		{
			;/// DBG_PRINTF_FL((L"slit path result not the same.\n"));
		}
		return ptinfo2;
	}
#endif

protected:
	static LPCTSTR GetFilePostFix(LPCTSTR lpFile)
	{
		LPCTSTR lpPostFix = _tcsrchr(lpFile, _T('.'));
		if (lpPostFix != NULL)
			lpPostFix++;
		return lpPostFix;
	}
};


