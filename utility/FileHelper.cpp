//#include "Stdafx.h"
#include "FileHelper.h"
#include "atlfile.h"
#include "atlpath.h"
//#include "../Tool.h"
#include <windef.h>
#include <WinVer.h>
#include <xstring>
#include <ShlObj.h>
#include <time.h>
//#include <xfunctional>
#include <functional>


#pragma comment( lib , "Version.lib" ) 
using namespace std;

typedef struct tagCSIDL_NAME
{
	LPCTSTR pszCsidlName;
	int     nFolder;
}CSIDL_NAME, * LPCSIDL_NAME;

#ifdef _UNICODE
#define CSIDL_NAME_ENTRY(ID) \
{ L#ID , ID }
#else
#define CSIDL_NAME_ENTRY(ID) \
{ #ID , ID }
#endif

CSIDL_NAME g_csidlnamelist[] =
{
	CSIDL_NAME_ENTRY(CSIDL_DESKTOP),
	CSIDL_NAME_ENTRY(CSIDL_INTERNET),
	CSIDL_NAME_ENTRY(CSIDL_PROGRAMS),
	CSIDL_NAME_ENTRY(CSIDL_CONTROLS),
	CSIDL_NAME_ENTRY(CSIDL_PRINTERS),
	CSIDL_NAME_ENTRY(CSIDL_PERSONAL),
	CSIDL_NAME_ENTRY(CSIDL_FAVORITES),
	CSIDL_NAME_ENTRY(CSIDL_STARTUP),
	CSIDL_NAME_ENTRY(CSIDL_RECENT),
	CSIDL_NAME_ENTRY(CSIDL_SENDTO),
	CSIDL_NAME_ENTRY(CSIDL_BITBUCKET),
	CSIDL_NAME_ENTRY(CSIDL_STARTMENU),
	CSIDL_NAME_ENTRY(CSIDL_DESKTOPDIRECTORY),
	CSIDL_NAME_ENTRY(CSIDL_DRIVES),
	CSIDL_NAME_ENTRY(CSIDL_NETWORK),
	CSIDL_NAME_ENTRY(CSIDL_NETHOOD),
	CSIDL_NAME_ENTRY(CSIDL_FONTS),
	CSIDL_NAME_ENTRY(CSIDL_TEMPLATES),
	CSIDL_NAME_ENTRY(CSIDL_COMMON_STARTMENU),
	CSIDL_NAME_ENTRY(CSIDL_COMMON_PROGRAMS),
	CSIDL_NAME_ENTRY(CSIDL_COMMON_STARTUP),
	CSIDL_NAME_ENTRY(CSIDL_COMMON_DESKTOPDIRECTORY),
	CSIDL_NAME_ENTRY(CSIDL_APPDATA),
	CSIDL_NAME_ENTRY(CSIDL_PRINTHOOD),
	CSIDL_NAME_ENTRY(CSIDL_ALTSTARTUP),
	CSIDL_NAME_ENTRY(CSIDL_COMMON_ALTSTARTUP),
	CSIDL_NAME_ENTRY(CSIDL_COMMON_FAVORITES),
	CSIDL_NAME_ENTRY(CSIDL_INTERNET_CACHE),
	CSIDL_NAME_ENTRY(CSIDL_COOKIES),
	CSIDL_NAME_ENTRY(CSIDL_HISTORY),
	CSIDL_NAME_ENTRY(CSIDL_PROFILE),
	CSIDL_NAME_ENTRY(CSIDL_WINDOWS),
	CSIDL_NAME_ENTRY(CSIDL_SYSTEM),
	CSIDL_NAME_ENTRY(CSIDL_LOCAL_APPDATA),
	CSIDL_NAME_ENTRY(CSIDL_COMMON_APPDATA),
	CSIDL_NAME_ENTRY(CSIDL_PROGRAM_FILESX86),
	CSIDL_NAME_ENTRY(CSIDL_PROGRAM_FILES),
	CSIDL_NAME_ENTRY(CSIDL_SYSTEMX86),
	CSIDL_NAME_ENTRY(CSIDL_PROGRAM_FILES_COMMON),
	CSIDL_NAME_ENTRY(CSIDL_MYMUSIC),
	CSIDL_NAME_ENTRY(CSIDL_MYPICTURES),
	CSIDL_NAME_ENTRY(CSIDL_COMMON_MUSIC),
	CSIDL_NAME_ENTRY(CSIDL_COMMON_PICTURES),
	CSIDL_NAME_ENTRY(CSIDL_COMMON_VIDEO),
	CSIDL_NAME_ENTRY(CSIDL_COMMON_TEMPLATES),
	CSIDL_NAME_ENTRY(CSIDL_COMMON_DOCUMENTS),
	CSIDL_NAME_ENTRY(CSIDL_RESOURCES),
	CSIDL_NAME_ENTRY(CSIDL_MYDOCUMENTS)
};


void CFileHelper::Test(CString k)
{

}

BOOL CFileHelper::IsFileExists(CString filePath )
{
	return ATLPath::FileExists( filePath.GetBuffer() );
}

CString CFileHelper::GetFileName( CString filePath )
{
	return ATLPath::FindFileName( filePath.GetBuffer() );
}

BOOL CFileHelper::IsDictoryExists( CString path )
{
	return ATLPath::IsDirectory( path.GetBuffer() );

	DWORD dwAttr = ::GetFileAttributes( path );
	if ( dwAttr == 0xFFFFFFFF )
	{
		return FALSE;
	}
	if ( ( dwAttr & FILE_ATTRIBUTE_DIRECTORY ) > 0 )
	{
		return TRUE;
	}
	return FALSE;	
}

CString CFileHelper::GetFileExtention( CString filePath )
{
	return ATLPath::FindExtension( filePath );
}

CString CFileHelper::GetFileDir(CString filePath)
{
	auto pos = filePath.ReverseFind(L'\\');
	if (pos == -1)
	{
		pos == filePath.ReverseFind(L'//');
	}
	if (pos >= 0)
	{
		return filePath.Left(pos + 1);
	}
	return L"";
}

CString CFileHelper::GetParentDir(const CString & path)
{
	if (path.Right(1) == L"\\")
	{
		int i = path.GetLength() - 2;
		for (; i >= 0; i--)
		{
			if (path[i] == '\\')
				break;
		}
		return path.Left(i);
	}
	return path.Left(path.ReverseFind('\\'));
}

CString CFileHelper::ConcatenatePath(const CString& leftPath, const CString& rightPath)
{
	if (leftPath.Right(1) == L"\\")
	{
		return leftPath + rightPath;
	}
	else
	{
		return leftPath + L"\\"  + rightPath;
	}
}

BOOL CFileHelper::Delete( CString filePath )
{
	DWORD fileAttributes = GetFileAttributes(filePath);
	fileAttributes &= ~FILE_ATTRIBUTE_READONLY;
	SetFileAttributes(filePath, fileAttributes);

	return DeleteFile( filePath );
}
BOOL CFileHelper::DeleteDir(CString dirPath)
{
	DeleteFiles(dirPath, TRUE, TRUE);
	return ::RemoveDirectory(dirPath);
}

void CFileHelper::DeleteFiles( CString folderPath, BOOL deleteSubFolderFiles, BOOL deleteSubFolder  )
{
	DeleteFilesImpl(folderPath.GetString() , deleteSubFolderFiles ,deleteSubFolder );
}

void CFileHelper::DeleteFilesImpl( const wchar_t *folderPath, BOOL deleteSubFolderFiles, BOOL deleteSubFolder )
{
	wchar_t fileFound[256];
	WIN32_FIND_DATAW info;
	HANDLE hp; 
	swprintf_s( fileFound, L"%s\\*.*", folderPath );
	hp = FindFirstFileW( fileFound, &info );
	if (hp == INVALID_HANDLE_VALUE)
	{
		return;
	}
	do
	{
		if ( !(( wcscmp(info.cFileName, L"." )==0)|| (wcscmp( info.cFileName, L".." ) ==0 )) )
		{
			if( ( info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY )
			{
				if ( deleteSubFolderFiles )
				{
					wstring subFolder = folderPath;
					subFolder.append( L"\\" );
					subFolder.append( info.cFileName );
					DeleteFilesImpl( subFolder.c_str(), deleteSubFolderFiles, deleteSubFolder );
					if ( deleteSubFolder )
					{
						::RemoveDirectoryW( subFolder.c_str() );
					}					
				}				
			}
			else
			{
				swprintf_s( fileFound,L"%s\\%s", folderPath, info.cFileName );
				BOOL retVal = ::DeleteFileW( fileFound );
			}
		}

	}while( FindNextFileW( hp, &info ) ); 
	::FindClose(hp);
}

void CFileHelper::DeleteFileEx(CString _strPathfile)
{
	const DWORD l_dwPATHLEN = 1024;
	static CString l_strTemp;
	if (l_strTemp.IsEmpty())
	{
		::GetTempPath(l_dwPATHLEN, l_strTemp.GetBuffer(l_dwPATHLEN));
		l_strTemp.ReleaseBuffer();

		if (!PathIsDirectoryW(l_strTemp))
		{
			::CreateDirectoryW(l_strTemp, 0);
		}
	}

	if (GetFileAttributesW(_strPathfile) & FILE_ATTRIBUTE_DIRECTORY)
	{
		WIN32_FIND_DATAW l_fileinfo;
		HANDLE l_hFind = INVALID_HANDLE_VALUE;
		
		if (_strPathfile[_strPathfile.GetLength() - 1] != L'\\')
			_strPathfile.Append(L"\\");

		l_hFind = FindFirstFileW(_strPathfile + L"*.*", &l_fileinfo);
		if (INVALID_HANDLE_VALUE == l_hFind)
			return;

		do
		{
			if (l_fileinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if ((_tcscmp(l_fileinfo.cFileName, L".") == 0)|| (_tcscmp(l_fileinfo.cFileName, L"..") == 0))
					continue;

				DeleteFileEx(_strPathfile + l_fileinfo.cFileName);

				if (!RemoveDirectoryW(_strPathfile + l_fileinfo.cFileName))
					::MoveFileExW(_strPathfile + l_fileinfo.cFileName, nullptr, MOVEFILE_DELAY_UNTIL_REBOOT);
			}
			else if (!::DeleteFileW(_strPathfile + l_fileinfo.cFileName))
			{
				CString l_strTempFileName = l_strTemp + l_fileinfo.cFileName;
				srand((unsigned int)(time(NULL)));
				l_strTempFileName.Format(_T("%s%d"), l_strTempFileName.GetBuffer(0), rand() % 10000);

				::MoveFileExW(_strPathfile + l_fileinfo.cFileName, l_strTempFileName, MOVEFILE_REPLACE_EXISTING);
				::MoveFileExW(l_strTempFileName, nullptr, MOVEFILE_DELAY_UNTIL_REBOOT);
			}
		} while (FindNextFileW(l_hFind, &l_fileinfo));

		if (!RemoveDirectoryW(_strPathfile))
			::MoveFileExW(_strPathfile + l_fileinfo.cFileName, nullptr, MOVEFILE_DELAY_UNTIL_REBOOT);

		::FindClose(l_hFind);
		l_hFind = INVALID_HANDLE_VALUE;
	}
	else if (!::DeleteFileW(_strPathfile))
	{
		CString deleteFile = _strPathfile + L".delete";
		::MoveFileExW(_strPathfile, deleteFile, MOVEFILE_REPLACE_EXISTING);
		::MoveFileExW(deleteFile, nullptr, MOVEFILE_DELAY_UNTIL_REBOOT);
	}	
}

BOOL CFileHelper::RenameFile(const CString& strOldName, const CString& strNewName)
{
	return MoveFile(strOldName, strNewName);
}

//bool CFileHelper::SHDeleteFiles(CString strFrom)
//{
//	TCHAR pFromTemp[MAX_PATH] = {0};  
//	_tcscpy(pFromTemp,strFrom.GetBuffer());  
//
//	SHFILEOPSTRUCT FileOp = {0};  
//	FileOp.fFlags = FOF_SILENT|FOF_NOCONFIRMATION|FOF_MULTIDESTFILES;  
//	if(false)  
//	{  
//		FileOp.fFlags |= FOF_ALLOWUNDO;  
//	}  
//	FileOp.hNameMappings = NULL;  
//	FileOp.hwnd = NULL;  
//	FileOp.lpszProgressTitle = NULL;  
//	FileOp.pFrom = pFromTemp;  
//	FileOp.pTo = NULL;  
//	FileOp.wFunc = FO_DELETE;  
//
//	return SHFileOperation(&FileOp) == 0;
//}

void CFileHelper::Move( CString sourceFilePath, CString destFilePath )
{
	::DeleteFile( destFilePath );
	::MoveFile( sourceFilePath, destFilePath );
}

void CFileHelper::Copy( CString sourceFilePath, CString destFilePath )
{
	::CopyFile( sourceFilePath, destFilePath, FALSE );
}


UINT64 CFileHelper::GetFileSize( CString filePath )
{
	CAtlFile file;
	HRESULT ret = file.Create( filePath, FILE_READ_ATTRIBUTES , FILE_SHARE_READ, OPEN_EXISTING );

	if ( !SUCCEEDED(ret) )
		return 0;

	UINT64 size;
	if ( file.GetSize( size ) == S_OK )
		return size;
	else
		return 0;
}

CString CFileHelper::GetApplicationDataPath()
{
	CString strPath;
	HRESULT hr = SHGetFolderPath( NULL , CSIDL_LOCAL_APPDATA, NULL , SHGFP_TYPE_CURRENT , strPath.GetBuffer( MAX_PATH + 1 ) );
	strPath.ReleaseBuffer();
	ATLASSERT( hr == S_OK );
	if( strPath.Right(1) !=  _T("\\") )
	{
		strPath.Append( _T("\\") );
	}
	return strPath;
}

CString CFileHelper::GetMyPicturePath()
{
	CString strPath;
	HRESULT hr = SHGetFolderPath( NULL , CSIDL_MYPICTURES , NULL , SHGFP_TYPE_CURRENT , strPath.GetBuffer( MAX_PATH + 1 ) );
	strPath.ReleaseBuffer();
	ATLASSERT( hr == S_OK );
	if( strPath.Right(1) !=  _T("\\") )
	{
		strPath.Append( _T("\\") );
	}
	return strPath;
}

BOOL CFileHelper::CreateDir( CString path )
{
	if (!PathIsDirectoryW(path))
	{
		::SHCreateDirectoryEx(NULL, path, 0);
	}
	return true;
	//std::string str = CommonHelper::ToString( path );
	//std::string str;
	//str = CT2A(path.GetBuffer()).m_psz;

	//const char* tmp = str.c_str() ;
	////return CreateDir("c:\\fmj\\fmj1\\fmj2");
	//return ( CreateDir( const_cast<char*>( tmp ) ) );
}


BOOL CFileHelper::CreateDir( char* path )
{
	char DirName[256];
	char* p = path;
	char* q = DirName; 
	while(*p)
	{
		if (('\\' == *p) || ('/' == *p))
		{
			if (':' != *(p-1))
			{
				if( !ATLPath::IsDirectory( DirName ) )
				{
					::CreateDirectoryA(DirName, NULL);
				}				
			}
		}
		*q++ = *p++;
		*q = '\0';
	}
	::CreateDirectoryA(DirName, NULL);
	return TRUE;
}

BOOL CFileHelper::EnumSubDirectories(const CString & strDirPath, std::vector<CString>& subDirectories, bool bRecursive)
{
	if (!IsDictoryExists(strDirPath))
	{
		return FALSE;
	}

	CString strTemp = strDirPath;
	if (strTemp[strTemp.GetLength() - 1] != '\\')
	{
		strTemp += L"\\";
	}
	strTemp.Append(L"*");

	WIN32_FIND_DATA fd;
	HANDLE hFindFile = FindFirstFile(strTemp, &fd);
	if (hFindFile == INVALID_HANDLE_VALUE)
	{
		::FindClose(hFindFile);
		return FALSE;
	}

	BOOL bIsDirectory = FALSE;
	BOOL bFinish = FALSE;

	CString strSubDirPath;

	while (!bFinish)
	{
		bIsDirectory = ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);

		//如果是.或..  
		if (bIsDirectory && (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0))
		{
			bFinish = (FindNextFile(hFindFile, &fd) == FALSE);
			continue;
		}

		if (bIsDirectory && bRecursive) //是子目录  
		{
			EnumSubDirectories(strTemp.Left(strTemp.GetLength() - 1) + fd.cFileName, subDirectories, bRecursive);
		}
		else if (bIsDirectory)
		{
			subDirectories.push_back(strTemp.Left(strTemp.GetLength() - 1) + fd.cFileName);
		}

		bFinish = (FindNextFile(hFindFile, &fd) == FALSE);
	}

	::FindClose(hFindFile);

	return TRUE;
}

BOOL CFileHelper::EnumSubFiles(const CString & strDirPath, std::vector<CString>& subFiles, bool bRecursive)
{
	if (!IsDictoryExists(strDirPath))
	{
		return FALSE;
	}

	CString strTemp = strDirPath;
	if (strTemp[strTemp.GetLength() - 1] != '\\')
	{
		strTemp += L"\\";
	}
	strTemp.Append(L"*");

	WIN32_FIND_DATA fd;
	HANDLE hFindFile = FindFirstFile(strTemp, &fd);
	if (hFindFile == INVALID_HANDLE_VALUE)
	{
		::FindClose(hFindFile);
		return FALSE;
	}

	BOOL bIsDirectory = FALSE;
	BOOL bFinish = FALSE;

	CString strSubDirPath;

	while (!bFinish)
	{
		bIsDirectory = ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);

		//如果是.或..  
		if (bIsDirectory && (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0))
		{
			bFinish = (FindNextFile(hFindFile, &fd) == FALSE);
			continue;
		}

		if (bIsDirectory && bRecursive)
		{
			EnumSubFiles(strTemp.Left(strTemp.GetLength() - 1) + fd.cFileName, subFiles, bRecursive);
		}
		else if (!bIsDirectory)
		{
			subFiles.push_back(strTemp.Left(strTemp.GetLength() - 1) + fd.cFileName);
		}

		bFinish = (FindNextFile(hFindFile, &fd) == FALSE);
	}

	::FindClose(hFindFile);

	return TRUE;
}
string WStringToUTF8String(LPCWSTR str)
{
	return CT2A(str, CP_UTF8);
}

CString Utf8StringToWString(LPCSTR str)
{
	return CA2T(str, CP_UTF8);
}


//utf8和unicode格式尚未单元测试 FMJ
BOOL CFileHelper::WriteAllText( CString filePath, CString& str, FileEncoding encoding /*= FILE_ENCODING_ANSI */ )
{
	CAtlFile file;
	HRESULT ret = file.Create( filePath, FILE_WRITE_DATA, FILE_SHARE_WRITE,CREATE_ALWAYS );
	if ( !SUCCEEDED( ret ) )
		return FALSE;	
	switch ( encoding )
	{
	case FILE_ENCODING_UNICODE:
		{
			//初始化UNICODE bom
			unsigned char bom[2] = {  0xff, 0xfe };
			file.Write( &bom, 2 );
			file.Write( str.GetBuffer(), str.GetLength()*2 );
		}
		break;
	case FILE_ENCODING_UTF8:
		{
			//std::string utf = CommonHelper::ConvertLocalToUtf8( str );
			std::string utf = WStringToUTF8String(str);
			//初始化utf8bom
			unsigned char bom[3] = { 0xef, 0xbb, 0xbf };
			file.Write( &bom, 3 );
			file.Write( utf.c_str(), strlen( utf.c_str() ));
		}
		break;
	default:	//默认为ANSI格式
		{
			int length = str.GetLength()*2 + 1;
			char* pBuffer = new char[length]();
			ZeroMemory( pBuffer,length );
			memcpy( pBuffer, CT2A( str ).m_psz, length  );
			ret = file.Write( pBuffer, strlen(pBuffer) );
			delete pBuffer;
		}
	}

	if ( !SUCCEEDED( ret ) )
		return FALSE;

	return ret;
}


CString CFileHelper::ReadAllText( CString filePath, FileEncoding encoding /*= FILE_ENCODING_ANSI */ )
{
	char* pBuffer = NULL;
	ULONGLONG buffSize;
	if( !ReadAllBinary( filePath, pBuffer, buffSize ) )
	{
		return _T("");
	}
	return ReadAllText( pBuffer, buffSize, encoding );
}

BOOL IsTextUTF8(LPSTR lpBuffer, int iBufSize)
{
	/*
	0zzzzzzz;
	110yyyyy, 10zzzzzz
	1110xxxx, 10yyyyyy, 10zzzzzz
	11110www, 10xxxxxx, 10yyyyyy, 10zzzzzz
	*/

	int iLeftBytes = 0;
	BOOL bUtf8 = FALSE;
	if (iBufSize <= 0)
		return FALSE;

	for (int i = 0; i < iBufSize; i++)
	{
		char c = lpBuffer[i];
		if (c < 0)       //至少有一个字节最高位被置位
			bUtf8 = TRUE;
		if (iLeftBytes == 0)//之前尚无UTF-8编码的字符的前导字节，或者是下个字符。
		{
			if (c >= 0) //0000 0000 - 0100 0000
				continue;
			do//统计出高位连续的的个数
			{
				c <<= 1;
				iLeftBytes++;
			} while (c < 0);
			iLeftBytes--;    //表示本字符的剩余字节的个数；
			if (iLeftBytes == 0)//最高位是10，不能作为UTF-8编码的字符的前导字节。
				return FALSE;
		}
		else
		{
			c &= 0xC0;       //1100 0000
			if (c != (char)0x80)//1000 0000 对于合法的UTF-8编码，非前导字节的前两位必须为。
				return 0;
			else
				iLeftBytes--;
		}
	}
	if (iLeftBytes)
		return FALSE;
	return bUtf8;
}
//utf8和unicode格式尚未单元测试 FMJ
CString CFileHelper::ReadAllText( char* &pBuffer,ULONGLONG& buffSize, FileEncoding encoding )
{
	CString result;
	switch ( encoding )
	{
	case FILE_ENCODING_UNICODE:
		{
			ATLASSERT((BYTE) pBuffer[0] == 0xff && (BYTE)pBuffer[1] == 0xfe );
			wchar_t* pwstr = reinterpret_cast<wchar_t*>( pBuffer );
			//此处应该需要做LITTLE_ENDIAN的判断 否则可能忽悠bug
			if ( *pwstr == 0xfeff )
				return ( pwstr + 1 );
			else
				return pwstr;
		}
		break;
	case FILE_ENCODING_UTF8:
		{
			const unsigned char* pUBuffer = (const unsigned char*)pBuffer;
			if(  pUBuffer[0] == 0xef && pUBuffer[1] == 0xbb && pUBuffer[2] == 0xbf )
			{
				pBuffer += 3;
			}
			//return CommonHelper::ConvertUtf8ToLocal( pBuffer );
			return Utf8StringToWString(pBuffer);
		}
		break;
	default:	//默认为ANSI格式
		{
			if( IsTextUTF8( pBuffer , (int)buffSize ) )
			{
				return ReadAllText( pBuffer, FILE_ENCODING_UTF8);
			}
			result = CA2T(pBuffer).m_psz;
		}
	}
	delete pBuffer;
	return result;
}

BOOL CFileHelper::ReadAllBinary( CString filePath, OUT char*& buffer, OUT ULONGLONG& buffSize )
{
	if( !IsFileExists( filePath ) )
	{
		return NULL;
	}

	CAtlFile file;
	HRESULT ret = file.Create( filePath, FILE_READ_DATA, FILE_SHARE_READ,OPEN_EXISTING );
	if( ret != S_OK )
	{
		return NULL;
	}

	if ( S_OK != file.GetSize( buffSize ))
	{
		file.Close();
		return NULL;
	}
	if( buffer != NULL)
	{
		delete buffer;
	}
	//bufferSize 以0结尾
	buffer = new char[(int)buffSize + 1]();
	ZeroMemory( buffer,(int)buffSize + 1 );

	DWORD readSize = 0;
	file.Read( buffer, (DWORD)buffSize, readSize );
	ATLASSERT( buffSize == readSize );

	return TRUE;
}

string CFileHelper::ReadAllBinary(CString filePath, bool addone)
{
	string result;
	
	if (!IsFileExists(filePath))
	{
		return result;
	}

	CAtlFile file;
	HRESULT ret = file.Create(filePath, FILE_READ_DATA, FILE_SHARE_READ, OPEN_EXISTING);
	if (ret != S_OK)
	{
		return "";
	}
	ULONGLONG fileSize;
	if (S_OK != file.GetSize(fileSize))
	{
		file.Close();
		return "";
	}
	result.resize(addone ?fileSize + 1: fileSize);

	DWORD readSize = 0;
	file.Read((LPVOID)result.c_str(), (DWORD)fileSize, readSize);
	
	return result;
}

BOOL CFileHelper::WriteAllBinary(CString filePath, LPCVOID buf, UINT64 len, CString* res)
{
	CAtlFile file;
	HRESULT ret = file.Create( filePath, FILE_WRITE_DATA, FILE_SHARE_WRITE, CREATE_ALWAYS );
	if ( !SUCCEEDED( ret ) )
	{
		CString resTemp;
		resTemp.Format(_T("file.Create error:%ld, file path:%s"), ret, filePath);
		if(res != NULL) *res = resTemp;

		return FALSE;	
	}
	try
	{
		ret = file.Write(buf, (DWORD)len);

		CString resTemp;
		resTemp.Format(_T("file.Write error:%ld, file path:%s"), ret, filePath);
		if(res != NULL) *res = resTemp;
	}
	catch (...)
	{
		CString resTemp;
		resTemp.Format(_T("file.catch error:%ld, file path:%s"), ret, filePath);
		if(res != NULL) *res = resTemp;

		return FALSE;
	}

	if ( !SUCCEEDED( ret ) )
		return FALSE;

	return TRUE;
}

CString CFileHelper::GetAppPath()
{
	CString strPath;
	TCHAR *szBuffer = strPath.GetBuffer( MAX_PATH + 1 );
	::GetModuleFileName( NULL, szBuffer, MAX_PATH );
	strPath.ReleaseBuffer();
	int nFind = strPath.ReverseFind( '\\' );
	return strPath.Left( nFind + 1 );
}

CString CFileHelper::GetAppExeName()
{
	CString strPath;
	TCHAR* szBuffer = strPath.GetBuffer(MAX_PATH + 1);
	::GetModuleFileName(NULL, szBuffer, MAX_PATH);
	strPath.ReleaseBuffer();
	int nFind = strPath.ReverseFind('\\');
	return strPath.Mid(nFind + 1);
}

//pcutil::CVersion CFileHelper::GetProductVersion()
//{
//	CString strPath;
//	TCHAR *szBuffer = strPath.GetBuffer( MAX_PATH + 1 );
//	::GetModuleFileName( NULL, szBuffer, MAX_PATH );
//	strPath.ReleaseBuffer();
//	return CFileHelper::GetProductVersion(strPath);
//}

CString CFileHelper::GetProductVersion(LPCWSTR FileName)
{
	typedef struct _tagLanguage
	{
		WORD wLanguage;
		WORD wCodePage;
	} tagLanguage, * LPLanguage;

	struct VS_VERSIONINFO
	{
		WORD                wLength;
		WORD                wValueLength;
		WORD                wType;
		WCHAR               szKey[1];
		WORD                wPadding1[1];
		VS_FIXEDFILEINFO    Value;
		WORD                wPadding2[1];
		WORD                wChildren[1];
	};

	//CVersion 类，将字符串“1234,456,789,1110”的版本信息转换成数字，可以自行编写代码  
	//CVersion version(Version);
	//CString FileName;
	//TCHAR* szBuffer = FileName.GetBuffer(MAX_PATH + 1);
	//::GetModuleFileName(NULL, szBuffer, MAX_PATH);
	//FileName.ReleaseBuffer();

	DWORD dwVerHnd = 0;
	//先获取整个文件版本信息的大小  
	DWORD dwVerInfoSize = GetFileVersionInfoSize(FileName, &dwVerHnd);
	//根据大小来申请内存  
	TCHAR* VerInfo = new TCHAR[dwVerInfoSize];
	//获取文件版本信息，这些信息存在刚刚申请的内存中，修改版本信息通过直接修改内存后一次性将版本信息更新  
	BOOL res = GetFileVersionInfo(FileName, 0, dwVerInfoSize, VerInfo);
	if (!res)
	{
		delete[]VerInfo;
		return {};
	}

	//首先在获取的版本信息中读取语言信息，因为修改版本信息需要用到  
	LPLanguage language = NULL;
	UINT size = 0;
	VerQueryValue(VerInfo, _T("\\VarFileInfo\\Translation"), (LPVOID*)&language, &size);

	//读取文件版本信息  
	VS_FIXEDFILEINFO* FixedFileInfo = NULL;
	VerQueryValue(VerInfo, _T("\\"), (LPVOID*)&FixedFileInfo, &size);
	TCHAR TempBuf[MAX_PATH] = { 0 };
	

	//读取StringFileInfo中的信息信息  
	TCHAR* ProductVer = NULL;
	_stprintf_s(TempBuf, _T("\\StringFileInfo\\%04x%04x\\ProductVersion"), language->wLanguage, language->wCodePage);
	VerQueryValue(VerInfo, TempBuf, (LPVOID*)&ProductVer, &size);

	if (_tcslen(ProductVer) >MAX_PATH)
	{
		return {};
	}
	return CString(ProductVer);
}


//CVersion CFileHelper::GetFileVersion(CString filePath)
//{
//	DWORD fileInfoSize = ::GetFileVersionInfoSize(filePath, NULL);
//
//	char* fileInfo = new char[fileInfoSize];
//	if (::GetFileVersionInfo(filePath, NULL, fileInfoSize, fileInfo) != 0)
//	{
//		VS_FIXEDFILEINFO *versionInfo;
//		unsigned int len;
//		if (::VerQueryValue(fileInfo, _T("\\"), (LPVOID*)&versionInfo, &len) != 0)
//		{
//			WORD major = HIWORD(versionInfo->dwFileVersionMS);
//			WORD minor = LOWORD(versionInfo->dwFileVersionMS);
//			WORD build = HIWORD(versionInfo->dwFileVersionLS);
//
//			WORD branch = LOWORD(versionInfo->dwFileVersionLS);
//
//			return CVersion(major, minor, build, branch);
//		}
//	}
//	return CVersion(0, 0, 0);
//}

BOOL CFileHelper::DirectoryClear(CString& SPath)
{
	/*std::string path;
	path = CT2A(SPath.GetBuffer()).m_psz;*/

	//string path = CommonHelper::ToString(SPath);
	if (IsDictoryExists(SPath) )
	{
		if (SPath[0] != '\\')
		{
			DeleteFiles(SPath, TRUE, TRUE);
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		CreateDir(SPath);
	}
	return TRUE;
}

BOOL CFileHelper::CopyDirectory(CString& sourceFilePath, CString& destFilePath)
{
	TCHAR src[MAX_PATH] = { 0 };
	TCHAR dst[MAX_PATH] = { 0 };
	_tcscpy_s(src, sourceFilePath);
	_tcscpy_s(dst, destFilePath);
	int len = _tcslen(src);
	if (len > 0 && src[len - 1] == '\\')
		src[len - 1] = '\0';
	 len = _tcslen(dst);
	if (len > 0 && dst[len - 1] == '\\')
		dst[len - 1] = '\0';

	SHFILEOPSTRUCT FileOp;
	ZeroMemory(&FileOp, sizeof(SHFILEOPSTRUCT));
	FileOp.fFlags = FOF_SILENT|FOF_NOCONFIRMATION;//不显示对话框.
	FileOp.hNameMappings = NULL;
	FileOp.hwnd = NULL;
	FileOp.lpszProgressTitle = NULL;
	FileOp.pFrom = src;
	FileOp.pTo = dst;
	FileOp.wFunc = FO_COPY;
	return SHFileOperation(&FileOp) == 0;
}
CString CFileHelper::GetLocalStoragePath()
{
	return GetPathFromEnviroment(_T("%LOCALAPPDATA%\\")).c_str();
}


//------------------------------------------------------------------------------------------------------
//												enviroment and file description
//------------------------------------------------------------------------------------------------------
#define ID_WINENV_SYSTEMROOT_DIRECTORIES		_T("%systemroot directory%")
#define ID_WINENV_WINDOWS_DIRECTORIES			_T("%windows directory%")


inline TSTRING CFileHelper::toupper(const TSTRING& s)
{
	TSTRING upper(s);
	for (size_t i = 0; i < s.length(); i++)
		upper[i] = TOUPPER(upper[i]);
	return upper;
}

inline TSTRING CFileHelper::maketolower(const TSTRING& s)
{
	TSTRING lower(s);
	for (size_t i = 0; i < s.length(); i++)
		lower[i] = TOLOWER(lower[i]);
	return lower;
}

//TSTRING itos(const int i);

inline TSTRING CFileHelper::rtrim(TSTRING& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(),
		std::not1(std::ptr_fun(::isspace))).base(),
		s.end());
	return s;
}

