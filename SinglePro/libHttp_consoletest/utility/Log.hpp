#pragma once
/*!
* @file Log.hpp
* @brief 日志相关的接口
*
* @author fanmj2@lenovo.com
* @version 1.0
* @date 2021年3月5日
*/

#include <string>
#include <shlobj.h>
#include <atlstr.h>
#include <tchar.h>

/*
# liblog

## 管家目前使用的非常简单的日志库

### 日志初始化接口: InitializeLoger
```C++
void InitializeLogerWithApp(LPCTSTR appName = NULL, LPCTSTR file = NULL, INT64 maxSize = 1024 * 1024, bool encryped = true, bool autoCheckSize = false);
```
字段 |类型| 备注
---|---|---
appName|字符串|软件名称，如果为NULL，则日志会生成在Temp目录下<br>如果不为NULL，则生成在%programdata%\Lenovo\$appName$目录下
file|字符串|如果为NULL，则日志以进程名为文件名<br>如果是文件名，则以文件命名<br>如果是文件路径（包含反斜杠），则直接创建此日志文件(如果目录不存在，会自动创建)<br>
maxSize|int|日志文件的最大大小，超过这个大小，在初始化的时候会自动删除旧的日志<br>默认为1024*1024
encryped|bool|是否加密日志，默认为true<br> 加密时，日志文件的扩展名会加上.logdat<br><b>DEBUG环境下日志是不加密的</b>
autoCheckSize|bool|是否运行时自动检测日志大小，默认为false<br>如果为true，则每记录100条日志，自动检测日志的大小

>如果不调用InitializeLogerWithApp初始化函数，则会自动以默认参数来初始化
>Sample Code:
```C++
InitializeLogerWithApp(L"devicecenter",NULL, 1024 * 1024, true);
for (int index = 0; index < 20; index++)
{
LOG_INFO(L"Hello 日志:%d->%d", index, GetTickCount());
::Sleep(100);
}
```
## 设置日志输出到控制台
```C++
__SetEnableLogPrintf__()
```
日志默认是不输出到控制台的，如果需要输出，这调用此接口

## 设置日志输出到控制台:
```C++
__Set_Global_LogLevel__(LENOVO_LOG_LEVEL_DEBUG)
```
Debug环境，日志默认会全部输出，Release环境，默认只输出小于等于INFO级别的<br>
使用此接口设置日志输出的级别


## 解密工具
>tools目录下的PCMLogDecyptTool.exe <br>
双击此工具会自动解密%programdata%\Lenovo\devicecenter\logs目录下的 .logdat文件，解密文件会生成到%programdata%\Lenovo\devicecenter\logs\decrypted\\ 目录下<br><br>
工具的命令行参数支持解密路径<br>
. 表示解密工具目录下的机密日志文件<br>
完整路径带上双引号，譬如"d:\\logs\\"<br>
*/

#define LENOVO_LOG_LEVEL_ERROR		1
#define LENOVO_LOG_LEVEL_WARNING	2
#define LENOVO_LOG_LEVEL_INFO		3
#define LENOVO_LOG_LEVEL_TRACE		4
#define LENOVO_LOG_LEVEL_DEBUG		5

#define LOG_WIDEN2(x) L ## x
#define LOG_WIDEN(x) LOG_WIDEN2(x)
#define __LOG__WFILE__ LOG_WIDEN(__FILE__)

//此处的宏定义是,是为了和旧版本日志库的接口兼容
#define  __WriteLog__				CGlobalLogImplement::GetInstance()->WriteLog
#define __SetEnableLogPrintf__		CGlobalLogImplement::GetInstance()->SetEnableLogPrintf
#define __Set_Global_LogLevel__		CGlobalLogImplement::GetInstance()->SetLogLevel
#define InitializeLoger				CGlobalLogImplement::GetInstance()->Initialize
#define InitializeLogerWithApp		CGlobalLogImplement::GetInstance()->InitializeWithApp

