#pragma once

#include <string>
#include <set>
#include <vector>

class UtilsFile
{

public:
	//���ж�ȡ�ļ�
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

	/** ��ȡ C:\Users\�û���\AppData\Local ����·��
	*/
	static std::wstring GetAppDataPath();

	/** ��ȡ�ļ����������ļ�
	*/
	static bool EnumAllFiles(const std::wstring& strPath, std::vector<std::wstring>& vecFileName);
	static bool EnumAllFiles(const std::string& strPath, std::vector<std::string>& vecFileName);

	static bool IsFileExists(const std::wstring& strFile);

	static std::string w2a(const wchar_t* s);
	static std::string w2a(std::wstring s);
};