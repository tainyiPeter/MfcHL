// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

// GetSMBIOSInfo.cpp : Defines the exported functions for the DLL application.
//
#pragma once

#include <stdio.h>
#include <windows.h>  // Really only need <WinDef.h> and <Winbase.h>
#include <tchar.h>
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include "shlobj.h"
#include "DebugUtil.h"

// ***********************************************************************
// SMBIOS Structures AND Functions
//
// To help handle the CheckBIOSVersion routine
//

//
// GetSystemFirmwareTable
// From MS -> http://msdn.microsoft.com/en-us/library/windows/desktop/ms724379(v=vs.85).aspx
//
// Funny! - This struct FROM MICROSOFT DOCUMENTATION causes a compiler warning! - Ignore it
#pragma warning( push )
#pragma warning( disable : 4200 )
#pragma pack(1)
struct RawSMBIOSData
{
	BYTE    Used20CallingMethod;
	BYTE    SMBIOSMajorVersion;
	BYTE    SMBIOSMinorVersion;
	BYTE    DmiRevision;
	DWORD   Length;
	BYTE    SMBIOSTableData[];
};
#pragma warning( pop )

// SMBios stucture common header
typedef struct _SMBIOSHeader
{
	BYTE structType;
	BYTE structLength;
	WORD structHandle;
} SMBIOSHeader;

// ABBREVIATED Type 0 "BIOS Information" Structure
// I ONLY added the info I need... there is WAY MORE after this in the REAL Type 0 structure!!!
typedef struct _SMBIOSType0
{
	SMBIOSHeader header;
	BYTE         vendor;
	BYTE         BIOSVersion;
	WORD         BIOSStartAddrSeg;
	BYTE         BIOSReleaseDate;
	BYTE         BIOSROMSize;
	// ... more in the real structure ... but I don't care about them ...
} SMBIOSType0;

typedef struct _SMBIOSType1
{
	SMBIOSHeader header;
	BYTE manufacturer;
	BYTE productName;
	BYTE version;
	BYTE serialNumber;
	BYTE UUID[16];
	BYTE wake;
	BYTE SKUNumber;
	BYTE Family;
	// ... more in the real structure ... but I don't care about them ...
} SMBIOSType1;

// Again, an ABBREVIATED Type 2 "Baseboard Information" Structure
// I ONLY added the info I need... there is WAY MORE after this in the REAL Type 2 structure!!!
typedef struct _SMBIOSType2
{
	SMBIOSHeader header;
	BYTE manufacturer;
	BYTE productName;
	BYTE version;
	BYTE serialNumber;
	BYTE assetTagNumber;
	// ... more in the real structure ... but I don't care about them ...
} SMBIOSType2;

// Again, an ABBREVIATED Type 3 "System Enclosure Information" Structure
// I ONLY added the info I need... there is WAY MORE after this in the REAL Type 3 structure!!!
typedef struct _SMBIOSType3
{
	SMBIOSHeader header;
	BYTE manufacturer;
	BYTE type;
	BYTE version;
	BYTE serialNumber;
	BYTE assetTagNumber;
	// ... more in the real structure ... but I don't care about them ...
} SMBIOSType3;

typedef struct _SMBIOSType17
{
	SMBIOSHeader header;
	WORD PhysicalMemoryHandle;
	WORD MemoryErrInfo;
	WORD TotalWidth;
	WORD DataWiddth;
	WORD Size;
	BYTE FormFactor;
	BYTE DeviceSet;
	BYTE DeviceLocator;
	BYTE BankLocator;
	BYTE MemoryType;
	WORD TypeDetail;
	WORD Speed;
	BYTE Vendor;

	BYTE SerialNub;
	BYTE assertTag;
	BYTE PartNumber;
	// ... more in the real structure ... but I don't care about them ...
} SMBIOSType17;

#pragma pack()


class MachineInformation
{
public:
	//TYPE 1
	TCHAR biosVersion[2048] = {0};
	TCHAR biosDate[2048] = { 0 };
	TCHAR ecVersion[2048] = { 0 };
	TCHAR manufacturer[2048] = { 0 };
	TCHAR version[2048] = { 0 };
	TCHAR serialNumber[2048] = { 0 };
	TCHAR modelNumber[2048] = { 0 };
	TCHAR SKUNumber[2048] = { 0 };
	TCHAR family[2048] = { 0 };

