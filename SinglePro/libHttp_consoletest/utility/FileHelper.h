#pragma once

#include <atlstr.h>
#include <vector>
#include "ClientConfig.h"
#include "Version.h"
#include "DataDefine.h"
#include <string>
namespace pcutil
{
enum FileEncoding
{
	FILE_ENCODING_UNKNOWN,
	FILE_ENCODING_UNICODE,
	FILE_ENCODING_UTF8,
	FILE_ENCODING_ANSI
};

class WEAVERLIB_API_UTILS CFileHelper
{
public:
	static BOOL CreateDir( CString path );
	static BOOL CreateDir( char* path );

	static BOOL EnumSubDirectories(const CString& strDirPath, std::vector<CString>& subDirectories, bool bRecursive = false);
	static BOOL EnumSubFiles(const CString& strDirPath, std::vector<CString>& subFiles, bool bRecursive = false);

	static BOOL Delete(CString filePath);
	static BOOL DeleteDir( CString filePath );
	//递归删除目录下所有的文件 包含子目录下的文件, deleteSubFolderFiles表示是否递归删除子目录的文件，deleteSubFolder表示是否删除子目录
	static void DeleteFiles( CString folderPath,  BOOL deleteSubFolderFiles = FALSE, BOOL deleteSubFolder = FALSE );
	static void DeleteFilesImpl(const wchar_t *folderPath, BOOL deleteSubFolderFiles = FALSE, BOOL deleteSubFolder = FALSE );
	//static bool SHDeleteFiles(CString strFrom);
	//尝试删除文件/文件夹，如果删除不了，则会标记，下次启动电脑后会自动删除;
	static void DeleteFileEx(CString _strPathfile);

	//关于此函数的限制条件，参考MSDN MoveFile的描述
	static BOOL RenameFile(const CString& strOldName, const CString& strNewName);

	static BOOL IsFileExists( CString filePath );
	static CString GetFileName( CString filePath );
	static CString GetFileExtention( CString filePath );
	static CString GetFileDir(CString filePath);
	static CString GetParentDir(const CString& path);
	static CString ConcatenatePath(const CString& leftPath, const CString& rightPath);
	static void Move( CString sourceFilePath, CString destFilePath );
	static void Copy( CString sourceFilePath, CString destFilePath  );
	static UINT64 GetFileSize( CString filePath );

	static BOOL IsDictoryExists( CString path );

	static CString GetApplicationDataPath();
	static CString GetMyPicturePath();
	static CString GetLocalStoragePath();
	static CString GetProgramDataPath();


	static CString GetTempPath();
	//暂时只支持ANSI格式
	static BOOL WriteAllText( CString filePath, CString& str, FileEncoding encoding = FILE_ENCODING_ANSI );
	//暂时只支持ANSI格式
	static CString ReadAllText( CString filePath, FileEncoding encoding = FILE_ENCODING_ANSI );

	static CString ReadAllText( char* &pBuffer, UINT64& buffSize, FileEncoding encoding = FILE_ENCODING_ANSI );

	static BOOL ReadAllBinary( CString filePath, OUT char*& buffer, OUT UINT64& buffSize );

	//addone 之前的调用都是+1的，不确认什么原因，兼容处理
	static std::string ReadAllBinary(CString filePath, bool addone = true );

	static BOOL WriteAllBinary( CString filePath, LPCVOID buf, UINT64 len, CString* res = NULL);
	//获取应用程序目录
	static CString GetAppPath();
	static CString GetAppExeName();
	static CVersion GetProductVersion();
	static CString GetProductVersion(LPCWSTR FileName);
	static CVersion GetFileVersion(CString filePath);
	static BOOL DirectoryClear( CString& Path);
	static BOOL CopyDirectory(CString& sourceFilePath, CString& destFilePath);
	static CString GetWindowsDir();
	static CString GetFileVerInfo(LPCTSTR  strFilePath, LPCTSTR  pszType);

	//获取环境变量路径
	static CString GetEnvPath(LPCTSTR pszEnvName);
	static CString ParseSpecialPath(LPCTSTR pszEnvPath);


private:
	static TSTRING GetPathFromEnviroment(const TSTRING& sEnvName);

	inline static TSTRING toupper(const TSTRING& s);
	 

	inline static TSTRING maketolower(const TSTRING& s);
	 

	//TSTRING itos(const int i);

	inline static TSTRING rtrim(TSTRING& s);
	 

	inline static TSTRING ltrim(TSTRING& s);
	 
	inline static TSTRING trim(TSTRING& s);
	

		
};
	
}