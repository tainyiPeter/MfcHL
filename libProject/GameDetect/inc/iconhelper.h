// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include <gdiplus.h>
#include "Shellapi.h"
#include "base64.h"
#include "DebugUtil.h"
#pragma comment (lib, "Gdiplus.lib")
#pragma comment(lib,"Shell32.lib") 

#define MaxIconSize 1024 * 100

class CIconhelper
{
public:
	CIconhelper(CString striconfolder=L"icon"):m_GDIPlusToken(NULL)
				, m_strImagePath(""), m_strIconFolder(striconfolder)
	{
		Init();
	}
	~CIconhelper()
	{
		if (m_GDIPlusToken != NULL)
			Gdiplus::GdiplusShutdown(m_GDIPlusToken);
	}

	bool SaveProcessIcon(const wstring strPidPath, const wstring strPidIconName)
	{
		if (strPidPath.empty() || strPidIconName.empty())
			return false;

		CString strIconPath = m_strImagePath + L"\\" + GetFileName(strPidIconName).c_str();
	
		if (!PathFileExists(strIconPath.GetBuffer()))
		    return SaveFileIcon(strPidPath.c_str(), strIconPath);
		return true;
	}

	wstring GetIconPath()
	{
		return m_strImagePath.GetBuffer();
	}

	wstring GetPidIconName(const wstring& strPidPath)
	{
		if (strPidPath.empty())
			return _T("");

		INT64 hashValue = hashCode(strPidPath.c_str());
		CString strwtemp;
		INT64 hashValueHight = hashValue >> 32;//  0xFFFFFFFF00000000;
		INT64 hashValueLow = hashValue & 0x00000000FFFFFFFF;
		strwtemp.Format(L"%s\\%lx%lx", m_strIconFolder.GetBuffer(), hashValueHight, hashValueLow); //icon current folder name
		strwtemp += L".png";
		return strwtemp.GetBuffer();
	}

	
private:

	size_t hashCode(const wstring& _Keyval)
	{
		if (_Keyval.empty())
			return 0;

		WCHAR tempString[MAX_PATH + 27] = { 0 };
		// add template string when string size doesn't align, not cut off the string.
		swprintf_s(tempString, L"%ls", _Keyval.c_str());
		size_t _Val = 2166136261U;

		size_t _First = 0;
		size_t _Last = _Keyval.size();	//9
		size_t _Stride = 8;
		//  string size not cut off	some of last words 	
		//	if (_Stride < _Last)
		//		_Last -= _Stride;
		for (; _First < _Last; _First += _Stride)
		{
			INT64 nPathInt64 = (INT64)tempString[_First];
			nPathInt64 |= (INT64)(tempString[_First + 1] & 0x00FF) << 8;
			nPathInt64 |= (INT64)(tempString[_First + 2] & 0x00FF) << 16;
			nPathInt64 |= (INT64)(tempString[_First + 3] & 0x00FF) << 24;
			nPathInt64 |= (INT64)(tempString[_First + 4] & 0x00FF) << 32;
			nPathInt64 |= (INT64)(tempString[_First + 5] & 0x00FF) << 40;
			nPathInt64 |= (INT64)(tempString[_First + 6] & 0x00FF) << 48;
			nPathInt64 |= (INT64)(tempString[_First + 7] & 0x00FF) << 56;
			_Val = 16777619U * _Val ^ (size_t)nPathInt64;
		}
		return (_Val);
	}

	wstring GetLocalAppDataPath()
	{
		return CRegUtil().GetKeyValueStr(HKEY_CURRENT_USER,
			L"Volatile Environment",
			_T("LOCALAPPDATA"));
	}

	void Init()
	{
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&m_GDIPlusToken, &gdiplusStartupInput, NULL);

		wstring retString = CRegUtil().GetKeyValueStr(HKEY_LOCAL_MACHINE,
			GAMEZONE_LOG,
			GAMEZONE_ICONPATH);
		if (!retString.empty())
		{
			m_strImagePath.Format(L"%s\\%s",
				retString.c_str(), m_strIconFolder.GetBuffer());
		}
		else
		{
			std::wstring name;
			name = GetLocalAppDataPath();
			m_strImagePath.Format(L"%s\\Packages\\E046963F.LenovoCompanion_k1h2ywk1493x8\\LocalState\\%s",
				name.c_str(), m_strIconFolder.GetBuffer());
		}