	//TYPE 3
	//BYTE manufacturer;
	TCHAR type[2048] = { 0 };
	//BYTE version;
	//BYTE serialNumber;
	TCHAR assetTagNumber[2048] = { 0 };

	//TYPE 2	
	//BYTE manufacturer;
	TCHAR productName[2048] = { 0 };
	TCHAR ideaSerialNumber[2048] = { 0 };
	//BYTE version;
	//BYTE serialNumber;
	//BYTE assetTagNumber;
	//Type 17
	DWORD MemorySize = 0;
	BYTE MemoryType = 0;
	TCHAR MemoryVendor[2048] = { 0 };
	TCHAR PartNub[2048] = { 0 };
};

class MachineInformationHelper
{
public:
	 MachineInformationHelper()
	{
		CheckMachineInformationAvailability();
	}
	 ~MachineInformationHelper()
	 {
	 }

private:
	MachineInformation info;


	// Quick easy SMBIOS "helper function" to jump over structures in the SMBios table
	int GetLengthOfStructure(BYTE *p)
	{
		int len = (WORD)((SMBIOSHeader *)p)->structLength;
		while (*((WORD *)(p + len)) != 0x0000) len++;
		return len + 2;
	}

	// Another SMBios "helper function" to get the string from the bottom of the SMBios structure	
	BYTE *GetString(int strNum, BYTE *buf)
	{
		int foundStrNum = 1;
		BYTE *strPtr = buf + (*(buf + 1));  // go directly to start of string space (if any)
		if ((strNum == 0) || ((*strPtr) == '\0'))
			return NULL;
		if (strNum != 1)
		{
			while (foundStrNum != strNum)
			{
				while ((*strPtr) != '\0') strPtr++;
				strPtr++;
				foundStrNum++;
			}
			return strPtr;
		}
		else
			return strPtr;
	}

	// Another SMBios "helper function"
	// This will return a string to match the BYTE value of the enclosure type
	const TCHAR *getChassisType(BYTE t)
	{
		switch (t & 0x7F)  // Remove chassis lock present bit
		{
		case 1:    return TEXT("Other");
		case 2:    return TEXT("Unknown");
		case 3:    return TEXT("Desktop");
		case 4:    return TEXT("Low Profile Desktop");
		case 5:    return TEXT("Pizza Box");
		case 6:    return TEXT("Mini Tower");
		case 7:    return TEXT("Tower");
		case 8:    return TEXT("Portable");
		case 9:    return TEXT("LapTop");
		case 0xA:  return TEXT("Notebook");
		case 0xB:  return TEXT("HandHeld");
		case 0xC:  return TEXT("Docking Station");
		case 0xD:  return TEXT("All In One");
		case 0xE:  return TEXT("SubNotebook");
		case 0xF:  return TEXT("Space Saving");
		case 0x10: return TEXT("Lunch Box");
		case 0x11: return TEXT("Main Server Chassis");
		case 0x12: return TEXT("Expansion Chassis");
		case 0x13: return TEXT("Sub Chassis");
		case 0x14: return TEXT("Bus Expansion Chassis");
		case 0x15: return TEXT("Peripheral Chassis");
		case 0x16: return TEXT("RAID Chassis");
		case 0x17: return TEXT("Rack Mount Chassis");
		case 0x18: return TEXT("Sealed-case PC");
		case 0x19: return TEXT("Multi-system chassis");
		case 0x1A: return TEXT("Compact PCI");
		case 0x1B: return TEXT("Advanced TCA");
		case 0x1C: return TEXT("Blade");
		case 0x1D: return TEXT("Blade Enclosure");

			/*New values from new SMBIOS spec, [WINAPPFRAM-1213] */
		case 0x1F: return TEXT("Convertible");
		case 0x1E: return TEXT("Tablet");
		case 0x20: return TEXT("Detachable");
		case 0x23: return TEXT("Mini PC");
		case 0x24: return TEXT("Stick PC");


		default:   return TEXT("Undefined");
		}
	}



