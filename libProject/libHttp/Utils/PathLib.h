#pragma once

#include "../PrepareDef.h"

//
//
//typedef enum PathFlag
//{
//	SYSCONFIG_PATH,				// 系统配置目录
//	FACE_PATH,					// 皮肤目录
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
//	// 获取主App名称
//	static const CString GetAppFullName();
//	// 主程序运行目录
//	static const CString GetAppPath();
//	// 主程序名称
//	static const CString GetAppName();
//	// 指定目录路径
//	static const CString GetSpecifyPath(PathFlag flag);	
//
//	// 获取特殊目录
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
//	//获取当前工作目录
//	static const CString GetCurrentDirectory();
//	// 获取指定目录的父目录
//	static const CString GetParentDirectory(LPCTSTR pszPath);
//	// 创建目录，多级目录会一起创建
//	static BOOL CreateDirectory(LPCTSTR pszPath);
//	// 确保目录存在，不存在则新建
//	static BOOL EnsureDirExsit(LPCTSTR pszPath);
//	// 检查文件或目录是否存在
//	static BOOL PathFileExists(LPCTSTR pszPath);
//	// 判断路径是否相对路径
//	static BOOL IsRelativePath(LPCTSTR pszPath);
//	// 判断路径是否为网络路径
//	static BOOL IsNetPath(LPCTSTR pszPath);
//	// 删除文件
//	static BOOL DeleteFile(LPCTSTR lpFileName);
//	// 重命名
//	static BOOL RenameFile(LPCTSTR lpFileName, LPCTSTR lpNewName);
//	// 目录删除
//	static BOOL RemoveDirectory(LPCTSTR pszPath);
//	// 清空目录
//	static BOOL EmptyDirectoryFiles(LPCTSTR pszPath);
//};
//
//// 文件名称工具类
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
//// 目录切换工具类，自动回复最初工作目录
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