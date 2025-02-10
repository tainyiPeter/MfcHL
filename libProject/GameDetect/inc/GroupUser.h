#pragma once

#include <string>
#include <vector>
#include <windows.h>
#include <wtsapi32.h>
#include <memory>
#include <sddl.h>
#pragma comment(lib, "wtsapi32.lib")

namespace GroupUser
{
	struct CUser
	{
		std::wstring strLogonUser;
		std::wstring strDomainName;
	};

    inline void AutoWTSFreeMemory(WCHAR** p)
    {
        if (nullptr != p && nullptr != *p)
        {
            WTSFreeMemory(*p);
            *p = nullptr;
        }
    }

    inline int GetAllLogonUsers(std::vector<CUser>* pvUsers, DWORD* pdwError)
    {
        int nRet = 0;

        WTS_SESSION_INFO* pSessionInfo = nullptr;
        DWORD dwSessionCount = 0;

        do
        {
            if (nullptr == pvUsers)
            {
                nRet = -1;
                break;
            }

            if (!WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &dwSessionCount))
            {
                if (nullptr != pdwError)
                {
                    *pdwError = GetLastError();
                }

                nRet = -2;
                break;
            }

            for (DWORD i = 0; i < dwSessionCount; ++i)
            {
                WCHAR* pszUserName = nullptr;
                std::unique_ptr<WCHAR*, decltype(AutoWTSFreeMemory)*> upUserName(&pszUserName, AutoWTSFreeMemory);
                WCHAR* pszDomainName = nullptr;
                std::unique_ptr<WCHAR*, decltype(AutoWTSFreeMemory)*> upDomainName(&pszDomainName, AutoWTSFreeMemory);

                DWORD dwSessionId = pSessionInfo[i].SessionId;
                CUser user;

                DWORD cbUserNameSize = 0;
                if (!WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, dwSessionId, WTSUserName, &pszUserName, &cbUserNameSize))
                {
                    if (nullptr != pdwError)
                    {
                        *pdwError = GetLastError();
                    }

                    nRet = -3;
                    break;
                }
                user.strLogonUser = pszUserName;

                DWORD cbDomainNameSize = 0;
                if (!WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, dwSessionId, WTSDomainName, &pszDomainName, &cbDomainNameSize))
                {
                    if (nullptr != pdwError)
                    {
                        *pdwError = GetLastError();
                    }

                    nRet = -4;
                    break;
                }
                user.strDomainName = pszDomainName;

                if (!user.strLogonUser.empty())
                {
                    pvUsers->push_back(user);
                }
            }

            if (0 != nRet)
            {
                break;
            }

        } while (false);

        if (nullptr != pSessionInfo)
        {
            WTSFreeMemory(pSessionInfo);
            pSessionInfo = nullptr;
        }

        return nRet;
    }

    inline int GetCurLogonUser(std::wstring* pstrCurLogonUser, std::wstring* pstrDomainName, DWORD* pdwError)
    {
        int nRet = 0;

        WTS_SESSION_INFO* pSessionInfo = nullptr;
        WCHAR* pszUserName = nullptr;
        WCHAR* pszDomainName = nullptr;
        DWORD dwSessionCount = 0;

        do
        {
            if (nullptr == pstrCurLogonUser)
            {
                nRet = -1;
                break;
            }

            if (!WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &dwSessionCount))
            {
                if (nullptr != pdwError)
                {
                    *pdwError = GetLastError();
                }

                nRet = -2;
                break;
            }

            for (DWORD i = 0; i < dwSessionCount; ++i)
            {
                if (pSessionInfo[i].State == WTSActive)	
                {
                    DWORD dwSessionId = pSessionInfo[i].SessionId;

                    DWORD cbUserNameSize = 0;
                    if (!WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, dwSessionId, WTSUserName, &pszUserName, &cbUserNameSize))
                    {
                        if (nullptr != pdwError)
                        {
                            *pdwError = GetLastError();
                        }

                        nRet = -3;
                        break;
                    }

                    *pstrCurLogonUser = pszUserName;

                    if (nullptr != pstrDomainName)
                    {
                        DWORD cbDomainNameSize = 0;
                        if (!WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, dwSessionId, WTSDomainName, &pszDomainName, &cbDomainNameSize))
                        {
                            if (nullptr != pdwError)
                            {
                                *pdwError = GetLastError();
                            }

                            nRet = -4;
                            break;
                        }

                        *pstrDomainName = pszDomainName;
                    }

                    break;
                }
            }

            if (0 != nRet)
            {
                break;
            }

        } while (false);

        if (nullptr != pszDomainName)
        {
            WTSFreeMemory(pszDomainName);
            pszDomainName = nullptr;
        }

        if (nullptr != pszUserName)
        {
            WTSFreeMemory(pszUserName);
            pszUserName = nullptr;
        }

        if (nullptr != pSessionInfo)
        {
            WTSFreeMemory(pSessionInfo);
            pSessionInfo = nullptr;
        }

        return nRet;
    }

    // pszUserName: domain\username
    inline int GetSIDFromUserName(const wchar_t* pszUserName, std::wstring* pstrUserSID, DWORD* pdwError)
    {
        SID_NAME_USE eSidType = SidTypeUser;
        DWORD cbSidBufferSize = 0;
        DWORD cchDomainNameBufferLen = 0;
        SID* pSid = nullptr;
        wchar_t* pszDomainName = nullptr;
        wchar_t* pszSid = nullptr;
        int nRet = 0;

        do
        {
            if (nullptr == pstrUserSID)
            {
                nRet = -1;
                break;
            }

            // Must "domain\\username"
            std::wstring strUser = pszUserName;
            std::wstring::size_type nPos = strUser.find('\\');
            if (nPos == std::wstring::npos)
            {
                // Not found domain
                nRet = -2;
                break;
            }

            BOOL bRet = LookupAccountNameW(nullptr, pszUserName, nullptr, &cbSidBufferSize, nullptr, &cchDomainNameBufferLen, &eSidType);
            if (0 == cbSidBufferSize)
            {
                nRet = -3;
                break;
            }

            pSid = (SID*)new BYTE[cbSidBufferSize];
            pszDomainName = new wchar_t[cchDomainNameBufferLen];


            if (!LookupAccountNameW(nullptr, pszUserName, pSid, &cbSidBufferSize, pszDomainName, &cchDomainNameBufferLen, &eSidType))
            {
                if (nullptr != pdwError)
                {
                    *pdwError = GetLastError();
                }

                nRet = -4;
                break;
            }


            if (!ConvertSidToStringSidW(pSid, &pszSid))
            {
                if (nullptr != pdwError)
                {
                    *pdwError = GetLastError();
                }

                nRet = -5;
                break;
            }

            *pstrUserSID = pszSid;

        } while (false);

        if (nullptr != pszSid)
        {
            LocalFree(pszSid);
            pszSid = nullptr;
        }

        if (nullptr != pszDomainName)
        {
            delete[] pszDomainName;
            pszDomainName = nullptr;
        }

        if (nullptr != pSid)
        {
            delete[](BYTE*)(pSid);
            pSid = nullptr;
        }

        return nRet;
    }

}