		DBG_PRINTF_FL((L"CIconhelper::ImagePath %s\n", m_strImagePath.GetBuffer()));
	}

	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
	{
		UINT  num = 0;          // number of image encoders
		UINT  size = 0;         // size of the image encoder array in bytes
		Gdiplus::Status status = Gdiplus::Ok;
		Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

		status = Gdiplus::GetImageEncodersSize(&num, &size);
		if (status != Gdiplus::Ok)
			return -1;
		if(size == 0)
			return -1;  // Failure
		pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
		if(pImageCodecInfo == NULL)
			return -1;  // Failure

		status = GetImageEncoders(num, size, pImageCodecInfo);
		if (status != Gdiplus::Ok)
			return -1;

		for(UINT j = 0; j < num; ++j)
		{
#pragma warning(suppress: 6385)
			if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
			{
				*pClsid = pImageCodecInfo[j].Clsid;
				free(pImageCodecInfo);
				return j;  // Success
			}    
		}

		free(pImageCodecInfo);
		return -1;  // Failure
	}

	bool SaveHIcon2PngFile(HICON hIcon, LPCTSTR lpszPicFileName)
	{  
  			    
		ICONINFO icInfo = { 0 };	
		if (!::GetIconInfo(hIcon, &icInfo))
		{
			return false;
		}

		BITMAP bitmap; 
		GetObject(icInfo.hbmColor, sizeof(BITMAP), &bitmap);

		Gdiplus::Bitmap* pBitmap = NULL;
		Gdiplus::Bitmap* pWrapBitmap = NULL;
		if (bitmap.bmBitsPixel != 32) 
		{   
			pBitmap = Gdiplus::Bitmap::FromHICON(hIcon); 
		} 
		else
		{
			pWrapBitmap = Gdiplus::Bitmap::FromHBITMAP(icInfo.hbmColor, NULL);
			if (!pWrapBitmap)
				return false;

			Gdiplus::BitmapData bitmapData;
			Gdiplus::Rect rcImage(0, 0, pWrapBitmap->GetWidth(), pWrapBitmap->GetHeight());
			pWrapBitmap->LockBits(&rcImage, Gdiplus::ImageLockModeRead, pWrapBitmap->GetPixelFormat(), &bitmapData); 
			pBitmap = new Gdiplus::Bitmap(bitmapData.Width, bitmapData.Height, bitmapData.Stride, PixelFormat32bppARGB, (BYTE*)bitmapData.Scan0);
			pWrapBitmap->UnlockBits(&bitmapData);
		}

		CLSID encoderCLSID;
		SecureZeroMemory(&encoderCLSID, sizeof(CLSID));
		bool bRet = true;
		if (GetEncoderClsid(_T("image/png"), &encoderCLSID) != -1)
		{
			if (pBitmap != NULL)
			{
				Gdiplus::Status st = pBitmap->Save(lpszPicFileName, &encoderCLSID, NULL/*&encoderParameters*/);
				if (st != Gdiplus::Ok)
				{
					bRet = false;
				}
			}
		}
		if (pBitmap)
			delete pBitmap;
		if (pWrapBitmap)
			delete pWrapBitmap;
		DeleteObject(icInfo.hbmColor);
		DeleteObject(icInfo.hbmMask);

		return bRet;
	}

	
	HICON GetFileIcon(const CString& strFilePath, BOOL bLarge)
	{
		SHFILEINFO SHFI;
		SecureZeroMemory(&SHFI, sizeof(SHFI));
		DWORD_PTR ret = ::SHGetFileInfo(strFilePath, 0, &SHFI, sizeof(SHFI), 
			SHGFI_ICON | (bLarge ? SHGFI_LARGEICON : SHGFI_SMALLICON));

		if (ret != 0)
		{
			return SHFI.hIcon;
		}

		return NULL;
	}

	BOOL SaveFileIcon(const CString& strFilePath, const CString& strImagePath, BOOL bLarge = TRUE)
	{
		BOOL bRet = FALSE;
		HICON hIcon = GetFileIcon(strFilePath, bLarge);
		if (hIcon != NULL)
		{
			bRet = SaveHIcon2PngFile(hIcon, strImagePath);
			::DestroyIcon(hIcon);
		}

		return bRet;
	}

	//Remove the current folder name
	wstring GetFileName(const wstring& strFileName)
	{
		if (strFileName.empty())
			return _T("");
		
		wstring str(strFileName);
		int idex = (int)str.find(_T("\\"));
		return str.substr(idex + 1);
	}