inline TSTRING CFileHelper::ltrim(TSTRING& s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		std::not1(std::ptr_fun(::isspace))));
	return s;
}
inline TSTRING CFileHelper::trim(TSTRING& s)
{
	return ltrim(rtrim(s));
}


inline TSTRING CFileHelper::GetPathFromEnviroment(const TSTRING& sEnvName)
{
	TSTRING sEnvNameLower = maketolower(sEnvName);
	TSTRING sPath;

	if (sEnvNameLower.compare(ID_WINENV_SYSTEMROOT_DIRECTORIES) == 0)
	{
		TCHAR tchSystemDirectory[MAX_PATH + 1] = { 0 };

		GetSystemDirectory(tchSystemDirectory, MAX_PATH);
		sPath = tchSystemDirectory;
	}
	else if (sEnvNameLower.compare(ID_WINENV_WINDOWS_DIRECTORIES) == 0)
	{
		TCHAR tchWindowsDirectory[MAX_PATH + 1] = { 0 };

		GetWindowsDirectory(tchWindowsDirectory, MAX_PATH);
		sPath = tchWindowsDirectory;
	}
	else
	{
		TCHAR tchPath[MAX_PATH + 1] = { 0 };

		//349564, added by jacky, 2006/11/22
		//ExpandEnvironmentStrings gets the windows env, GetEnvironmentVariable gets the user define env
		ExpandEnvironmentStrings(sEnvNameLower.c_str(), tchPath, MAX_PATH);

		if (0 == sEnvNameLower.compare(tchPath))
		{
			memset(tchPath, 0, sizeof(tchPath));
			GetEnvironmentVariable(sEnvNameLower.c_str(), tchPath, MAX_PATH);
		}

		sPath = tchPath;
	}

	//2016年2月19日这个困惑实在太大
	////remove the last "\" of path
	//if (!sPath.empty() && sPath[sPath.length() - 1] == _T('\\'))
	//	sPath = sPath.substr(0, sPath.length() - 1);

	return sPath;
}

