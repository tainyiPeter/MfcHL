#include "UtilsTools.h"
#include "UtilsFile.h"
#include "UtilsString.h"
#include "../MacroDef.h"
#include "FileHelper.h"

#include <functional>
#include <sstream>
#include <shlwapi.h>
#include <array>
#include <set>
#include <fstream>
#include <iostream>
#include <windows.h>
#include <wincrypt.h>
#include <lm.h>



#define			StyleLocalPath			L"/style/local"
#define			StyleRemotePath			L"/style/remote"



void UtilsTools::SplitStr(std::wstring& strSrc, const std::wstring& delim, std::vector<std::wstring>& ret)
{
	size_t last = 0;
	size_t index = strSrc.find_first_of(delim, last);
	while (index != std::wstring::npos)
	{
		ret.push_back(strSrc.substr(last, index - last));
		last = index + 1;
		while (delim == strSrc.substr(last, 1))
			last++;
		index = strSrc.find_first_of(delim, last);
	}
	if (index - last > 0)
	{
		ret.push_back(strSrc.substr(last, index - last));
	}
}

void UtilsTools::SplitStr(std::string& strInStr, const char* delim, std::vector<std::string>& ret)
{
	size_t last = 0;
	size_t index = strInStr.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		ret.push_back(strInStr.substr(last, index - last));
		last = index + 1;
		while (delim == strInStr.substr(last, 1))
			last++;

		index = strInStr.find_first_of(delim, last);
	}
	if (index - last > 0)
	{
		ret.push_back(strInStr.substr(last, index - last));
	}
}

uint64_t UtilsTools::SplitStrToBitEvents(const std::wstring& strSrc)
{
	std::wstring strSplit = strSrc;
	uint64_t bitEvent = 0;
	std::vector<std::wstring> vecEvent;
	SplitStr(strSplit, L",", vecEvent);
	for (auto it : vecEvent)
	{
		if (it.empty()) continue;
		uint64_t gEvent = _wtoi(it.c_str());
		bitEvent |= (1 << gEvent);
	}

	return bitEvent;
}

std::string UtilsTools::GetModuleDir()
{
	char szPath[MAX_PATH] = { 0 };
	if (GetModuleFileNameA(NULL, szPath, MAX_PATH))
	{
		(strrchr(szPath, '\\'))[1] = 0;
	}

	return szPath;
}

bool UtilsTools::CheckAndCreateDir(const CString& strPath)
{
	if (strPath.IsEmpty())
		return false;

	if (!CFileHelper::IsDictoryExists(strPath))
	{
		if (!CFileHelper::CreateDir(strPath))
		{
			//LOG_ERROR(L"Create HLConfig Dir failed!");
			return false;
		}
	}

	return true;
}

std::string UtilsTools::FormatCurrentTime()
{
	SYSTEMTIME currentTime = { 0 };
	GetLocalTime(&currentTime);

    std::string strFormat = UtilsString::FormatString("%.4d-%.2d-%.2d %.2d:%.2d:%.2d",
        currentTime.wYear, currentTime.wMonth, currentTime.wDay, \
        currentTime.wHour, currentTime.wMinute, currentTime.wSecond);
	
	//std::string strFormat = UtilsString::FormatString("%d", 3);
	return strFormat;
}

std::string UtilsTools::ReplaceString(const std::string& strSrc, const std::string& strOld, const std::string& strNew)
{
	std::string strDst = strSrc;
	size_t oldPos = 0;
	while (strDst.find(strOld, oldPos) != -1)//在未被替换的文本中寻找目标文本
	{
		size_t start = strDst.find(strOld, oldPos);//找到目标文本的起始下标

		strDst.replace(start, strOld.size(), strNew);

		oldPos = start + strNew.size();//记录未替换文本的起始下标
	}

	return strDst;
}

std::wstring UtilsTools::ReplaceString(const std::wstring& strSrc, const std::wstring& strOld, const std::wstring& strNew)
{
    std::wstring strDst = strSrc;
    size_t oldPos = 0;
    while (strDst.find(strOld, oldPos) != -1)//在未被替换的文本中寻找目标文本
    {
        size_t start = strDst.find(strOld, oldPos);//找到目标文本的起始下标

        strDst.replace(start, strOld.size(), strNew);
        //从str[start]开始到str[a.size()]替换为b
        //str[start]到str[a.size()]也就是a所在得片段

        oldPos = start + strNew.size();//记录未替换文本的起始下标
    }

    return strDst;
}

