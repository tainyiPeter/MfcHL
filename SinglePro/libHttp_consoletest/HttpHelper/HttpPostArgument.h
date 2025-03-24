#pragma once
#include "ClientConfig.h"
//#include <atlbase.h>
#include <atlstr.h>
#include <string>

namespace pcutil
{

class WEAVERLIB_API_UTILS CHttpPostArgument
{
public:
	CHttpPostArgument();
	~CHttpPostArgument();
	CHttpPostArgument( const std::string &strName , const std::string &strValue , DWORD dwArgumentType );

	CHttpPostArgument( const CHttpPostArgument &other );
	CHttpPostArgument& operator=( const CHttpPostArgument &other );

public:
	bool operator==( const CHttpPostArgument &other ) const;
	bool operator<( const CHttpPostArgument &other ) const ;

public:
    enum TypeArgument  	// POST TYPE
    {
        TypeUnknown = 0,
        TypeNormal = 1,
        //TypeBinary = 2,
        TypeRawText = 2
	};

public:
	std::string GetName() ;
	std::string GetValue() ;
	DWORD GetType();

	void SetName( const std::string &strName );
	void SetValue( const std::string &strValue );
	void SetType( DWORD dwArgumentType );

private:
	std::string m_strName;
	std::string m_strValue;
	DWORD m_dwArgumentType;
};

class WEAVERLIB_API_UTILS CHttpPostBinaryRefArgument
{
public:
	CString ArgName;
	CString FileName;
	CString ContentType;
	LPCSTR PBuffer;
	DWORD Length;
    std::string BinaryCache;
	CHttpPostBinaryRefArgument(
		LPCTSTR argName, const CString fileName, LPCSTR pBuffer, DWORD length,
		LPCTSTR contentType = _T( "application/octet-stream" ) );

};

}