/// @brief 记录错误级别的日志
#define LOG_ERROR(MSG,...)		__WriteLog__(LENOVO_LOG_LEVEL_ERROR, MSG, __LOG__WFILE__, __LINE__, __VA_ARGS__)
/// @brief 记录警告级别的日志
#define LOG_WARNING(MSG,...)	__WriteLog__(LENOVO_LOG_LEVEL_WARNING, MSG, __LOG__WFILE__, __LINE__, __VA_ARGS__)
/// @brief 记录INFO级别的日志
#define LOG_INFO(MSG,...)		__WriteLog__(LENOVO_LOG_LEVEL_INFO, MSG, __LOG__WFILE__, __LINE__, __VA_ARGS__)
/// @brief 记录TRACE级别的日志
#define LOG_TRACE(MSG,...)		__WriteLog__(LENOVO_LOG_LEVEL_TRACE, MSG, __LOG__WFILE__, __LINE__, __VA_ARGS__)
/// @brief 记录DEBUG级别的日志
#define LOG_DEBUG(MSG,...)		__WriteLog__(LENOVO_LOG_LEVEL_DEBUG, MSG, __LOG__WFILE__, __LINE__, __VA_ARGS__)

#define LOG_ERROR_RETURN_VOID(MSG, ...) { __WriteLog__(LENOVO_LOG_LEVEL_ERROR, MSG, __LOG__WFILE__, __LINE__, __VA_ARGS__); return; }
#define LOG_ERROR_RETURN_FALSE(MSG, ...) { __WriteLog__(LENOVO_LOG_LEVEL_ERROR, MSG, __LOG__WFILE__, __LINE__, __VA_ARGS__); return FALSE; }
#define WRITE_LOG	LOG_INFO

class CGlobalLogImplement
{
public:
	BOOL				g_lenovo_log_encryped;
	BOOL				g_lenovo_log_printf = FALSE;
	int					g_lenovo_global_log_level = LENOVO_LOG_LEVEL_DEBUG;
	INT64				g_lenovo_global_max_size;
	TCHAR				g_lenovo_encrypted_log_Path[MAX_PATH] = { 0 };
	bool				g_lenovo_log_autochecksize;
	HANDLE				g_lenovo_global_log_Mutex = NULL;

	void Initialize(LPCTSTR file = NULL, INT64 maxSize = 1024 * 1024, bool encryped = true)
	{
		//兼容旧版本，默认是管家的目录 
		InitializeLogerWithApp(L"SmartEngine", file, maxSize, encryped);
	}

