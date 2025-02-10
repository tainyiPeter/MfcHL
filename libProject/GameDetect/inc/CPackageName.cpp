// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once


#include "CPackageName.h"

wstring GetProcessPackageFullName(
	_In_ HANDLE ProcessHandle
)
{
	WCHAR systemdir[MAX_PATH] = { 0 };
	GetSystemDirectory(systemdir, MAX_PATH);
	wstring mpath = systemdir;
	mpath.append(L"\\kernel32.dll");
	LibraryCaller libcaller(mpath,true);
	LONG result;
	WCHAR *namebuffer = NULL;
	ULONG nameLength = 0;
	result = libcaller.ExcecuteFunc<LONG(HANDLE, ULONG*, WCHAR*)>(std::string("GetPackageFullName"), ProcessHandle, &nameLength, namebuffer);

	wstring strbuffer;
	if (result == ERROR_INSUFFICIENT_BUFFER)
	{
		namebuffer = (WCHAR*)malloc(nameLength * sizeof(WCHAR) + 2);
		SecureZeroMemory(namebuffer, nameLength * sizeof(WCHAR) + 2);
		result = libcaller.ExcecuteFunc<LONG(HANDLE, ULONG*, WCHAR*)>("GetPackageFullName", ProcessHandle, &nameLength, namebuffer);

		strbuffer = namebuffer;
		free(namebuffer);
	}

	return strbuffer;
}

//LONG GetPackageFamilyName(
//	HANDLE hProcess,
//	UINT32 *packageFamilyNameLength,
//	PWSTR  packageFamilyName
//);


wstring GetProcessPackageFamilyName(
	_In_ HANDLE ProcessHandle
)
{
	WCHAR systemdir[MAX_PATH] = { 0 };
	GetSystemDirectory(systemdir, MAX_PATH);
	wstring mpath = systemdir;
	mpath.append(L"\\kernel32.dll");
	LibraryCaller libcaller(mpath,true);
	LONG result;
	WCHAR *namebuffer = NULL;
	ULONG nameLength = 0;
	result = libcaller.ExcecuteFunc<LONG(HANDLE, ULONG*, WCHAR*)>("GetPackageFamilyName", ProcessHandle, &nameLength, namebuffer);

	wstring strbuffer;
	if (result == ERROR_INSUFFICIENT_BUFFER)
	{
		namebuffer = (WCHAR*)malloc(nameLength * sizeof(WCHAR) + 2);
		SecureZeroMemory(namebuffer, nameLength * sizeof(WCHAR) + 2);
		result = libcaller.ExcecuteFunc<LONG(HANDLE, ULONG*, WCHAR*)>("GetPackageFamilyName", ProcessHandle, &nameLength, namebuffer);

		strbuffer = namebuffer;
		free(namebuffer);
	}

	return strbuffer;
}


//LONG GetPackageId(
//	HANDLE hProcess,
//	UINT32 *bufferLength,
//	BYTE   *buffer
//);

PACKAGE_ID*  GetProcessPackageId(
	_In_ HANDLE ProcessHandle,
	wstring& strIsHaveName
)
{
	WCHAR systemdir[MAX_PATH] = { 0 };
	GetSystemDirectory(systemdir, MAX_PATH);
	wstring mpath = systemdir;
	mpath.append(L"\\kernel32.dll");
	LibraryCaller libcaller(mpath,true);
	LONG result;
	BYTE *namebuffer = NULL;
	ULONG nameLength = 0;
	result = libcaller.ExcecuteFunc<LONG(HANDLE, ULONG*, BYTE*)>("GetPackageId", ProcessHandle, &nameLength, namebuffer);

	if (result == ERROR_INSUFFICIENT_BUFFER)
	{
		namebuffer = (BYTE*)malloc(nameLength + 2);
		SecureZeroMemory(namebuffer, nameLength + 2);
		result = libcaller.ExcecuteFunc<LONG(HANDLE, ULONG*, BYTE*)>("GetPackageId", ProcessHandle, &nameLength, namebuffer);
	}


	if (result == ERROR_SUCCESS)
	{
		return (PACKAGE_ID*)namebuffer;
	}

	if (result == APPMODEL_ERROR_NO_PACKAGE)
	{
		strIsHaveName = L"no";
	}

	return nullptr;
}

//BOOL IsImmersiveProcess(
	//HANDLE hProcess
//);
BOOL CheckIsUWPProc(
	_In_ HANDLE ProcessHandle
)
{
	WCHAR systemdir[MAX_PATH] = { 0 };
	GetSystemDirectory(systemdir, MAX_PATH);
	wstring mpath = systemdir;
	mpath.append(L"\\user32.dll");
	LibraryCaller libcaller(mpath,true);
	BOOL result = FALSE;

	result = libcaller.ExcecuteFunc<BOOL(HANDLE)>("IsImmersiveProcess", ProcessHandle);

	return result;
}


