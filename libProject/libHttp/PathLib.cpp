#include "pch.h"

#include <strsafe.h>

#include "HttpLib\PathLib.h"

const TCHAR DRIVE_IDENTIFICATION = _T(':');					// 盘符标识符
const TCHAR PATH_SPLIT_CHAR_W	= _T('\\');					// window风格路径分割符
const TCHAR PATH_SPLIT_CHAR_U	= _T('/');					// LINUX风格路径分割符

#define IS_PATH_SPLIT_CHAR(ch)	(((ch) == PATH_SPLIT_CHAR_W) || ((ch) == PATH_SPLIT_CHAR_U))			// 字符是否路径分割符

#define PATH_BUFFER_SIZE		(1024)


PathLib::PathLib(void)
{
}

PathLib::~PathLib(void)
{
}

const CString PathLib::GetAppFullName()
{
	TCHAR szBuffer[1024];

	GetModuleFileName(NULL, szBuffer, 1024);
	
	return szBuffer;
}

const CString PathLib::GetAppPath()
{
	CString strAppFullName = GetAppFullName();
	CString strAppPath = strAppFullName.Left(strAppFullName.ReverseFind('\\') + 1);
	return strAppPath;
}

const CString PathLib::GetAppName()
{
	CString strAppFullName = GetAppFullName();
	CString strAppName = strAppFullName.Right(strAppFullName.GetLength() - strAppFullName.ReverseFind('\\') - 1);
	return strAppName;
}

const CString PathLib::GetSpecifyPath(PathFlag flag)
{
	CString strPath = _T("");

	switch (flag)
	{
	case SYSCONFIG_PATH:
		{
			strPath = GetConfigPath();
			break;
		}
	case FACE_PATH:
		{
			strPath = GetFacePath();
			break;
		}
	case SETTING_PATH:
		{
			strPath = GetSettingPath();
			break;
		}
	case BLOCK_PATH:
		{
			strPath = GetBlockPath();
			break;
		}
	case DATA_PATH:
		{
			strPath = GetDataPath();
			break;
		}
	case ZIXUN_PATH:
		{
			strPath = GetZixunPath();
			break;
		}
	case ATTACHMODULES_PATH:
		{
			strPath = GetAttachModulesPath();
			break;
		}
	case USER_PATH:
		{
			strPath = GetUserPath();
			break;
		}	
	case LOG_PATH:
		{
			strPath = GetLogPath();
			break;
		}
	case SCITER_PATH:
		{
			strPath = GetSciterPath();
			break;
		}
	default:
		ASSERT(FALSE);
	}

	return strPath;
}

const CString PathLib::GetConfigPath()
{
	static CString strPath = GetAppPath() + _T("config");
	return strPath;
}

const CString PathLib::GetFacePath()
{
	static CString strPath = GetAppPath() + _T("faceBlack");
	return strPath;
}

const CString PathLib::GetSettingPath()
{
	static CString strPath = GetAppPath() + _T("Setting");
	return strPath;
}

const CString PathLib::GetBlockPath()
{
	static CString strPath = GetAppPath() + _T("Block");
	return strPath;
}

const CString PathLib::GetDataPath()
{
	static CString strPath = GetAppPath() + _T("Data");
	return strPath;
}

const CString PathLib::GetZixunPath()
{
	static CString strPath = GetAppPath() + _T("InfoData");
	return strPath;
}

const CString PathLib::GetAttachModulesPath()
{
	static CString strPath = GetAppPath() + _T("attachmodules");
	return strPath;
}

const CString PathLib::GetUserPath()
{
	static CString strPath = GetAppPath() + _T("User");
	return strPath;
}

const CString PathLib::GetLogPath()
{
	static CString strPath = GetAppPath() + _T("Log");
	return strPath;
}

const CString PathLib::GetSciterPath()
{
	static CString strPath = GetAppPath() + _T("Sciter");
	return strPath;
}

const CString PathLib::GetCurrentDirectory()
{
	TCHAR szPath[1024] = { 0 };

	::GetCurrentDirectory(sizeof(szPath), szPath);

	return szPath;
}

const CString PathLib::GetParentDirectory(LPCTSTR pszPath)
{
	CString strPath = pszPath;

	strPath.TrimRight(PATH_SPLIT_CHAR_W);
	strPath.TrimRight(PATH_SPLIT_CHAR_U);

	TCHAR szDirve[8], szPath[256], szFileName[256], szExtName[64];

	_tsplitpath_s(strPath, szDirve, szPath, szFileName, szExtName);

	strPath.Format(_T("%s%s"), szDirve, szPath);

	return strPath;
}

