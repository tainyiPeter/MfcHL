//#include "StdAfx.h"
#include "Version.h"
//#include "CommonHelper.h"
#include "Tool.h"
#include <vector>

using namespace std;

namespace pcutil
{

CVersion::CVersion()
{
    SetDefaultValue();
}

CVersion::CVersion( const CString &version )
{
	SetDefaultValue();
    Parse( version );
}

CVersion::CVersion( int major, int minor )
{
    m_nBuild = -1;
    m_nRevision = -1;
    if (major < 0)
    {
		ATLASSERT( false);
	}
    if (minor < 0)
    {
		ATLASSERT( false);
    }
    m_nMajor = major;
    m_nMinor = minor;
}

CVersion::CVersion(int major, int minor, int build)
{
    m_nBuild = -1;
    m_nRevision = -1;
    if (major < 0)
    {
		ATLASSERT( false);
    }
    if (minor < 0)
    {
		ATLASSERT( false);
    }
    if (build < 0)
    {
		ATLASSERT( false);
    }
    m_nMajor = major;
    m_nMinor = minor;
    m_nBuild = build;
}

CVersion::CVersion(int major, int minor, int build, int revision)
{
    m_nBuild = -1;
    m_nRevision = -1;
    if (major < 0)
    {
		ATLASSERT( false);
    }
    if (minor < 0)
    {
		ATLASSERT( false);
    }
    if (build < 0)
    {
		ATLASSERT( false);
    }
    if (revision < 0)
    {
		ATLASSERT( false);
    }
    m_nMajor = major;
    m_nMinor = minor;
    m_nBuild = build;
    m_nRevision = revision;
}

CVersion::~CVersion()
{

}

int CVersion::CompareTo( const CVersion &version ) const
{
    const CVersion &version2 = version;
    if (m_nMajor != version2.m_nMajor)
    {
        if (m_nMajor > version2.m_nMajor)
        {
            return 1;
        }
        return -1;
    }
    if (m_nMinor != version2.m_nMinor)
    {
        if (m_nMinor > version2.m_nMinor)
        {
            return 1;
        }
        return -1;
    }
    if (m_nBuild != version2.m_nBuild)
    {
        if (m_nBuild > version2.m_nBuild)
        {
            return 1;
        }
        return -1;
    }
    if (m_nRevision == version2.m_nRevision)
    {
        return 0;
    }
    if (m_nRevision > version2.m_nRevision)
    {
        return 1;
    }
    return -1;
}

void CVersion::SetDefaultValue()
{
	m_nMajor = -1;
	m_nMinor = -1;
	m_nBuild = -1;
	m_nRevision = -1;

}
void CStringSplit(const CString& s, const CString& delim, std::vector<CString >& ret)
{
	int last = 0;
	int index = s.Find(delim, last);
	while (index != -1)
	{
		ret.push_back(s.Mid(last, index - last));
		last = index + 1;
		while (delim == s.GetAt(last))
			last++;
		index = s.Find(delim, last);
	}
	if (last<s.GetLength())
	{
		ret.push_back(s.Mid(last, s.GetLength() - last));
	}
}
bool CVersion::Parse( const CString &version )
{
    if ( version.IsEmpty() ) return false;
    SetDefaultValue();

    std::vector< CString > strArray;
	CStringSplit(version, _T("."), strArray);
    /*if ( !CommonHelper::SplitCStringToArray( version , _T( "." ) , strArray ) )
    {
        return false;
    }*/

    unsigned int length = strArray.size();
    if ( ( length < 1 ) || ( length > 4 ) )
    {
		return false;
    }
    m_nMajor = _ttol( strArray[0] );
    if ( m_nMajor < 0 )
    {
	    SetDefaultValue();
		return false;
    }
	if (length > 1) {
		m_nMinor = _ttol(strArray[1]);
		if (m_nMinor < 0)
		{
			SetDefaultValue();
			return false;
		}

		if (length > 2)
		{
			m_nBuild = _ttol(strArray[2]);
			if (m_nBuild < 0)
			{
				SetDefaultValue();
				return false;
			}

			if (length > 3)
			{
				m_nRevision = _ttol(strArray[3]);
				if (m_nRevision < 0)
				{
					SetDefaultValue();
					return false;
				}
			}
		}
	}
	return true;

}


int CVersion::GetHashCode()
{
    int num = 0;
    num |= (m_nMajor & 15) << 0x1c;
    num |= (m_nMinor & 0xff) << 20;
    num |= (m_nBuild & 0xff) << 12;
    return (num | (m_nRevision & 0xfff));
}

CString CVersion::ToString() const
{
	CString strRet ;
	if( m_nMajor == -1 )
		return strRet;
	strRet.AppendFormat( _T("%d") , m_nMajor );
	if( m_nMinor == -1 )
		return strRet;
	strRet.AppendFormat( _T(".%d") , m_nMinor );
	if( m_nBuild == -1 )
		return strRet;
    CString strBuild;
    strBuild.Format( _T("%d"), m_nBuild );
    strBuild = strBuild.Right( 4 );
	strRet.AppendFormat( _T(".%s") , strBuild );
	if( m_nRevision == -1 )
		return strRet;
	strRet.AppendFormat( _T(".%.4d") , m_nRevision );

	return strRet;

}

BOOL CVersion::IsEmpty() const
{
	return m_nMajor == -1 && m_nMinor == -1 && m_nBuild == -1 && m_nRevision == -1;
}
//服务器支持的标准格式
CString CVersion::ToShortString() const
{
	CString strRet ;
	if( m_nMajor == -1 )
		return strRet;
	strRet.AppendFormat( _T("%d") , m_nMajor );
	if( m_nMinor == -1 )
		return strRet;
	strRet.AppendFormat( _T(".%d") , m_nMinor );
	if( m_nBuild == -1 )
		return strRet;
	strRet.AppendFormat( _T(".%d") , m_nBuild );

	return strRet;

}



CString CVersion::ToLongString()
{
	CString strRet;
	if (m_nMajor == -1)
		return strRet;
	strRet.AppendFormat(_T("%d"), m_nMajor);
	if (m_nMinor == -1)
		return strRet;
	strRet.AppendFormat(_T(".%d"), m_nMinor);
	if (m_nBuild == -1)
		return strRet;
	strRet.AppendFormat(_T(".%d"), m_nBuild);
	if (m_nRevision == -1)
		return strRet;
	strRet.AppendFormat(_T(".%d"), m_nRevision);

	return strRet;
}

bool CVersion::operator==( const CVersion &version  ) const
{
	return ( CompareTo( version ) == 0 );
}

bool CVersion::operator!=( const CVersion &version  ) const
{
	return ( CompareTo( version ) != 0 );
}

bool CVersion::operator>( const CVersion &version  ) const
{
	return ( CompareTo( version ) > 0 );
}

bool CVersion::operator>=( const CVersion &version  ) const
{
	return !( CompareTo( version ) < 0 );
	
}

bool CVersion::operator<( const CVersion &version  ) const
{
	return ( CompareTo( version ) < 0 );
}

bool CVersion::operator<=( const CVersion &version  ) const
{
	return !( CompareTo( version ) > 0 );
}



int CVersion::GetMajor() const
{
    return m_nMajor;
}

int CVersion::GetMinor() const
{
    return m_nMinor;
}

int CVersion::GetBuild() const
{
    return m_nBuild;
}

int CVersion::GetRevision() const
{
    return m_nRevision;
}





}