CString CFileHelper::GetProgramDataPath()
{
	ATL::CString initPath;
	::SHGetSpecialFolderPath(NULL, initPath.GetBuffer(MAX_PATH), CSIDL_COMMON_APPDATA, FALSE);
	initPath.ReleaseBuffer();
	initPath.Append(_T("\\Lenovo\\SmartEngine\\"));
	return initPath;
}


CString CFileHelper::GetTempPath()
{
	wchar_t path[MAX_PATH];
	::GetTempPath(MAX_PATH, path);

	if (!PathIsDirectoryW(path))
	{
		::SHCreateDirectoryEx(NULL, path, 0);
	}
	return path;
}

CString CFileHelper::GetWindowsDir()
{
	CString wstr;
	UINT size = GetWindowsDirectory(NULL, 0);
	wchar_t *path = new wchar_t[size];
	if (GetWindowsDirectory(path, size) != 0) //函数调用失败将返回0
	{
		wstr = path;
	}
	delete [] path;
	return wstr;
}


CString CFileHelper::GetFileVerInfo(LPCTSTR  strFilePath, LPCTSTR  pszType)
{
	DWORD dwSize;
	DWORD dwRtn;
	CString szVersion;

	if (strFilePath == NULL)
		return "";

	//获取版本信息大小
	dwSize = GetFileVersionInfoSize(strFilePath, NULL);
	if (dwSize == 0)
	{
		return "";
	}
	CHAR *pBuf;
	pBuf = new CHAR[dwSize + 1];
	if (pBuf == NULL)
		return "";
	memset(pBuf, 0, dwSize + 1);
	//获取版本信息
	dwRtn = GetFileVersionInfo(strFilePath, NULL, dwSize, pBuf);
	if (dwRtn == 0)
	{
		return "";
	}


	//版本资源中获取信息

	char* pVerValue = NULL;
	UINT nSize = 0;
	dwRtn = VerQueryValue(pBuf, TEXT("\\VarFileInfo\\Translation"), (LPVOID*)&pVerValue, &nSize);
	if (dwRtn == 0)
	{
		return "";
	}
	CString strSubBlock, strTranslation, strTemp;
	strTemp.Format(L"000%x", *((unsigned short int *)pVerValue));
	strTranslation = strTemp.Right(4);
	strTemp.Format(L"000%x", *((unsigned short int *)&pVerValue[2]));
	strTranslation += strTemp.Right(4);
	//080404b0为中文，040904E4为英文

	//文件描述
	strSubBlock.Format(L"\\StringFileInfo\\%ws\\%ws", strTranslation, pszType);
	dwRtn = VerQueryValue(pBuf, strSubBlock.GetBufferSetLength(256), (LPVOID*)&pVerValue, &nSize);
	strSubBlock.ReleaseBuffer();

	//可以测试的属性
	// CompanyName  FileDescription	 FileVersion	 InternalName LegalCopyright OriginalFilename	 ProductName ProductVersion Comments LegalTrademarks PrivateBuild	 SpecialBuild

	DWORD dwErr = GetLastError();
	if (dwRtn == 0)
	{
		return "";
	}
	//CString cs = (TCHAR*)pVerValue;
	szVersion = (TCHAR*)pVerValue;
	//cs = CA2T(szVersion.data(), CP_UTF8);
	delete pBuf;
	return szVersion;
}


