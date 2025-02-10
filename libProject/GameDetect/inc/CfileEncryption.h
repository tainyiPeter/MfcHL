// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <stdlib.h>
#include <wincrypt.h>
#include <wincred.h>
#include "DebugUtil.h"
#include "Regutil.h"
#pragma comment(lib, "Crypt32.lib")

#define KEYSTRINGLEN 9
using namespace std;

class CFileEnvryption
{
public:
	CFileEnvryption()
	{

	}
	~CFileEnvryption()
	{
		
	}
public:

	bool SetRegOptimazationCode(const wstring& strcode, const wstring& strRegPath, const wstring& strRegValue)
	{
		wstring value(strcode);
		bool result = false;
		result = CRegUtil().SetKeyValue(HKEY_CURRENT_USER,
			strRegPath.c_str(),
			strRegValue.c_str(),
			(LPVOID)value.c_str(),
			(ULONG)value.length() * sizeof(WCHAR),
			REG_SZ,
			TRUE);

		return result;
	}

	bool GetRegOptimazationCode(wstring& strcode, const wstring& strRegPath, const wstring& strRegValue)
	{
		wstring retString = CRegUtil().GetKeyValueStr(HKEY_CURRENT_USER,
			strRegPath.c_str(),
			strRegValue.c_str());

		if (!retString.empty())
		{
			strcode = retString;
			return true;
		}

		return false;
	}
	
	bool GetPWDString(wstring& pwdstring,const wstring& strRegPath, const wstring& strRegValue)
	{
		if (GetRegOptimazationCode(pwdstring, strRegPath, strRegValue))
			return true;

		errno_t         err;
		unsigned int    number;
		wstring randomstring;//[KEYSTRINGLEN + 1] = { 0 };
		for (int i = 0; i < KEYSTRINGLEN; i++)
		{
			err = rand_s(&number);
			if (err != 0)
			{
				randomstring += to_wstring(i);
			}
			else
			{
				unsigned char key = (unsigned char)((double)number /
					((double)UINT_MAX + 1) * 10.0) + 1;
				randomstring += to_wstring(key);
			}
		}

		pwdstring = randomstring;
		return SetRegOptimazationCode(randomstring, strRegPath, strRegValue);
	}

	bool SaveDataW(LPCWSTR data, DWORD length, const wstring& filePath, const wstring& descriptionName, const wstring& password)
	{
		try
		{
			if (filePath.empty() || descriptionName.empty() || password.empty())
				return false;

			DATA_BLOB DataIn;
			DATA_BLOB DataOut;
			BYTE *pbDataInput = (BYTE*)data;
			DWORD cbDataInput = length + 1;
			DataIn.pbData = pbDataInput;
			DataIn.cbData = cbDataInput;
			PDATA_BLOB pDataKey = NULL;
			DATA_BLOB DataKey;

			DataKey.pbData = (BYTE *)password.c_str();
			DataKey.cbData = (DWORD)password.size() * sizeof(WCHAR);
			pDataKey = &DataKey;

			if (!CryptProtectData(
				&DataIn,
				descriptionName.c_str(),
				pDataKey,
				NULL,
				NULL,
				0,
				&DataOut))
			{
				DBG_PRINTF_FL((L"CFileEnvryption::SaveMarcoDataW CryptProtectData error is %d\n", GetLastError()));
				return false;
			}

			HANDLE hFILE = CreateFile(filePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, NULL, NULL);
			if (hFILE == INVALID_HANDLE_VALUE)
			{
				DBG_PRINTF_FL((L"CFileEnvryption::SaveMarcoDataW CreateFile error is %d\n", GetLastError()));
				return false;
			}

			DWORD dwWrite = 0;
			for (unsigned int i = 0; i < DataOut.cbData; i++)
			{
				WriteFile(hFILE, &DataOut.pbData[i], 1, &dwWrite, NULL);
			}
			CloseHandle(hFILE);

			LocalFree(DataOut.pbData);

			return TRUE;
		}
		catch (...)
		{
			DBG_PRINTF_FL((L"CFileEnvryption::SaveMarcoDataW exception %d\n", GetLastError()));
		}

		return false;
	}

	bool GetDataW(wstring& data, const wstring& filePath, const wstring& password)
	{
		try
		{
			if (filePath.empty() || password.empty())
				return false;

			HANDLE hFILE = CreateFile(filePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, NULL, NULL);
			if (hFILE == INVALID_HANDLE_VALUE)
			{
				DBG_PRINTF_FL((L"CFileEnvryption::GetMarcoDataW CreateFile error is %d\n", GetLastError()));
				return false;
			}

			int file_size = 0;
			file_size = GetFileSize(hFILE, NULL);
			if (file_size <= 0)
			{
				CloseHandle(hFILE);
				DBG_PRINTF_FL((L"CFileEnvryption::GetMarcoDataW GetFileSize 0\n"));
				return false;
			}

			BYTE *buffer = (BYTE*)malloc(file_size);
			if (buffer == NULL)
			{
				CloseHandle(hFILE);
				return false;
			}

			for ( int i = 0; i < file_size; i++)
			{
				DWORD dwRead;
				BYTE ch;
				ReadFile(hFILE, &ch, 1, &dwRead, NULL);
				buffer[i] = ch;
			}
			CloseHandle(hFILE);

			DATA_BLOB DataIn;
			DATA_BLOB DataVerify;
			DataIn.pbData = buffer;
			DataIn.cbData = file_size;
			LPWSTR pDescrOut = NULL;

			PDATA_BLOB pDataKey = NULL;
			DATA_BLOB DataKey;

			DataKey.pbData = (BYTE *)password.c_str();
			DataKey.cbData = (DWORD)password.size() * sizeof(WCHAR);
			pDataKey = &DataKey;

			if (!CryptUnprotectData(
				&DataIn,
				&pDescrOut,
				pDataKey,
				NULL,
				NULL,
				0,
				&DataVerify))
			{
				DBG_PRINTF_FL((L"CFileEnvryption::GetMarcoDataW CryptUnprotectData error is %d\n", GetLastError()));
				if (buffer != NULL)
					free(buffer);
				return false;
			}

			BYTE *returnbuffer = (BYTE*)malloc(DataVerify.cbData + 2);
			if (returnbuffer == NULL)
				return false;

			SecureZeroMemory(returnbuffer, DataVerify.cbData + 2);
			memcpy_s(returnbuffer, DataVerify.cbData + 2, DataVerify.pbData, DataVerify.cbData);
			data = (LPCWSTR)returnbuffer;
			if (returnbuffer != NULL)
				free(returnbuffer);

			LocalFree(pDescrOut);
			LocalFree(DataVerify.pbData);

			if (buffer != NULL)
				free(buffer);

			return TRUE;
		}
		catch (...)
		{
			DBG_PRINTF_FL((L"CFileEnvryption::SaveMarcoDataW exception error is %d\n", GetLastError()));
			DeleteFile(filePath.c_str());
		}

		return false;
	}

