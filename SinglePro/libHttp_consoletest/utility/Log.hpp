#pragma once
/*!
* @file Log.hpp
* @brief ��־��صĽӿ�
*
* @author fanmj2@lenovo.com
* @version 1.0
* @date 2021��3��5��
*/

#include <string>
#include <shlobj.h>
#include <atlstr.h>
#include <tchar.h>

/*
# liblog

## �ܼ�Ŀǰʹ�õķǳ��򵥵���־��

### ��־��ʼ���ӿ�: InitializeLoger
```C++
void InitializeLogerWithApp(LPCTSTR appName = NULL, LPCTSTR file = NULL, INT64 maxSize = 1024 * 1024, bool encryped = true, bool autoCheckSize = false);
```
�ֶ� |����| ��ע
---|---|---
appName|�ַ���|������ƣ����ΪNULL������־��������TempĿ¼��<br>�����ΪNULL����������%programdata%\Lenovo\$appName$Ŀ¼��
file|�ַ���|���ΪNULL������־�Խ�����Ϊ�ļ���<br>������ļ����������ļ�����<br>������ļ�·����������б�ܣ�����ֱ�Ӵ�������־�ļ�(���Ŀ¼�����ڣ����Զ�����)<br>
maxSize|int|��־�ļ�������С�����������С���ڳ�ʼ����ʱ����Զ�ɾ���ɵ���־<br>Ĭ��Ϊ1024*1024
encryped|bool|�Ƿ������־��Ĭ��Ϊtrue<br> ����ʱ����־�ļ�����չ�������.logdat<br><b>DEBUG��������־�ǲ����ܵ�</b>
autoCheckSize|bool|�Ƿ�����ʱ�Զ������־��С��Ĭ��Ϊfalse<br>���Ϊtrue����ÿ��¼100����־���Զ������־�Ĵ�С

>���������InitializeLogerWithApp��ʼ������������Զ���Ĭ�ϲ�������ʼ��
>Sample Code:
```C++
InitializeLogerWithApp(L"devicecenter",NULL, 1024 * 1024, true);
for (int index = 0; index < 20; index++)
{
LOG_INFO(L"Hello ��־:%d->%d", index, GetTickCount());
::Sleep(100);
}
```
## ������־���������̨
```C++
__SetEnableLogPrintf__()
```
��־Ĭ���ǲ����������̨�ģ������Ҫ���������ô˽ӿ�

## ������־���������̨:
```C++
__Set_Global_LogLevel__(LENOVO_LOG_LEVEL_DEBUG)
```
Debug��������־Ĭ�ϻ�ȫ�������Release������Ĭ��ֻ���С�ڵ���INFO�����<br>
ʹ�ô˽ӿ�������־����ļ���


## ���ܹ���
>toolsĿ¼�µ�PCMLogDecyptTool.exe <br>
˫���˹��߻��Զ�����%programdata%\Lenovo\devicecenter\logsĿ¼�µ� .logdat�ļ��������ļ������ɵ�%programdata%\Lenovo\devicecenter\logs\decrypted\\ Ŀ¼��<br><br>
���ߵ������в���֧�ֽ���·��<br>
. ��ʾ���ܹ���Ŀ¼�µĻ�����־�ļ�<br>
����·������˫���ţ�Ʃ��"d:\\logs\\"<br>
*/

#define LENOVO_LOG_LEVEL_ERROR		1
#define LENOVO_LOG_LEVEL_WARNING	2
#define LENOVO_LOG_LEVEL_INFO		3
#define LENOVO_LOG_LEVEL_TRACE		4
#define LENOVO_LOG_LEVEL_DEBUG		5

#define LOG_WIDEN2(x) L ## x
#define LOG_WIDEN(x) LOG_WIDEN2(x)
#define __LOG__WFILE__ LOG_WIDEN(__FILE__)

//�˴��ĺ궨����,��Ϊ�˺;ɰ汾��־��Ľӿڼ���
#define  __WriteLog__				CGlobalLogImplement::GetInstance()->WriteLog
#define __SetEnableLogPrintf__		CGlobalLogImplement::GetInstance()->SetEnableLogPrintf
#define __Set_Global_LogLevel__		CGlobalLogImplement::GetInstance()->SetLogLevel
#define InitializeLoger				CGlobalLogImplement::GetInstance()->Initialize
#define InitializeLogerWithApp		CGlobalLogImplement::GetInstance()->InitializeWithApp