BOOL PathLib::CreateDirectory(LPCTSTR pszPath)
{
	if (PathLib::PathFileExists(pszPath))
	{
		return TRUE;
	}

	if (PathLib::IsRelativePath(pszPath))			// 相对路径转化为绝对路径
	{
		CString strAbsPath;
		strAbsPath.Format(_T("%s\\%s"), PathLib::GetCurrentDirectory(), pszPath);

		return PathLib::CreateDirectory(strAbsPath);
	}

	CString strParentPath = PathLib::GetParentDirectory(pszPath);

	if (PathLib::CreateDirectory(strParentPath))
	{
		return ::CreateDirectory(pszPath, NULL);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL PathLib::EnsureDirExsit(LPCTSTR pszPath)
{
	return PathLib::CreateDirectory(pszPath);
}

BOOL PathLib::PathFileExists(LPCTSTR pszPath)
{
	return ::PathFileExists(pszPath);
}

BOOL PathLib::IsRelativePath(LPCTSTR pszPath)
{
	if (PathLib::IsNetPath(pszPath))		// 是网络路径
	{
		return FALSE;
	}

	if (_tcsclen(pszPath) <= 2)	// 长度不够，肯定是相对路径
	{
		return TRUE;
	}

	if (pszPath[1] == DRIVE_IDENTIFICATION)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL PathLib::IsNetPath(LPCTSTR pszPath)
{
	if (_tcsclen(pszPath) <= 2)
	{
		return FALSE;
	}

	// 长度大于2才有比较价值
	if (pszPath[0] == PATH_SPLIT_CHAR_W && pszPath[1] == PATH_SPLIT_CHAR_W)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL PathLib::DeleteFile(LPCTSTR lpFileName)
{
	if (!PathLib::PathFileExists(lpFileName))
	{
		return TRUE;
	}

	BOOL bRet = ::DeleteFile(lpFileName);

	if (!bRet && GetLastError() == ERROR_ACCESS_DENIED)
	{
		SetFileAttributes(lpFileName, GetFileAttributes(lpFileName) & (~FILE_ATTRIBUTE_READONLY));
		bRet = ::DeleteFile(lpFileName);
	}

	return bRet;
}

BOOL PathLib::RenameFile(LPCTSTR lpFileName, LPCTSTR lpNewName)
{
	if (!PathLib::PathFileExists(lpFileName))
	{
		return FALSE;
	}

	if (!PathLib::DeleteFile(lpNewName))
	{
		return FALSE;
	}

	BOOL bRet = ::MoveFile(lpFileName, lpNewName);

	return bRet;
}

BOOL PathLib::RemoveDirectory(LPCTSTR pszPath)
{
	if (!PathLib::EmptyDirectoryFiles(pszPath))
	{
		return FALSE;
	}

	return ::RemoveDirectory(pszPath);
}

BOOL PathLib::EmptyDirectoryFiles(LPCTSTR pszPath)
{
	HANDLE hFind;
	WIN32_FIND_DATA fd;
	BOOL bRet = TRUE;

	CString strFilter;
	strFilter.Format(_T("%s\\*.*"), pszPath);

	hFind = FindFirstFile(strFilter, &fd);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	while (FindNextFile(hFind, &fd) != 0)
	{
		if (_tcscmp(fd.cFileName, _T(".")) == 0 || _tcscmp(fd.cFileName, _T("..")) == 0)
		{
			continue;
		}

		strFilter.Format(_T("%s\\%s"), pszPath, fd.cFileName);

		bRet = PathLib::DeleteFile(strFilter);

		if (!bRet)
		{
			break;
		}
	}

	FindClose(hFind);

	return bRet;
}

PathHolder::PathHolder(void)
{
	Moor();
}

PathHolder::~PathHolder(void)
{
	Unmoor();
}

void PathHolder::Moor()
{
	m_PathAnchor = PathLib::GetCurrentDirectory();
}

void PathHolder::Unmoor()
{
	SetCurrentDirectory(m_PathAnchor);
}

FileHolder::FileHolder(LPCTSTR lpPathName)
{
	SetPathName(lpPathName);
}

FileHolder::~FileHolder(void)
{

}

void FileHolder::SetPathName(LPCTSTR lpPathName)
{
	m_strPathName = lpPathName;
}

const CString FileHolder::GetFilePath()
{
	int nPos = m_strPathName.ReverseFind(PATH_SPLIT_CHAR_W);

	if (nPos < 0)
	{
		nPos = m_strPathName.ReverseFind(PATH_SPLIT_CHAR_U);
	}

	if (nPos < 0)
	{
		return _T("");
	}

	CString strPath = m_strPathName.Left(nPos + 1);

	return strPath;
}

const CString FileHolder::GetFileName()
{
	int nPos = m_strPathName.ReverseFind(PATH_SPLIT_CHAR_W);

	if (nPos < 0)
	{
		nPos = m_strPathName.ReverseFind(PATH_SPLIT_CHAR_U);
	}

	if (nPos < 0)
	{
		return m_strPathName;
	}

	CString strName = m_strPathName.Mid(nPos + 1);

	return strName;
}

const CString FileHolder::GetBaseName()
{
	CString strFileName = GetFileName();

	int nPos = strFileName.ReverseFind(_T('.'));

	if (nPos < 0)
	{
		return strFileName;
	}

	CString strBaseName = strFileName.Left(nPos);

	return strBaseName;
}

const CString FileHolder::GetExtName()
{
	CString strFileName = GetFileName();

	int nPos = strFileName.ReverseFind(_T('.'));

	if (nPos < 0)
	{
		return _T("");
	}

	CString strExtName = strFileName.Mid(nPos + 1);

	return strExtName;
}

const CString FileHolder::PathNameWithNewExt(LPCTSTR lpExtName)
{
	ASSERT(lpExtName != NULL);

	CString strExtName;

	if (lpExtName[0] != _T('.'))
	{
		strExtName.Format(_T(".%s"), lpExtName);
	}
	else
	{
		strExtName = lpExtName;
	}

	CString strPath = GetFilePath();
	CString strBaseName = GetBaseName();

	CString strPathName;
	strPathName.Format(_T("%s%s%s"), strPath, strBaseName, strExtName);

	return strPathName;
}

