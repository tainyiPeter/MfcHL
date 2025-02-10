#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <atlstr.h>
#include "FileInfo.h"
#include <string>
#include <functional>
#include "StringUtility.h"
#include "LibraryCaller.h"

using namespace std;

#define _FLP_	L"function(%S),line(%d)\t"
#define _FL_	__FUNCTION__, __LINE__

#define DBG_PRINT_FUNCINFO Debug2DbgView(_FLP_, _FL_);
// if null return HRESULT
#define IF_NULL_RETURN_HRESULT(p)   do{\
	if((p)==NULL)\
	{\
	return S_FALSE;\
	}\
}while(false)

// if null return BOOL
#define IF_NULL_RETURN_BOOL(p)      do{\
	if((p) == NULL)\
	{\
	return FALSE;\
	}\
}while(false)

//  close file handle
#define SAFE_CLOSE_FILE_HANDLE(P)     do{\
	if((P)&&(P)!=INVALID_HANDLE_VALUE)\
	{\
	CloseHandle((PVOID)(P));\
	(P)=NULL;\
	}\
}while(false)
// close normal handle.
#define SAFE_CLOSE_HANDLE(p)         do{\
	if((p) != NULL)\
	{\
	CloseHandle((p));\
	(p) = NULL;\
	}\
}while(false);

//  release memory safely.
#define SAFE_RELEASE_MEMORY(P)       do{\
	if((P) != NULL)\
	{\
	delete [] (P);\
	(P)=NULL;\
	}\
}while(false)


enum SpecialLogFlag
{
	allFlag = 1,
	encryption,
	moniterUWP
};


static int CheckOutSpecialLogFlag()
{
	static int outputdebug = -1;
	if (outputdebug == -1)
	{
		WCHAR wcData[MAX_PATH] = { 0 };
		ULONG dwDisp = sizeof(wcData);
		HKEY Key;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			L"SOFTWARE\\Lenovo\\Modern\\Logs",
			0,
			KEY_QUERY_VALUE,
			&Key) != ERROR_SUCCESS)
		{
			outputdebug = 0;
			return 0;
		}
		if (RegQueryValueEx(Key,
			L"Gaming.Special",
			NULL,
			NULL,
			(LPBYTE)wcData,
			&dwDisp) != ERROR_SUCCESS)
		{
			outputdebug = 0;
		}
		else
			outputdebug = _wtoi(wcData);

		CloseHandle(Key);
	}
	return outputdebug;
}

class Gaming_Utilities_dll : public LibraryCaller
{
public:
	static Gaming_Utilities_dll* GetInstance()
	{
		static Gaming_Utilities_dll utilities_all(L"Gaming.Utilities.dll");
		return &utilities_all;
	}

	wchar_t* get_module_name()
	{
		return module_name_.GetBuffer();
	}

private:
	Gaming_Utilities_dll(const std::wstring& filename):LibraryCaller(filename)
	{
		TCHAR path[MAX_PATH] = { 0 };
		CFileInfo fileinfo;
		fileinfo.GetCurrentModuleFilePath(path);
		fileinfo.GetFileShortName(path, module_name_, FALSE);
	};

	CString module_name_;
};

template<typename... Args>
static void Debug2DbgView(__in const TCHAR* tSzFormat, Args&&... args)
{
	try
	{
		if (Gaming_Utilities_dll::GetInstance()->isLoaded())
		{
			Gaming_Utilities_dll::GetInstance()->ExcecuteFunc<void(const TCHAR*, const TCHAR*, ...)>("write_log", 
				Gaming_Utilities_dll::GetInstance()->get_module_name(), tSzFormat, std::forward<Args>(args)...);
		}
	}
	catch (...)
	{

	}
}

#define DBG_PRINTF_FL(arg) do{ DBG_PRINT_FUNCINFO Debug2DbgView arg; }while(0)