	void InitializeWithApp(LPCTSTR appName = NULL, LPCTSTR file = NULL, INT64 maxSize = 1024 * 1024, bool encryped = true, bool autoCheckSize = true)
	{
		g_lenovo_log_autochecksize = autoCheckSize;
		g_lenovo_global_max_size = maxSize;
		g_lenovo_log_encryped = encryped;

#ifdef DEBUG
		g_lenovo_log_encryped = false;//DEBUG 默认不做日志的加密
		g_lenovo_global_log_level = LENOVO_LOG_LEVEL_DEBUG;	//DEBUG默认生成所有的日志
#else
		g_lenovo_global_log_level = LENOVO_LOG_LEVEL_INFO;	//Release的日志默认是INFO级别的
#endif

		//
		ATL::CString initPath;
		::SHGetSpecialFolderPath(NULL, initPath.GetBuffer(MAX_PATH), CSIDL_COMMON_APPDATA, FALSE);
		initPath.ReleaseBuffer();
		initPath.Append(_T("\\Lenovo\\SmartEngine\\config\\config.ini"));
		
															//可以通过配置文件打开详细的日志
		//ATL::CString initPath = L"C:\\programdata\\lenovo\\SmartEngine\\config\\config.ini";
		if (::PathFileExists(initPath))
		{
			g_lenovo_global_log_level = ::GetPrivateProfileInt(_T("config"), L"g_lenovo_global_log_level", g_lenovo_global_log_level, initPath);
			g_lenovo_global_max_size = ::GetPrivateProfileInt(_T("config"), L"g_lenovo_global_max_size", (int)g_lenovo_global_max_size, initPath);
			g_lenovo_log_encryped = ::GetPrivateProfileInt(_T("config"), L"g_lenovo_log_encryped", g_lenovo_log_encryped, initPath);
			g_lenovo_log_printf = ::GetPrivateProfileInt(_T("config"), L"g_lenovo_log_printf", g_lenovo_log_printf, initPath);
		}

		//如果初始化不传递File，默认使用进程名作为日志文件
		ATL::CString logFile;
		if (file == NULL || wcsnlen_s(file, MAX_PATH) == 0)
		{
			ATL::CString strPath;
			::GetModuleFileName(NULL, strPath.GetBuffer(MAX_PATH), MAX_PATH);
			strPath.ReleaseBuffer();
			logFile = ::PathFindFileName(strPath);
			logFile.Append(L".log");
		}
		else
		{
			logFile = file;
		}

		ATL::CString logFolder;
		//如果传入的是文件路径 则直接使用，否则默认使用管家的日志路径
		int backslashPos = logFile.ReverseFind(_T('\\'));
		if (backslashPos > 0)
		{
			logFolder = logFile.Mid(0, backslashPos + 1);
		}
		else
		{
			if (appName == NULL || wcsnlen_s(appName, MAX_PATH) == 0)
			{
				//没有AppName，日志默认生成在临时目录下
				/*::GetTempPath(MAX_PATH, logFolder.GetBuffer(MAX_PATH));
				logFolder.ReleaseBuffer();*/
				//2022年1月16日 可能会生成到系统目录影响卸载还是生成到log 目录下
				//使用AppName作为ProgramData下目录名称，软件卸载的时候需要软件自己来负责删除
				::SHGetSpecialFolderPath(NULL, logFolder.GetBuffer(MAX_PATH), CSIDL_COMMON_APPDATA, FALSE);
				logFolder.ReleaseBuffer();
				logFolder.Append(_T("\\Lenovo\\SmartEngine\\logs\\"));
			}
			else
			{
				//使用AppName作为ProgramData下目录名称，软件卸载的时候需要软件自己来负责删除
				::SHGetSpecialFolderPath(NULL, logFolder.GetBuffer(MAX_PATH), CSIDL_COMMON_APPDATA, FALSE);
				logFolder.ReleaseBuffer();
				logFolder.Append(_T("\\Lenovo\\"));
				logFolder.Append(appName);
				logFolder.Append(L"\\logs\\");
			}
			logFile = logFolder + logFile;
		}
		if (!::PathIsDirectory(logFolder))
		{
			::SHCreateDirectoryEx(NULL, logFolder, 0);
		}

		//https://jira.xpaas.lenovo.com/browse/LPM-2725 如果是链接 这不做处理
		if (GetFileAttributes(logFolder) & FILE_ATTRIBUTE_REPARSE_POINT)
		{
			return;
		}

		if (g_lenovo_log_encryped)
		{
			//兼容管家之前版本未加密的日志，删除旧的日志文件
			int index = logFile.ReverseFind(_T('.'));
			ATL::CString oldFile = logFile.Mid(0, index);
			oldFile.Append(_T(".normal"));
			oldFile.Append(logFile.Right(logFile.GetLength() - index));
			if (::PathFileExists(oldFile))
			{
				::DeleteFile(oldFile);
			}

			logFile = logFile + L".logdat";
		}
		_tcscpy_s(g_lenovo_encrypted_log_Path, logFile.GetBuffer());

		LogCheckMaxSize();

		if (g_lenovo_global_log_Mutex == NULL)
		{
			SECURITY_ATTRIBUTES sa;
			SECURITY_DESCRIPTOR sd;
			::InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
			::SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
			sa.nLength = sizeof(SECURITY_ATTRIBUTES);
			sa.bInheritHandle = TRUE;
			sa.lpSecurityDescriptor = &sd;
			g_lenovo_global_log_Mutex = ::CreateMutex(&sa, FALSE, ATL::CString(L"Global\\SELOG_") + ::PathFindFileName(logFile));
		}
	}



