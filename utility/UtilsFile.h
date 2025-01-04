#pragma once

#include <string>
#include <set>
#include <vector>

class UtilsFile
{

public:
	//按行读取文件
	static void GetStringByLine(const std::wstring& strFileName, std::vector<std::string>& vecData);

	static std::wstring GetPath();
	static std::wstring GetDataPath();
	static std::wstring GetCfgPath();
	static std::wstring GetCfgFile();

	/** %appdata%+GameHL
	*/
	static std::wstring GetUserDataPath();

	/*\\Lenovo\\SmartEngine\\GameHL\\LS_SDK\\
	*/
	//static std::wstring GetProgDataPath();

	/** 获取 C:\Users\用户名\AppData\Local 所在路径
	*/
	static std::wstring GetAppDataPath();

	/** 获取文件夹下所有文件
	*/
	static bool EnumAllFiles(const std::wstring& strPath, std::vector<std::wstring>& vecFileName);
	static bool EnumAllFiles(const std::string& strPath, std::vector<std::string>& vecFileName);

	static bool IsFileExists(const std::wstring& strFile);

	static std::string w2a(const wchar_t* s);
	static std::string w2a(std::wstring s);
};