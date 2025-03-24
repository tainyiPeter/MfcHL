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
	//�ݹ�ɾ��Ŀ¼�����е��ļ� ������Ŀ¼�µ��ļ�, deleteSubFolderFiles��ʾ�Ƿ�ݹ�ɾ����Ŀ¼���ļ���deleteSubFolder��ʾ�Ƿ�ɾ����Ŀ¼
	static void DeleteFiles( CString folderPath,  BOOL deleteSubFolderFiles = FALSE, BOOL deleteSubFolder = FALSE );
	static void DeleteFilesImpl(const wchar_t *folderPath, BOOL deleteSubFolderFiles = FALSE, BOOL deleteSubFolder = FALSE );
	//static bool SHDeleteFiles(CString strFrom);
	//����ɾ���ļ�/�ļ��У����ɾ�����ˣ�����ǣ��´��������Ժ���Զ�ɾ��;
	static void DeleteFileEx(CString _strPathfile);

	//���ڴ˺����������������ο�MSDN MoveFile������
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
	//��ʱֻ֧��ANSI��ʽ
	static BOOL WriteAllText( CString filePath, CString& str, FileEncoding encoding = FILE_ENCODING_ANSI );
	//��ʱֻ֧��ANSI��ʽ
	static CString ReadAllText( CString filePath, FileEncoding encoding = FILE_ENCODING_ANSI );

	static CString ReadAllText( char* &pBuffer, UINT64& buffSize, FileEncoding encoding = FILE_ENCODING_ANSI );

	static BOOL ReadAllBinary( CString filePath, OUT char*& buffer, OUT UINT64& buffSize );

	//addone ֮ǰ�ĵ��ö���+1�ģ���ȷ��ʲôԭ�򣬼��ݴ���
	static std::string ReadAllBinary(CString filePath, bool addone = true );

	static BOOL WriteAllBinary( CString filePath, LPCVOID buf, UINT64 len, CString* res = NULL);
	//��ȡӦ�ó���Ŀ¼
	static CString GetAppPath();
	static CString GetAppExeName();
	static CVersion GetProductVersion();
	static CString GetProductVersion(LPCWSTR FileName);
	static CVersion GetFileVersion(CString filePath);
	static BOOL DirectoryClear( CString& Path);
	static BOOL CopyDirectory(CString& sourceFilePath, CString& destFilePath);
	static CString GetWindowsDir();
	static CString GetFileVerInfo(LPCTSTR  strFilePath, LPCTSTR  pszType);

	//��ȡ��������·��
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