std::wstring UtilsTools::FormatVideoSavePath(const std::wstring& strSrc)
{
	if (strSrc.empty()) return strSrc;

    std::wstring strRet = ReplaceString(strSrc, L"/", L"\\");

    size_t end = strRet.size();
    if (strRet[end] == '\\' || strRet[end] == '/')
        end--; // 如果尾字符是反斜杠，则减少一个位置

    return strRet.substr(0, end); // 返回去除首尾反斜杠的子字符串
}


void UtilsTools::SaveLocalData(const std::wstring& strLocalFile, const std::string& strData)
{
	std::ofstream ofile(strLocalFile, std::ios::trunc);
	ofile << strData;
	ofile.close();
}

void UtilsTools::LoadLocalData(const std::wstring& strLocalFile, std::string& strData)
{
	if (!CFileHelper::IsFileExists(strLocalFile.c_str()))
		return;
	std::ifstream ifile(strLocalFile.c_str());
	ifile >> strData;
	ifile.close();
}

std::string UtilsTools::Trim(const std::string& src)
{
	std::string strRet = src;
	int index = 0;
	if (!strRet.empty())
	{
		while ((index = strRet.find(' ', index)) != std::string::npos)
		{
			strRet.erase(index, 1);
		}
	}

	return strRet;
}

std::wstring UtilsTools::Trim(const std::wstring& src)
{
	std::wstring strRet = src;
	int index = 0;
	if (!strRet.empty())
	{
		while ((index = strRet.find(L' ', index)) != std::wstring::npos)
		{
			strRet.erase(index, 1);
		}
	}

	return strRet;
}

uint64_t UtilsTools::ClearBit(uint64_t src, uint16_t pos)
{
	return src & ~(1 << pos);
}

std::time_t UtilsTools::GetTimestamp()
{
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    std::time_t timestamp = tmp.count();
    return timestamp;
}

int64_t UtilsTools::GetPastTime(int64_t ts)
{
	return UtilsTools::GetTimestamp() - ts;
}

// 将SYSTEMTIME转换为时间戳
int64_t UtilsTools::SystemtimeToTimestamp(SYSTEMTIME& st)
{
	FILETIME ft;
	SystemTimeToFileTime(&st, &ft);

	// 直接将FILETIME转换为__int64
	ULARGE_INTEGER ui;
	ui.LowPart = ft.dwLowDateTime;
	ui.HighPart = ft.dwHighDateTime;

	// 需要注意FILETIME的字节序
	return (ui.QuadPart - 116444736000000000ULL) / 10000;
}

SYSTEMTIME UtilsTools::StringToSystemTime(const std::string& strTime)
{
	SYSTEMTIME st = { 0 };
	DWORD ms = 0;
	int ret = sscanf_s(strTime.c_str(), "%4d-%2d-%2d %2d:%2d:%2d:%d",
		&st.wYear,
		&st.wMonth,
		&st.wDay,
		&st.wHour,
		&st.wMinute,
		&st.wSecond,
		&ms
	);
	if (!ret)
	{
		return SYSTEMTIME();
	}

	st.wMilliseconds = ms;

	return st;
}

//时间转换为时间戳（年-月-日T时:分:秒+时:分）
time_t UtilsTools::TimeStringToTimestamp(std::string strTime)
{
	struct tm tmTime = { 0 };
	int min, sec;
	int ret = sscanf_s(strTime.c_str(), "%4d-%2d-%2dT%2d:%2d:%2d+%2d:%2d",
		&tmTime.tm_year,
		&tmTime.tm_mon,
		&tmTime.tm_mday,
		&tmTime.tm_hour,
		&tmTime.tm_min,
		&tmTime.tm_sec,
		&min,
		&sec
	);

	if (ret)
	{
		tmTime.tm_year -= 1900;
		tmTime.tm_mon--;
	}
	return ::_mkgmtime(&tmTime) - 28800;
}