BOOL FindEnvironmentPos(LPCTSTR pszEnvPath, int& nFirstPos, int& nSecondPos)
{
	BOOL bRet = TRUE;
	if (pszEnvPath == NULL)
	{
		bRet = FALSE;
		goto Exit0;
	}

	const TCHAR cSpil = TEXT('%');

	const TCHAR* pFirstPos = NULL;
	const TCHAR* pSecondPos = NULL;

	pFirstPos = _tcschr(pszEnvPath, cSpil);
	if (pFirstPos == NULL)
	{
		bRet = FALSE;
		goto Exit0;
	}

	pSecondPos = _tcschr(pFirstPos + 1, cSpil);
	if (pSecondPos == NULL)
	{
		bRet = FALSE;
		goto Exit0;
	}

	nFirstPos = int(pFirstPos - pszEnvPath);
	nSecondPos = int(pSecondPos - pszEnvPath);
Exit0:
	return bRet;
}

CString CombinationPath(const CString& strLeft,
	const CString& strMiddle,
	const CString& strRight)
{
	CString strResult;
	const int nCount = 3;
	const CString* pStr[nCount] =
	{ &strLeft, &strMiddle, &strRight };

	for (int i = 0; i < nCount; i++)
	{
		const CString& str = *pStr[i];

		if (strResult.GetLength() <= 0)
			strResult += str;
		else
		{
			if (str.GetLength() <= 0)
				continue;

			if (strResult[strResult.GetLength() - 1] == TEXT('\\'))
				strResult.Delete(strResult.GetLength() - 1);

			if (str[0] != TEXT('\\'))
			{
				strResult += TEXT('\\');
			}

			strResult += str;
		}
	}

	return strResult;
}

