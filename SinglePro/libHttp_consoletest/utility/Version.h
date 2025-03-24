#pragma once
#include <atlstr.h>
#include "ClientConfig.h"
namespace pcutil
{

class WEAVERLIB_API_UTILS CVersion
{
public:
	CVersion();
	CVersion( const CString &version );
	CVersion( int major, int minor );
	CVersion( int major, int minor, int build );
	CVersion( int major, int minor, int build, int revision );

	~CVersion();

//Method
public:
	bool Parse( const CString &version );
	void SetDefaultValue();
	int GetHashCode();
	int CompareTo( const CVersion &version ) const;
	CString ToString() const;
	CString ToShortString() const;
	CString ToLongString();
	BOOL IsEmpty() const;
//Operator
public:
	bool operator==( const CVersion &version  ) const;
	bool operator!=( const CVersion &version  ) const;
	bool operator>( const CVersion &version  ) const;
	bool operator>=( const CVersion &version  ) const;
	bool operator<( const CVersion &version  ) const;
	bool operator<=( const CVersion &version  ) const;

public:
	int GetMajor() const;
	int GetMinor() const;
	int GetBuild() const;
	int GetRevision() const;


private:
    int m_nMajor;
    int m_nMinor;
	int m_nBuild;
    int m_nRevision;



};


}