bool UtilsTools::GetFileHash256(const char* filePath, BYTE* hashBuffer)
{
	// 打开文件
	HANDLE hFile = CreateFileA(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return false;
	}

	// 获取文件大小
	DWORD fileSizeHigh = 0;
	DWORD fileSizeLow = GetFileSize(hFile, &fileSizeHigh);
	if (fileSizeLow == INVALID_FILE_SIZE) {
		CloseHandle(hFile);
		return false;
	}
	DWORD fileSize = fileSizeHigh << 32 | fileSizeLow;

	// 创建哈希对象
	HCRYPTPROV hCryptProv;
	if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
		CloseHandle(hFile);
		return false;
	}

	HCRYPTHASH hHash;
	if (CryptCreateHash(hCryptProv, CALG_SHA_256, 0, 0, &hHash)) {
		// 分块读取文件内容并计算哈希
		BYTE* fileBuffer = new BYTE[4096];
		DWORD bytesRead;
		while (ReadFile(hFile, fileBuffer, 4096, &bytesRead, NULL) && bytesRead > 0) {
			CryptHashData(hHash, fileBuffer, bytesRead, 0);
		}
		delete[] fileBuffer;

		// 获取哈希值
		DWORD hashSize = 32;
		if (CryptGetHashParam(hHash, HP_HASHVAL, hashBuffer, &hashSize, 0)) 
		{
			//CryptReleaseHash(hHash);
			CryptDestroyHash(hHash);
			CryptReleaseContext(hCryptProv, 0);
			CloseHandle(hFile);
			return true;
		}

		//CryptReleaseHash(hHash);
		CryptDestroyHash(hHash);
	}

	CryptReleaseContext(hCryptProv, 0);
	CloseHandle(hFile);

	return false;
}

void UtilsTools::GetMemInfo()
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);

	GlobalMemoryStatusEx(&statex);

	// 物理内存总量
	SIZE_T physicalTotal = statex.ullTotalPhys;
	// 可用物理内存
	SIZE_T physicalAvailable = statex.ullAvailPhys;
	// 虚拟内存总量
	SIZE_T virtualTotal = statex.ullTotalVirtual;
	// 可用虚拟内存
	SIZE_T virtualAvailable = statex.ullAvailVirtual;

	std::cout << "Total physical memory: " << physicalTotal << " bytes" << std::endl;
	std::cout << "Available physical memory: " << physicalAvailable << " bytes" << std::endl;
	std::cout << "Total virtual memory: " << virtualTotal << " bytes" << std::endl;
	std::cout << "Available virtual memory: " << virtualAvailable << " bytes" << std::endl;

	
}

std::vector<std::wstring> UtilsTools::GetUserLocalGroups()
{
	std::wstring strGroupName;

	NET_API_STATUS status = NetApiBufferFree(NULL);

	// 获取当前用户名
	DWORD username_len = 0;
	GetUserName(NULL, &username_len);
	TCHAR* username = new TCHAR[username_len+1];
	GetUserName(username, &username_len);

	// 获取当前用户所在的组信息
	LPLOCALGROUP_USERS_INFO_0 pBuf = NULL;
	DWORD prefMaxLen = MAX_PREFERRED_LENGTH;
	DWORD entriesRead = 0;
	DWORD totalEntries = 0;
	status = NetUserGetLocalGroups(NULL, username, 0, LG_INCLUDE_INDIRECT,
		(LPBYTE*)&pBuf, prefMaxLen, &entriesRead, &totalEntries);

	std::vector<std::wstring> vecData;
	if (status == NERR_Success) 
	{
		// 输出组信息
		for (DWORD i = 0; i < entriesRead; i++)
		{

			//std::cout << "Group: " << (TCHAR*)pBuf[i] << std::endl;

			vecData.push_back(pBuf[i].lgrui0_name);
		}

		// 释放资源
		status = NetApiBufferFree(pBuf);
		if (status != NERR_Success) {
			std::cerr << "Error freeing buffer" << std::endl;
		}
	}
	else {
		std::cerr << "Error getting user groups: " << status << std::endl;
	}

	// 清理分配的用户名内存
	delete[] username;
	return vecData;
}

int32_t UtilsTools::GetDiskFreeSpace()
{
	ULARGE_INTEGER FreeBytesAvailable, TotalNumberOfBytes, TotalNumberOfFreeBytes;
	if (false == GetDiskFreeSpaceEx(NULL, &FreeBytesAvailable, &TotalNumberOfBytes, &TotalNumberOfFreeBytes))
		return -1;

	return FreeBytesAvailable.QuadPart / 1024 / 1024;
}