	void WriteLog(int level, LPCTSTR format, LPCTSTR sourceFile, int sourceline, ...)
	{
		if (level > g_lenovo_global_log_level)
		{
			return;
		}
		//如果调用方没有初始化过，这以默认参数来初始化
		if (wcsnlen_s(g_lenovo_encrypted_log_Path, MAX_PATH) == 0)
		{
			InitializeLogerWithApp();
		}
		if (wcsnlen_s(g_lenovo_encrypted_log_Path, MAX_PATH) == 0)
		{
			return;
		}

		if (g_lenovo_global_log_Mutex != NULL)
		{
			::WaitForSingleObject(g_lenovo_global_log_Mutex, INFINITE);
		}
		DWORD NumberOfBytesWritten = 0;
		try
		{
			if (g_lenovo_log_autochecksize)
			{
				//自动检测日志的大小
				static int logCount = 0;
				if (++logCount % 100 == 0)
				{
					LogCheckMaxSize();
				}
			}
			ATL::CString logFormat;
			ATL::CString logContent;
			SYSTEMTIME currentTime = { 0 };
			GetLocalTime(&currentTime);

			logFormat.Format(_T("%.2d-%.2d %.2d:%.2d:%.2d:%.3d : %s[%.5d] %s(%d) %s\n"),
				currentTime.wMonth, currentTime.wDay, currentTime.wHour, currentTime.wMinute, currentTime.wSecond, currentTime.wMilliseconds,
				GetLogLevelString(level),
				GetCurrentThreadId(),
				PathFindFileName(sourceFile), sourceline,  format);

			va_list vaPtr;
			va_start(vaPtr, sourceline);
			logContent.FormatV(logFormat.GetBuffer(), vaPtr);
			va_end(vaPtr);

			if (g_lenovo_log_printf)
			{
				log_printf_console(level, logContent);
			}

			HANDLE fileHandle = INVALID_HANDLE_VALUE;
			do
			{
				fileHandle = CreateFile(g_lenovo_encrypted_log_Path, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
				if (fileHandle == INVALID_HANDLE_VALUE)
				{
					break;
				}
				//https://jira.xpaas.lenovo.com/browse/LPM-2725 SSRB提出的安全问题
				if (GetFileAttributes(g_lenovo_encrypted_log_Path) & FILE_ATTRIBUTE_REPARSE_POINT)
				{
					break;
				}
				SetFilePointer(fileHandle, 0, NULL, FILE_END);

				std::string utf8Content = ATL::CT2A(logContent, CP_UTF8).m_psz;
				if (g_lenovo_log_encryped)
				{
					int length = (int)utf8Content.length();
					LogStringBitCode((char*)utf8Content.c_str(), length);
					WriteFile(fileHandle, &length, 4, &NumberOfBytesWritten, NULL);
					WriteFile(fileHandle, utf8Content.c_str(), (DWORD)utf8Content.length(), &NumberOfBytesWritten, NULL);
				}
				else
				{
					WriteFile(fileHandle, utf8Content.c_str(), (DWORD)utf8Content.length(), &NumberOfBytesWritten, NULL);
				}

			} while (false);

			if (fileHandle != INVALID_HANDLE_VALUE)
			{
				CloseHandle(fileHandle);
			}

		}
		catch (...)
		{

		}
		::ReleaseMutex(g_lenovo_global_log_Mutex);
	}

	void SetEnableLogPrintf()
	{
		g_lenovo_log_printf = true;
	}

	void SetLogLevel(int level)
	{
		g_lenovo_global_log_level = level;
	}

	static CGlobalLogImplement* GetInstance()
	{
		static CGlobalLogImplement instance;
		return &instance;
	}


private:

	void LogCheckMaxSize()
	{
		HANDLE hFile = ::CreateFile(g_lenovo_encrypted_log_Path, GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			LARGE_INTEGER filesize;
			::GetFileSizeEx(hFile, &filesize);
			::CloseHandle(hFile);
			if (filesize.QuadPart >= g_lenovo_global_max_size)
			{
				::DeleteFile(g_lenovo_encrypted_log_Path);
			}
		}
	}

	void log_printf_console(int level, ATL::CString& logContent)
	{
		if (level == LENOVO_LOG_LEVEL_ERROR)
		{
			::SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		}
		else if (level == LENOVO_LOG_LEVEL_WARNING)
		{
			::SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
		}
		else if (level == LENOVO_LOG_LEVEL_INFO)
		{
			::SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
		}
		else if (level == LENOVO_LOG_LEVEL_TRACE)
		{
			::SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
		}
		else
		{
			::SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}
		wprintf(logContent.GetBuffer());
	}

	void LogStringBitCode(char* p, int length)
	{
		for (int i = 0; i < length; i++)
		{
			p[i] = ~p[i];
		}
	}

	ATL::CString GetLogLevelString(int level)
	{
		switch (level)
		{
		case LENOVO_LOG_LEVEL_ERROR:
			return L"[ERR]";
		case LENOVO_LOG_LEVEL_WARNING:
			return L"[WAR]";
		case LENOVO_LOG_LEVEL_INFO:
			return L"[INF]";
		case LENOVO_LOG_LEVEL_TRACE:
			return L"[TRA]";
		case LENOVO_LOG_LEVEL_DEBUG:
			return L"[DBG]";
		default:
			return L"[NONE]";;
		}
	}
};