public:
	bool GetIconBased64Code(const wstring strIconPath, wstring & outValue)
	{
		if (strIconPath.empty())
			return false;

		if (!PathFileExists(strIconPath.c_str()))
			return false;

		HICON hIcon = GetFileIcon(strIconPath.c_str(), TRUE);
		if (hIcon == NULL)
			return false;

		ICONINFO icInfo = { 0 };
		if (!::GetIconInfo(hIcon, &icInfo))
		{
			return false;
		}

		BITMAP bitmap;
		GetObject(icInfo.hbmColor, sizeof(BITMAP), &bitmap);

		Gdiplus::Bitmap* pBitmap = NULL;
		Gdiplus::Bitmap* pWrapBitmap = NULL;
		if (bitmap.bmBitsPixel != 32)
		{
			pBitmap = Gdiplus::Bitmap::FromHICON(hIcon);
		}
		else
		{
			pWrapBitmap = Gdiplus::Bitmap::FromHBITMAP(icInfo.hbmColor, NULL);
			if (!pWrapBitmap)
				return false;

			Gdiplus::BitmapData bitmapData;
			Gdiplus::Rect rcImage(0, 0, pWrapBitmap->GetWidth(), pWrapBitmap->GetHeight());
			pWrapBitmap->LockBits(&rcImage, Gdiplus::ImageLockModeRead, pWrapBitmap->GetPixelFormat(), &bitmapData);
			pBitmap = new Gdiplus::Bitmap(bitmapData.Width, bitmapData.Height, bitmapData.Stride, PixelFormat32bppARGB, (BYTE*)bitmapData.Scan0);
			pWrapBitmap->UnlockBits(&bitmapData);
		}

		bool bRet = true;
		CLSID encoderCLSID;
		SecureZeroMemory(&encoderCLSID, sizeof(CLSID));

		if (GetEncoderClsid(_T("image/png"), &encoderCLSID) != -1)
		{
			if (pBitmap != NULL)
			{
				IStream* pIStream = nullptr;
				if (CreateStreamOnHGlobal(NULL, TRUE, (LPSTREAM*)&pIStream) != S_OK)
				{
					bRet = false;
				}
				Gdiplus::Status st = pBitmap->Save(pIStream, &encoderCLSID);
				if (st != Gdiplus::Ok)
				{
					pIStream->Release();
					bRet = false;
				}
				else {
					STATSTG sts;
					pIStream->Stat(&sts, STATFLAG_DEFAULT);
					ULARGE_INTEGER uli = sts.cbSize;
					LARGE_INTEGER zero;
					zero.QuadPart = 0;
					int size = (int)uli.QuadPart;

					char* bits = new char[size];
					ULONG written;
					pIStream->Seek(zero, STREAM_SEEK_SET, NULL);
					pIStream->Read(bits, size, &written);

					std::string img_base64_memory = "data:image/png;base64," + base64_encode(reinterpret_cast<const unsigned char*>(bits), size);
					StringToWstring(std::move(img_base64_memory), outValue);

					delete[] bits;
					pIStream->Release();
				}

			}
		}


		if (pBitmap)
			delete pBitmap;
		if (pWrapBitmap)
			delete pWrapBitmap;
		DeleteObject(icInfo.hbmColor);
		DeleteObject(icInfo.hbmMask);

		if (hIcon != NULL)
			::DestroyIcon(hIcon);

		return bRet;
	}
private:
	ULONG_PTR m_GDIPlusToken;
	CString m_strImagePath;
	CString m_strIconFolder;
	wstring m_username;
};
