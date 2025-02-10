// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <windows.h>
#define CERT_CHAIN_PARA_HAS_EXTRA_FIELDS

#include <wincrypt.h>
#include <Wintrust.h>
#include <softpub.h>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include "UserPrivilegeUtil.h"

#pragma comment(lib, "wintrust.lib")
#pragma comment(lib, "Crypt32.lib")



#define ENCODING (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING)

using namespace std;

//security check
//#define SIGN_CHECK

typedef struct {
	LPWSTR lpszProgramName;
	LPWSTR lpszPublisherLink;
	LPWSTR lpszMoreInfoLink;
} SPROG_PUBLISHERINFO, *PSPROG_PUBLISHERINFO;


class CBinaryFileValidatorProxy
{
public:
	CBinaryFileValidatorProxy()
	{
	}
	~CBinaryFileValidatorProxy()
	{
	}

	wstring GetCompanyName(const wchar_t *lpFileName)
	{
		wchar_t *szCompanyName = GetSubjectName(lpFileName);
		if (szCompanyName) {
			wstring ret = szCompanyName;
			LocalFree(szCompanyName);
			return ret;
		}
		else {
			return L"";
		}
	}


private:

	static long GetWinVerifyTrustStatus(const wchar_t *szFile)
	{
		long trustStatus = -1;

		try
		{
			if (nullptr != szFile)
			{
				WINTRUST_DATA winData;
				WINTRUST_FILE_INFO fileData;

				SecureZeroMemory(&fileData, sizeof fileData);

				fileData.cbStruct = sizeof(fileData);
				fileData.pcwszFilePath = szFile;

				SecureZeroMemory(&winData, sizeof winData);

				winData.cbStruct = sizeof(winData);
				winData.dwUIChoice = WTD_UI_NONE;
				winData.fdwRevocationChecks = WTD_REVOKE_NONE;
				winData.dwProvFlags = WTD_REVOCATION_CHECK_NONE;
				winData.dwUnionChoice = WTD_CHOICE_FILE;
				winData.pFile = &fileData;

				GUID gID = WINTRUST_ACTION_GENERIC_VERIFY_V2;

				trustStatus = WinVerifyTrust((HWND)INVALID_HANDLE_VALUE, &gID, &winData);
			}
		}
		catch (...)
		{
		}

		return trustStatus;
	}




	static wchar_t *GetSubjectName(const wchar_t *lpFileName)
	{
		if (lpFileName == NULL) {
			return NULL;
		}
		wchar_t *szSubjectName = NULL;

		WCHAR szFileName[MAX_PATH] = { 0 };
		HCERTSTORE hStore = NULL;
		HCRYPTMSG hMsg = NULL;
		PCCERT_CONTEXT pCertContext = NULL;
		BOOL fResult;
		DWORD dwEncoding, dwContentType, dwFormatType;
		PCMSG_SIGNER_INFO pSignerInfo = NULL;
		DWORD dwSignerInfo = 0;
		CERT_INFO CertInfo;

		wcscpy_s(szFileName, lpFileName);

		while (true) {

			// Get message handle and store handle from the signed file.
			fResult = CryptQueryObject(CERT_QUERY_OBJECT_FILE,
				szFileName,
				CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED,
				CERT_QUERY_FORMAT_FLAG_BINARY,
				0,
				&dwEncoding,
				&dwContentType,
				&dwFormatType,
				&hStore,
				&hMsg,
				NULL);

			if (!fResult)	{
				break;
			}

			// Get signer information size.
			fResult = CryptMsgGetParam(hMsg,
				CMSG_SIGNER_INFO_PARAM,
				0,
				NULL,
				&dwSignerInfo);

			if (!fResult)	{
				break;
			}

			// Allocate memory for signer information.
			pSignerInfo = (PCMSG_SIGNER_INFO)LocalAlloc(LPTR, dwSignerInfo);

			if (!pSignerInfo)	{
				break;
			}

			// Get Signer Information.
			fResult = CryptMsgGetParam(hMsg,
				CMSG_SIGNER_INFO_PARAM,
				0,
				(PVOID)pSignerInfo,
				&dwSignerInfo);

			if (!fResult)	{
				break;
			}

			// Search for the signer certificate in the temporary 
			// certificate store.
			CertInfo.Issuer = pSignerInfo->Issuer;
			CertInfo.SerialNumber = pSignerInfo->SerialNumber;

			pCertContext = CertFindCertificateInStore(hStore,
				ENCODING,
				0,
				CERT_FIND_SUBJECT_CERT,
				(PVOID)&CertInfo,
				NULL);

			if (!pCertContext)	{
				break;
			}

			szSubjectName = GetCertificateInfo(pCertContext);
			break;

		}
	{
		if (pSignerInfo != NULL) LocalFree(pSignerInfo);
		if (pCertContext != NULL) CertFreeCertificateContext(pCertContext);
		if (hStore != NULL) CertCloseStore(hStore, 0);
		if (hMsg != NULL) CryptMsgClose(hMsg);
	}

	return szSubjectName;
	}


	static wchar_t *GetCertificateInfo(PCCERT_CONTEXT pCertContext)
	{
		BOOL fReturn = FALSE;
		LPTSTR szName = NULL;
		DWORD dwData;

		while (true)	{

			// Get Subject name size.
			dwData = CertGetNameString(pCertContext,
				CERT_NAME_SIMPLE_DISPLAY_TYPE,
				0,
				NULL,
				NULL,
				0);
			if (dwData == 0)		{
				break;
			}

			// Allocate memory for subject name.
			szName = (LPTSTR)LocalAlloc(LPTR, dwData * sizeof(TCHAR));
			if (!szName)		{
				break;
			}

			// Get subject name.
			if (!(CertGetNameString(pCertContext,
				CERT_NAME_SIMPLE_DISPLAY_TYPE,
				0,
				NULL,
				szName,
				dwData)))		{
				break;
			}
			// Print Subject Name.
			fReturn = TRUE;
			break;
		}

		return szName;
	}
	
};