	bool SaveDataWNoPassWord(LPCWSTR data, DWORD length, const wstring& filePath, const wstring& descriptionName)
	{
		try
		{
			if (filePath.empty() || descriptionName.empty())
				return false;

			DATA_BLOB DataIn;
			DATA_BLOB DataOut;
			BYTE *pbDataInput = (BYTE*)data;
			DWORD cbDataInput = length + 1;
			DataIn.pbData = pbDataInput;
			DataIn.cbData = cbDataInput;

			if (!CryptProtectData(
				&DataIn,
				descriptionName.c_str(),
				NULL,
				NULL,
				NULL,
				0,
				&DataOut))
			{
				DBG_PRINTF_FL((L"CFileEnvryption::SaveMarcoDataW CryptProtectData error is %d\n", GetLastError()));
				return false;
			}

			HANDLE hFILE = CreateFile(filePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, NULL, NULL);
			if (hFILE == INVALID_HANDLE_VALUE)
			{
				DBG_PRINTF_FL((L"CFileEnvryption::SaveMarcoDataW CreateFile error is %d\n", GetLastError()));
				return false;
			}

			DWORD dwWrite = 0;
			for (unsigned int i = 0; i < DataOut.cbData; i++)
			{
				WriteFile(hFILE, &DataOut.pbData[i], 1, &dwWrite, NULL);
			}
			CloseHandle(hFILE);

			LocalFree(DataOut.pbData);

			return TRUE;
		}
		catch (...)
		{
			DBG_PRINTF_FL((L"CFileEnvryption::SaveMarcoDataW exception %d\n", GetLastError()));
		}

		return false;
	}

	bool GetDataWNoPassWord(wstring& data, const wstring& filePath)
	{
		try
		{
			if (filePath.empty())
				return false;

			HANDLE hFILE = CreateFile(filePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, NULL, NULL);
			if (hFILE == INVALID_HANDLE_VALUE)
			{
				DBG_PRINTF_FL((L"CFileEnvryption::GetMarcoDataW CreateFile error is %d\n", GetLastError()));
				return false;
			}

			int file_size = 0;
			file_size = GetFileSize(hFILE, NULL);
			if (file_size <= 0)
			{
				CloseHandle(hFILE);
				DBG_PRINTF_FL((L"CFileEnvryption::GetMarcoDataW GetFileSize 0\n"));
				return false;
			}

			BYTE *buffer = (BYTE*)malloc(file_size);
			if (buffer == NULL)
			{
				CloseHandle(hFILE);
				return false;
			}

			for ( int i = 0; i < file_size; i++)
			{
				DWORD dwRead;
				BYTE ch;
				ReadFile(hFILE, &ch, 1, &dwRead, NULL);
				buffer[i] = ch;
			}
			CloseHandle(hFILE);

			DATA_BLOB DataIn;
			DATA_BLOB DataVerify;
			DataIn.pbData = buffer;
			DataIn.cbData = file_size;
			LPWSTR pDescrOut = NULL;

			if (!CryptUnprotectData(
				&DataIn,
				&pDescrOut,
				NULL,
				NULL,
				NULL,
				0,
				&DataVerify))
			{
				DBG_PRINTF_FL((L"CFileEnvryption::GetMarcoDataW CryptUnprotectData error is %d\n", GetLastError()));
				if (buffer != NULL)
					free(buffer);
				return false;
			}

			BYTE *returnbuffer = (BYTE*)malloc(DataVerify.cbData + 2);
			if (returnbuffer == NULL)
				return false;

			SecureZeroMemory(returnbuffer, DataVerify.cbData + 2);
			memcpy_s(returnbuffer, DataVerify.cbData + 2, DataVerify.pbData, DataVerify.cbData);
			data = (LPCWSTR)returnbuffer;
			if (returnbuffer != NULL)
				free(returnbuffer);

			LocalFree(pDescrOut);
			LocalFree(DataVerify.pbData);

			if (buffer != NULL)
				free(buffer);

			return TRUE;
		}
		catch (...)
		{
			DBG_PRINTF_FL((L"CFileEnvryption::SaveMarcoDataW exception error is %d\n", GetLastError()));
			DeleteFile(filePath.c_str());
		}

		return false;
	}
};