	bool CheckMachineInformationAvailability()
	{

		int iReturn = -1;

		DWORD dwBytesNeeded = GetSystemFirmwareTable('RSMB', NULL, NULL, 0);
		if (0 < dwBytesNeeded)
		{
			BYTE *SMBiosTable = (BYTE *)calloc(dwBytesNeeded, sizeof(BYTE));
			if (NULL != SMBiosTable)
			{
				DWORD dwBytes = GetSystemFirmwareTable('RSMB', NULL, SMBiosTable, dwBytesNeeded);
				if ((0 < dwBytes) && (dwBytesNeeded == dwBytes))
				{
					iReturn = 4;

					TCHAR printString[2048];  // Big buffer for printing the values


					RawSMBIOSData *SMBiosData = (RawSMBIOSData *)SMBiosTable;
					BYTE *bufPtr = SMBiosData->SMBIOSTableData;
					for (DWORD curLen = 0; (curLen < SMBiosData->Length) && bufPtr != NULL;)
					{
						if (0 == *bufPtr)  // Look for the Type 0 structure
						{
							SMBIOSType0 buf;
							memcpy_s(&buf, sizeof(SMBIOSType0), bufPtr, sizeof(SMBIOSType0));

							// BIOS Version
							MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)GetString((UINT)buf.BIOSVersion, bufPtr), -1, printString, 2048);
							_tcscpy_s(info.biosVersion,2048, printString);
							//break;
						}
						else if (1 == *bufPtr)  // Look for the Type 1 structure
						{
							SMBIOSType1 buf;
							memcpy_s(&buf, sizeof(SMBIOSType1), bufPtr, sizeof(SMBIOSType1));

							// System Information - Manufacturer
							MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)GetString((UINT)buf.manufacturer, bufPtr), -1, printString, 2048);
							_tcscpy_s(info.manufacturer, 2048, printString);

							// System Information - Version
							MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)GetString((UINT)buf.version, bufPtr), -1, printString, 2048);
							_tcscpy_s(info.version, printString);

							// System Information - Model Number
							MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)GetString((UINT)buf.productName, bufPtr), -1, printString, 2048);
							_tcscpy_s(info.modelNumber, printString);

							// System Information - Serial Number
							MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)GetString((UINT)buf.serialNumber, bufPtr), -1, printString, 2048);
							_tcscpy_s(info.serialNumber, printString);

							// System Information - SKU
							MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)GetString((UINT)buf.SKUNumber, bufPtr), -1, printString, 2048);
							_tcscpy_s(info.SKUNumber, printString);

							// System Information - Family
							MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)GetString((UINT)buf.Family, bufPtr), -1, printString, 2048);
							_tcscpy_s(info.family, printString);

							iReturn = 9;
							//break;
						}
						else if (2 == *bufPtr)  // Look for the Type 2 structure
						{
							SMBIOSType2 buf;
							memcpy_s(&buf, sizeof(SMBIOSType2), bufPtr, sizeof(SMBIOSType2));

							// System Information - Model Number
							MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)GetString((UINT)buf.productName, bufPtr), -1, printString, 2048);
							_tcscpy_s(info.productName, printString);

							MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)GetString((UINT)buf.serialNumber, bufPtr), -1, printString, 2048);
							_tcscpy_s(info.ideaSerialNumber, printString);

							iReturn = 0;
							//break;
						}
						else if (3 == *bufPtr)  // Look for the Type 3 structure
						{
							SMBIOSType3 buf;
							memcpy_s(&buf, sizeof(SMBIOSType3), bufPtr, sizeof(SMBIOSType3));
							_tcscpy_s(info.type, getChassisType(buf.type));

							// Asset Tag
							MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)GetString((UINT)buf.assetTagNumber, bufPtr), -1, printString, 2048);
							_tcscpy_s(info.assetTagNumber, printString);

							iReturn = 0;
							//break;
						}
						else if (17 == *bufPtr)  // Look for the Type 3 structure
						{
							CString print;
							SMBIOSType17 buf;
							memcpy_s(&buf, sizeof(SMBIOSType17), bufPtr, sizeof(SMBIOSType17));

							info.MemoryType = buf.MemoryType;
							info.MemorySize += buf.Size;

							if (buf.Size>0)
							{
								MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)GetString((UINT)buf.Vendor, bufPtr), -1, printString, 2048);
								_tcscpy_s(info.MemoryVendor, printString);

								MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)GetString((UINT)buf.PartNumber, bufPtr), -1, printString, 2048);
								_tcscpy_s(info.PartNub, printString);
						
							}
							iReturn = 0;
							//break;
						}
						curLen += GetLengthOfStructure(bufPtr);
						bufPtr += GetLengthOfStructure(bufPtr);
					}
				}
				else iReturn = 3;

				if (NULL != SMBiosTable) free(SMBiosTable);
			}
			else iReturn = 2;
		}
		else iReturn = 1;

		return true;
	}


	void GetTypeByValue(BYTE cTypeValue, wstring& strmemorytype)
	{
		if (cTypeValue == 0x0f)
			strmemorytype = _T("SDRAM");
		else if (cTypeValue == 0x10)
			strmemorytype = _T("SGRAM");
		else if (cTypeValue == 0x11)
			strmemorytype = _T("RDRAM");
		else if (cTypeValue == 0x12)
			strmemorytype = _T("DDR");
		else if (cTypeValue == 0x13)
			strmemorytype = _T("DDR2");
		else if (cTypeValue == 0x18)
			strmemorytype = _T("DDR3");
		else
			strmemorytype = _T("");
	}


