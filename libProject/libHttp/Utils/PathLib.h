#pragma once

#include "../PrepareDef.h"

//
//
//typedef enum PathFlag
//{
//	SYSCONFIG_PATH,				// ϵͳ����Ŀ¼
//	FACE_PATH,					// Ƥ��Ŀ¼
//	SETTING_PATH,
//	BLOCK_PATH,
//	DATA_PATH,
//	ZIXUN_PATH,
//	ATTACHMODULES_PATH,
//	USER_PATH,
//	LOG_PATH,
//	SCITER_PATH
//} PathFlag;
//
//class EXTERN_CLASS PathLib
//{
//public:
//	PathLib(void);
//	~PathLib(void);
//
//	// ��ȡ��App����
//	static const CString GetAppFullName();
//	// ����������Ŀ¼
//	static const CString GetAppPath();
//	// ����������
//	static const CString GetAppName();
//	// ָ��Ŀ¼·��
//	static const CString GetSpecifyPath(PathFlag flag);	
//
//	// ��ȡ����Ŀ¼
//	static const CString GetConfigPath();
//	static const CString GetFacePath();
//	static const CString GetSettingPath();
//	static const CString GetBlockPath();
//	static const CString GetDataPath();
//	static const CString GetZixunPath();
//	static const CString GetAttachModulesPath();
//	static const CString GetUserPath();
//	static const CString GetLogPath();
//	static const CString GetSciterPath();
//
//	//��ȡ��ǰ����Ŀ¼
//	static const CString GetCurrentDirectory();
//	// ��ȡָ��Ŀ¼�ĸ�Ŀ¼
//	static const CString GetParentDirectory(LPCTSTR pszPath);
//	// ����Ŀ¼���༶Ŀ¼��һ�𴴽�
//	static BOOL CreateDirectory(LPCTSTR pszPath);
//	// ȷ��Ŀ¼���ڣ����������½�
//	static BOOL EnsureDirExsit(LPCTSTR pszPath);
//	// ����ļ���Ŀ¼�Ƿ����
//	static BOOL PathFileExists(LPCTSTR pszPath);
//	// �ж�·���Ƿ����·��
//	static BOOL IsRelativePath(LPCTSTR pszPath);
//	// �ж�·���Ƿ�Ϊ����·��
//	static BOOL IsNetPath(LPCTSTR pszPath);
//	// ɾ���ļ�
//	static BOOL DeleteFile(LPCTSTR lpFileName);
//	// ������
//	static BOOL RenameFile(LPCTSTR lpFileName, LPCTSTR lpNewName);
//	// Ŀ¼ɾ��
//	static BOOL RemoveDirectory(LPCTSTR pszPath);
//	// ���Ŀ¼
//	static BOOL EmptyDirectoryFiles(LPCTSTR pszPath);
//};
//
//// �ļ����ƹ�����
//class EXTERN_CLASS FileHolder
//{
//public:
//	FileHolder(LPCTSTR lpPathName);
//	~FileHolder(void);
//
//	void SetPathName(LPCTSTR lpPathName);
//
//	const CString GetFilePath();
//	const CString GetFileName();
//	const CString GetBaseName();
//	const CString GetExtName();
//	const CString PathNameWithNewExt(LPCTSTR lpExtName);
//private:
//	CString m_strPathName;
//};
//
//// Ŀ¼�л������࣬�Զ��ظ��������Ŀ¼
//class EXTERN_CLASS PathHolder
//{
//public:
//	PathHolder(void);
//	~PathHolder(void);
//
//	void Moor();
//	void Unmoor();
//
//private:
//	CString m_PathAnchor;
//};