/// @brief ��¼���󼶱����־
#define LOG_ERROR(MSG,...)		__WriteLog__(LENOVO_LOG_LEVEL_ERROR, MSG, __LOG__WFILE__, __LINE__, __VA_ARGS__)
/// @brief ��¼���漶�����־
#define LOG_WARNING(MSG,...)	__WriteLog__(LENOVO_LOG_LEVEL_WARNING, MSG, __LOG__WFILE__, __LINE__, __VA_ARGS__)
/// @brief ��¼INFO�������־
#define LOG_INFO(MSG,...)		__WriteLog__(LENOVO_LOG_LEVEL_INFO, MSG, __LOG__WFILE__, __LINE__, __VA_ARGS__)
/// @brief ��¼TRACE�������־
#define LOG_TRACE(MSG,...)		__WriteLog__(LENOVO_LOG_LEVEL_TRACE, MSG, __LOG__WFILE__, __LINE__, __VA_ARGS__)
/// @brief ��¼DEBUG�������־
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
		//���ݾɰ汾��Ĭ���ǹܼҵ�Ŀ¼ 
		InitializeLogerWithApp(L"SmartEngine", file, maxSize, encryped);
	}

	void InitializeWithApp(LPCTSTR appName = NULL, LPCTSTR file = NULL, INT64 maxSize = 1024 * 1024, bool encryped = true, bool autoCheckSize = true)
	{
		g_lenovo_log_autochecksize = autoCheckSize;
		g_lenovo_global_max_size = maxSize;
		g_lenovo_log_encryped = encryped;

#ifdef DEBUG
		g_lenovo_log_encryped = false;//DEBUG Ĭ�ϲ�����־�ļ���
		g_lenovo_global_log_level = LENOVO_LOG_LEVEL_DEBUG;	//DEBUGĬ���������е���־
#else
		g_lenovo_global_log_level = LENOVO_LOG_LEVEL_INFO;	//Release����־Ĭ����INFO�����
#endif

		//
		ATL::CString initPath;
		::SHGetSpecialFolderPath(NULL, initPath.GetBuffer(MAX_PATH), CSIDL_COMMON_APPDATA, FALSE);
		initPath.ReleaseBuffer();
		initPath.Append(_T("\\Lenovo\\SmartEngine\\config\\config.ini"));
		
															//����ͨ�������ļ�����ϸ����־
		//ATL::CString initPath = L"C:\\programdata\\lenovo\\SmartEngine\\config\\config.ini";
		if (::PathFileExists(initPath))
		{
			g_lenovo_global_log_level = ::GetPrivateProfileInt(_T("config"), L"g_lenovo_global_log_level", g_lenovo_global_log_level, initPath);
			g_lenovo_global_max_size = ::GetPrivateProfileInt(_T("config"), L"g_lenovo_global_max_size", (int)g_lenovo_global_max_size, initPath);
			g_lenovo_log_encryped = ::GetPrivateProfileInt(_T("config"), L"g_lenovo_log_encryped", g_lenovo_log_encryped, initPath);
			g_lenovo_log_printf = ::GetPrivateProfileInt(_T("config"), L"g_lenovo_log_printf", g_lenovo_log_printf, initPath);
		}

		//�����ʼ��������File��Ĭ��ʹ�ý�������Ϊ��־�ļ�
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
		//�����������ļ�·�� ��ֱ��ʹ�ã�����Ĭ��ʹ�ùܼҵ���־·��
		int backslashPos = logFile.ReverseFind(_T('\\'));
		if (backslashPos > 0)
		{
			logFolder = logFile.Mid(0, backslashPos + 1);
		}
		else
		{
			if (appName == NULL || wcsnlen_s(appName, MAX_PATH) == 0)
			{
				//û��AppName����־Ĭ����������ʱĿ¼��
				/*::GetTempPath(MAX_PATH, logFolder.GetBuffer(MAX_PATH));
				logFolder.ReleaseBuffer();*/
				//2022��1��16�� ���ܻ����ɵ�ϵͳĿ¼Ӱ��ж�ػ������ɵ�log Ŀ¼��
				//ʹ��AppName��ΪProgramData��Ŀ¼���ƣ����ж�ص�ʱ����Ҫ����Լ�������ɾ��
				::SHGetSpecialFolderPath(NULL, logFolder.GetBuffer(MAX_PATH), CSIDL_COMMON_APPDATA, FALSE);
				logFolder.ReleaseBuffer();
				logFolder.Append(_T("\\Lenovo\\SmartEngine\\logs\\"));
			}
			else
			{
				//ʹ��AppName��ΪProgramData��Ŀ¼���ƣ����ж�ص�ʱ����Ҫ����Լ�������ɾ��
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

		//https://jira.xpaas.lenovo.com/browse/LPM-2725 ��������� �ⲻ������
		if (GetFileAttributes(logFolder) & FILE_ATTRIBUTE_REPARSE_POINT)
		{
			return;
		}

		if (g_lenovo_log_encryped)
		{
			//���ݹܼ�֮ǰ�汾δ���ܵ���־��ɾ���ɵ���־�ļ�
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
		//������÷�û�г�ʼ����������Ĭ�ϲ�������ʼ��
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
				//�Զ������־�Ĵ�С
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
				//https://jira.xpaas.lenovo.com/browse/LPM-2725 SSRB����İ�ȫ����
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