#define UUID_LENGTH    16

	struct dmi_header
	{
		BYTE type;
		BYTE length;
		INT16 handle;
		BYTE *data;
	};

	const char* dmi_string(const struct dmi_header *dm, unsigned int s)
	{
		char *bp = (char *)dm;

		if (s == 0)
			return "Not Specified";

		bp += dm->length;

		while (s > 1 && *bp)
		{
			bp += strnlen_s(bp, MAX_PATH*2);
			bp++;
			s--;
		}

		if (!*bp)
			return NULL;

		return bp;
	}

public:


	//enclosureType
	wstring EnclosureType()
	{
		wstring str_enclosure = info.type;
		return str_enclosure;
	}


	void GetFormatMemoryInfo(wstring & strMemorySize, wstring & strMemoryType, wstring & strMemoryVendor, int& fMemorySize)
	{
		DWORD MemorySize = 0;
		BYTE MemoryType;
		wstring  strPartNumber;
		CString strmemory_t;

		MemorySize = info.MemorySize;
		MemoryType = info.MemoryType;
		strMemoryVendor = info.MemoryVendor;
		strmemory_t = strMemoryVendor.c_str();
		strmemory_t.MakeLower();
		if (-1 != strmemory_t.Find(L"empty")
			|| -1 != strmemory_t.Find(L"unknown")
			|| (CString(strMemoryVendor.c_str()).GetBuffer()[0] <= L'9'&&CString(strMemoryVendor.c_str()).GetBuffer()[0] >= L'0'))
				strMemoryVendor = info.PartNub;

		fMemorySize = MemorySize;
		float  fmemory = 0.0;	
		wstring  strMeasure(_T("MB"));
		if (MemorySize >= 1024)
		{
			fmemory = FLOAT(MemorySize / 1024.0);
			strMeasure = _T("GB");
		}

		WCHAR wstrValue[10];
		swprintf_s(wstrValue, 10, _T("%.1f"), ceil(fmemory));

		//	_itow_s(dmemory, wstrValue, 10);
		strMemorySize = wstrValue;
		strMemorySize += strMeasure;

		GetTypeByValue(MemoryType, strMemoryType);
	}

	BOOL GetOEMString(string &oemstring)
	{
		char coolbuf[10] = {0};
		GetOEMString(8, coolbuf);
		oemstring = coolbuf;
		return TRUE;
	}

	BOOL GetOEMString(int stringid,char* oemstring)
	{
		BOOL bRet = FALSE;

		int nSize = ::GetSystemFirmwareTable('RSMB', 0, NULL, 0);
		if (nSize == 0)
		{
			return bRet;
		}

		BYTE* bufFirmwareTable = new BYTE[nSize];
		BYTE* pOffset = NULL;
		const char* szSystemManufacturer;		
		RawSMBIOSData *pSMBIOSData;
		BYTE* pData = NULL;
		SecureZeroMemory(bufFirmwareTable, nSize);
		nSize = ::GetSystemFirmwareTable('RSMB', 0, bufFirmwareTable, nSize);
		if (nSize == 0)
		{
			delete[] bufFirmwareTable;
			return bRet;
		}

		pSMBIOSData = (RawSMBIOSData*)bufFirmwareTable;
		pData = &pSMBIOSData->SMBIOSTableData[0];

		for (pOffset = &pSMBIOSData->SMBIOSTableData[0]; pOffset<(pData + pSMBIOSData->Length);)
		{
			dmi_header* pHeader = (dmi_header*)pOffset;
			if (11 == pHeader->type && pHeader->length >= 0x05 )
			{
				if (*(BYTE*)((BYTE*)pHeader + 4) > stringid)
				{
					szSystemManufacturer = dmi_string(pHeader, stringid);
					if (szSystemManufacturer == NULL)
					{
						break;
					}
					int nCoolingModeSize = (int)strnlen_s(szSystemManufacturer, MAX_PATH * 2);
					SecureZeroMemory(oemstring, nCoolingModeSize + 1);
					memcpy_s(oemstring, nCoolingModeSize + 1, szSystemManufacturer, nCoolingModeSize);
				
					bRet = TRUE;
				}

				break;
			}

			pOffset += pHeader->length;

			for (; (*(WORD *)pOffset != 0) && (pOffset < (pData + pSMBIOSData->Length));) pOffset++;

			pOffset += 2;
		}

		delete[] bufFirmwareTable;

		return bRet;
	}

	BOOL GetCoolingMode(char * CoolingMode)
	{
		BOOL bRet = FALSE;

		int nSize = ::GetSystemFirmwareTable('RSMB', 0, NULL, 0);
		if (nSize == 0)
		{
			return bRet;
		}

		BYTE* bufFirmwareTable = new BYTE[nSize];
		BYTE* pOffset = NULL;
		const char* szSystemManufacturer;
		const char* szCoolingMode;
		BYTE bUUID[UUID_LENGTH] = { 0 };
		RawSMBIOSData *pSMBIOSData;
		BYTE* pData = NULL;
		SecureZeroMemory(bufFirmwareTable, nSize);
		nSize = ::GetSystemFirmwareTable('RSMB', 0, bufFirmwareTable, nSize);
		if (nSize == 0)
		{
			delete[] bufFirmwareTable;
			return bRet;
		}

		pSMBIOSData = (RawSMBIOSData*)bufFirmwareTable;
		pData = &pSMBIOSData->SMBIOSTableData[0];

		for (pOffset = &pSMBIOSData->SMBIOSTableData[0]; pOffset<(pData + pSMBIOSData->Length);)
		{
			dmi_header* pHeader = (dmi_header*)pOffset;
			if (11 == pHeader->type && pHeader->length >= 0x05)
			{
				szSystemManufacturer = dmi_string(pHeader, pOffset[0x04]);

				szCoolingMode = dmi_string(pHeader, pOffset[0x07]);

				memcpy_s(bUUID, UUID_LENGTH, pOffset + 0x08, UUID_LENGTH);

				if (szSystemManufacturer == NULL)
				{
					break;
				}
				int nCoolingModeSize = (int)strnlen_s(szSystemManufacturer, MAX_PATH * 2);
				SecureZeroMemory(CoolingMode, nCoolingModeSize + 1);
				memcpy_s(CoolingMode, nCoolingModeSize + 1, szSystemManufacturer, nCoolingModeSize);

				for (int i = 0; i<UUID_LENGTH; i++)
				{
					//
				}

				bRet = TRUE;

				break;
			}

			pOffset += pHeader->length;

			for (; (*(WORD *)pOffset != 0) && (pOffset < (pData + pSMBIOSData->Length));) pOffset++;

			pOffset += 2;
		}

		delete[] bufFirmwareTable;

		return bRet;
	}

	int GetFamilyName(wstring &familyname)
	{
		WCHAR rntStr[256] = { 0 };
		wcscpy_s(rntStr, 256, info.family);
		familyname = rntStr;

		return 1;
	}

	BOOL IsSupportWarterCooling()
	{
		char coolingtype[256] = { 0 };
		SecureZeroMemory(coolingtype, 256);
		if (GetOEMString(7, coolingtype))
		{
			if (coolingtype[3] == '0')
				return TRUE;
		}
		return FALSE;
	}

	int BiosVersion(WCHAR* rntStr)
	{
		//TCHAR* biosVal = info->biosVersion;
#ifdef UNICODE
		if (rntStr == NULL)
		{
			return 0;
		}
		wcscpy_s(rntStr, 256, info.biosVersion);
		//CopyInformation(rntStr, biosVal);
		//rntStr=buffer;
		//strcpy(rntStr, buffer); //dest, source
		//strcpy_s(rntStr, sizeof(buffer)*20, buffer);
		//delete [] buffer;
#else
		std::string string(text);
#endif

		return 1;
	}

	int ModelNumber(wchar_t* rntStr)
	{
		//TCHAR* modNumVal = info.modelNumber;
#ifdef UNICODE
		if (rntStr == NULL)
		{
			return 0;
		}
		wcscpy_s(rntStr, 256, info.modelNumber);
		//CopyInformation(rntStr, modNumVal);
		//rntStr=buffer;
		//strcpy(rntStr, buffer);
		//strcpy(rntStr, buffer);
		//delete [] buffer;
#else
		std::string string(text);
#endif

		return 0;
	}

