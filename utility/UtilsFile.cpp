#include "UtilsFile.h"
#include "UtilsString.h"
#include "../MacroDef.h"
#include "FileHelper.h"

//#include "libutils.h"
//#include "CApplication.h"

#include <functional>
#include <fstream>
#include <iostream>


#include <sstream>
#include <shlwapi.h>
#include <shlobj.h>

#include <windows.h>
#include <io.h>

#define		HL_DATA_PATH			L"data"
#define		HL_CONFIG_PATH			L"config"
#define		HL_CONFIG_FILE			L"config.ini"
#define		HL_GAMEHL_PATH			L"GameHL\\"


void UtilsFile::GetStringByLine(const std::wstring& strFileName, std::vector<std::string>& vecData)
{
	if (!CFileHelper::IsFileExists(strFileName.c_str()))
	{
		//文件不存在 直接返回
		return;
	}

	std::ifstream ifile(strFileName.c_str());
	std::string line;
	while (getline(ifile, line))
	{
		vecData.push_back(line);
	}
	ifile.close();
}

std::wstring UtilsFile::GetPath()
{
	TCHAR szBuffer[MAX_PATH] = _T("");
	::GetModuleFileName(NULL, szBuffer, MAX_PATH);

	std::wstring strFullName = szBuffer;
	size_t iPos = strFullName.rfind(_T("\\"));	
	if (iPos == strFullName.npos)
		return L"";

	std::wstring strPath = strFullName.substr(0, iPos);
	strPath += _T("\\");
	return strPath;
}

std::wstring UtilsFile::GetDataPath()
{
	std::wstring strPath = UtilsString::FormatString(L"%s\\%s", GetPath().c_str(), HL_DATA_PATH);
	return strPath;
}

std::wstring UtilsFile::GetCfgPath()
{
	static std::wstring strPath;
	if (strPath.empty())
	{
		strPath = UtilsString::FormatString(L"%s%s", GetPath().c_str(), HL_CONFIG_PATH);
	}
	return strPath;
}

std::wstring UtilsFile::GetCfgFile()
{
	static std::wstring strFile;
	if (strFile.empty())
	{
		strFile = UtilsString::FormatString(L"%s\\%s", GetCfgPath().c_str(), HL_CONFIG_FILE);
	}
	return strFile;
}

std::wstring UtilsFile::GetAppDataPath()
{
	wchar_t buffer[MAX_PATH] = L"";
	SHGetSpecialFolderPath(0, buffer, CSIDL_APPDATA, false);
	return buffer;
}

std::wstring UtilsFile::GetUserDataPath()
{
	std::wstring strPath = GetAppDataPath();
	strPath += L"\\";
	strPath += HL_GAMEHL_PATH;

	return strPath;
}

bool UtilsFile::EnumAllFiles(const std::wstring& strPath, std::vector<std::wstring>& vecFileName)
{
	if (strPath.empty()) return false;

	std::wstring strTmpPath = strPath + _T("//*.*");
	WIN32_FIND_DATA findFileData;
	ZeroMemory(&findFileData, sizeof(WIN32_FIND_DATA));
	HANDLE hFile = ::FindFirstFile(strTmpPath.c_str(), &findFileData);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return true;
	}
	do
	{
		if (findFileData.cFileName[0] != '.')
		{
			if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
			}
			else
			{
				std::wstring strFullFile = strPath + L"\\";
				strFullFile += findFileData.cFileName;
				vecFileName.push_back(strFullFile);
				//vecFileName.push_back(findFileData.cFileName);
			}
		}
	} while (FindNextFile(hFile, &findFileData));
	FindClose(hFile);

	return !vecFileName.empty();
}

bool UtilsFile::EnumAllFiles(const std::string& strPath, std::vector<std::string>& vecFileName)
{
	//文件信息  
	struct _finddata_t fileinfo;
	std::string strDir;

	//文件句柄  
	intptr_t hFile = _findfirst(strDir.assign(strPath).append("\\*").c_str(), &fileinfo);
	if (hFile <= 0)
	{
		return false;
	}

	do
	{
		//如果是目录,迭代之  
		//如果不是,加入列表  
		if ((fileinfo.attrib & _A_SUBDIR))
		{
		}
		else
		{
			std::string strFullFile = strPath + "\\";
			strFullFile += fileinfo.name;
			vecFileName.push_back(strFullFile);
			//vecFileName.push_back(fileinfo.name);
		}
	} while (_findnext(hFile, &fileinfo) == 0);
	_findclose(hFile);
	return true;
}

bool UtilsFile::IsFileExists(const std::wstring& strFile)
{
	return  CFileHelper::IsFileExists(strFile.c_str());
}

std::string UtilsFile::w2a(std::wstring s)
{
	return w2a(s.c_str());
}

std::string UtilsFile::w2a(const wchar_t* s)
{
	std::string str;

	if (s != NULL)
	{
		int len = WideCharToMultiByte(CP_ACP, 0, s, -1, NULL, 0, NULL, NULL) - 1;

		if (len > 0)
		{//测试数据: s=神界 len=4
			str.reserve(len + 1);
			str.resize(len);
			len = WideCharToMultiByte(CP_ACP, 0, s, -1, (LPSTR)str.data(), len, NULL, NULL);
		}
	}

	return str.c_str();
}