CString CFileHelper::GetEnvPath(LPCTSTR pszEnvName)
{
	CString strResult;
	TCHAR szBuffer[MAX_PATH] = { 0 };

	int nFolder = -1;

	int nCount = sizeof(g_csidlnamelist) / sizeof(g_csidlnamelist[0]);

	for (int i = 0; i < nCount; i++)
	{
		if (_tcscmp(pszEnvName, g_csidlnamelist[i].pszCsidlName) == 0)
		{
			nFolder = g_csidlnamelist[i].nFolder;
			break;
		}
	}

	if (nFolder != -1)
	{
		//nFolder = CSIDL_PROGRAM_FILESX86;
		if (::SHGetSpecialFolderPath(NULL, szBuffer, nFolder, FALSE))
			strResult = szBuffer;
	}

	return strResult;
}

CString CFileHelper::ParseSpecialPath(LPCTSTR pszEnvPath)
{
	CString strResult = _T("");

	if (pszEnvPath == NULL)
	{
		return strResult;		
	}

	int nFirstPos = 0;
	int nSecondPos = 0;
	BOOL bFind = FALSE;

	strResult = pszEnvPath;

	bFind = FindEnvironmentPos(strResult, nFirstPos, nSecondPos);

	if (bFind == TRUE)
	{
		CString strLeft;
		CString strRight;
		CString strEnvName;
		CString strEnvPath;

		strLeft = strResult.Left(nFirstPos);
		strRight = strResult.Mid(nSecondPos + 1);
		strEnvName = _T("CSIDL_");
		strEnvName += strResult.Mid(nFirstPos + 1, nSecondPos - nFirstPos - 1);

		strEnvPath = GetEnvPath(strEnvName);

		strResult = CombinationPath(strLeft, strEnvPath, strRight);
	}

	return strResult;
}