#if 0
	int Manufacturer(char* rntStr)
	{
		TCHAR* manufVal = info->manufacturer;
#ifdef UNICODE
		CopyInformation(rntStr, manufVal);
		//rntStr=buffer;
		//strcpy(rntStr, buffer);
		//strcpy(rntStr, buffer);
		//delete [] buffer;
#else
		std::string string(text);
#endif

		return 0;
	}

	int SerialNumber(char* rntStr)
	{
		TCHAR* serNumVal = info->serialNumber;
#ifdef UNICODE
		CopyInformation(rntStr, serNumVal);
		//rntStr=buffer;
		//strcpy(rntStr, buffer);

		//delete [] buffer;
#else
		std::string string(text);
#endif

		return 0;
	}

	int IdeaSerialNumber(char* rntStr)
	{
		TCHAR* serNumVal = info->ideaSerialNumber;
#ifdef UNICODE
		CopyInformation(rntStr, serNumVal);
#else
		std::string string(text);
#endif

		return 0;
	}

	int SystemVersion(char* rntStr)
	{
		TCHAR* versNum = info->version;
#ifdef UNICODE
		CopyInformation(rntStr, versNum);
#else
		std::string string(text);
#endif

		return 0;
	}

	//SKU
	int SKU(char* rntStr)
	{
		TCHAR* SKUnum = info->SKUNumber;
#ifdef UNICODE
		CopyInformation(rntStr, SKUnum);
#else
		std::string string(text);
#endif

		return 0;
	}

	//family
	int Family(char* rntStr)
	{
		TCHAR* family = info->family;
#ifdef UNICODE
		CopyInformation(rntStr, family);
#else
		std::string string(text);
#endif

		return 0;
	}

	//AssetTagNumber
	int AssetTagNumber(char* rntStr)
	{
		TCHAR* assetTagNum = info->type;
#ifdef UNICODE
		CopyInformation(rntStr, assetTagNum);
#else
		std::string string(text);
#endif

		return 0;
	}

	//Product Name
	int ProductName(char* rntStr)
	{
		TCHAR* product = info->type;
#ifdef UNICODE
		CopyInformation(rntStr, product);
#else
		std::string string(text);
#endif

		return 0;
	}
#endif
};
