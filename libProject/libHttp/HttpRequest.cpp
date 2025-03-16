#include "HttpRequest.h"
#define WIN32_LEAN_AND_MEAN 
#include <winsock2.h>        // Include Winsock2 before Windows.h
#include <windows.h>         // Include Windows.h
#include "curl/curl.h"

#include "Utils/mem_macro.h"
#include "Utils/UtilsString.h"

#include <sstream>

//#include <LoggerManager.h>

//#if _MSC_VER>=1900
//#include "stdio.h" 
//_ACRTIMP_ALT FILE* __cdecl __acrt_iob_func(unsigned);
//#ifdef __cplusplus 
//extern "C"
//#endif 
//FILE* __cdecl __iob_func(unsigned i) {
//	return __acrt_iob_func(i);
//}
//#endif /* _MSC_VER>=1900 */

// #define _MEM_TRACE	

const char CONTENT_TYPE_JSON[]			= "application/json";
const char CONTENT_TYPE_FORM_DATA[]		= "multipart/form-data";
const char CONTENT_TYPE_URLENCODED[]	= "application/x-www-form-urlencoded";
const char CONTENT_TYPE_STREAM[]		= "application/octet-stream";

const UINT DEFAULT_DATA_BUFFER_LEN = (1024 * 10);

#define HTTP_SCHEME		"http"
#define HTTPS_SCHEME	"https"

#define HTTP_STATUS_CODE_OK   200

LONG64 NEXT_GLOBE_HTTP_REQUEST_ID = 0;

HttpRequest::HttpRequest()
{
	Init();
}

HttpRequest::HttpRequest(const char* lpUrl, HTTP_METHOD enumMethod)
{
	Init();

	SetUrl(lpUrl);
	SetMethod(enumMethod);
}

HttpRequest::HttpRequest(const char* lpUrl, const char* pszPostData /*= NULL*/, const char* pzsContentType /*= CONTENT_TYPE_DEFAULT*/)
{
	Init();

	SetUrl(lpUrl);
	SetMethod(HTTP_METHOD::POST);

	SetPostData(pszPostData);
	SetContentType(pzsContentType);
}

HttpRequest::~HttpRequest()
{
#ifdef _MEM_TRACE
	INFO_LOG("Delete HttpRequest Object [%p]", this);
#endif

	if (m_pHeaderList != NULL)
	{
		curl_slist_free_all(m_pHeaderList);
		m_pHeaderList = NULL;
	}
}

void HttpRequest::SetMethod(HTTP_METHOD enumMethod)
{
	m_enumMethod = enumMethod;
}

HTTP_METHOD HttpRequest::Method() const
{
	return m_enumMethod;
}

void HttpRequest::SetUrl(const std::string& strUrl)
{
	if (strUrl.empty())
	{
		return;
	}

	std::string tmpUrl = strUrl;

	if (UtilsString::IsEqualNoCase(tmpUrl.substr(0, 5), HTTPS_SCHEME))
	{
		m_bSSL = TRUE;
	}
	else if (UtilsString::IsEqualNoCase(tmpUrl.substr(0, 4), HTTP_SCHEME))
	{
		m_bSSL = FALSE;
	}
	else
	{
         tmpUrl = UtilsString::FormatString(("%s://%s"), m_bSSL ? HTTPS_SCHEME : HTTP_SCHEME, tmpUrl.c_str());
	}

	m_strUrl = tmpUrl;
}

const LPCSTR HttpRequest::Url()
{
	return m_strUrl.c_str();
}

void HttpRequest::SetSSL(BOOL bSSL /*= FALSE*/)
{
	m_bSSL = bSSL;
}

BOOL HttpRequest::SSL() const
{
	return m_bSSL;
}

void HttpRequest::SetContentType(const char* pszContentType)
{
	if (pszContentType == NULL)
	{
		pszContentType = CONTENT_TYPE_DEFAULT;
	}
	
	m_strContentType = pszContentType;

	AddHeader("Content-Type", pszContentType);
}

const LPCSTR HttpRequest::ContentType()
{
	return m_strContentType.c_str();
}

void HttpRequest::SetTimeOut(uint32_t timeout)
{
    m_timeout = timeout;
}

uint32_t HttpRequest::GetTimeOut()
{
    return m_timeout;
}

void HttpRequest::SetPostData(const char* pszPostData)
{
	if (pszPostData != NULL)
	{
		SetPostData(pszPostData, strlen(pszPostData));
	}
}

void HttpRequest::SetPostData(const char* pszPostData, UINT nLen)
{
	m_clsData.Reset();
	m_clsData.AppendData(pszPostData, nLen);
}

const char* HttpRequest::PostData()
{
	return m_clsData.Data();
}

const UINT HttpRequest::PostDataLen() const
{
	return m_clsData.Length();
}

LONG64 HttpRequest::RequestID() const
{
	return m_nReqID;
}

void HttpRequest::EnableDebug(BOOL bEnable /*= TRUE*/)
{
	m_bDebug = bEnable;
}

BOOL HttpRequest::Debug() const
{
	return m_bDebug;
}

void HttpRequest::SetReferer(const char* referer)
{
	m_strReferer = referer;
}

const std::string& HttpRequest::Referer() const
{
	return m_strReferer;
}

void HttpRequest::EnableRedirect(BOOL bEnable /*= TRUE*/)
{
	m_bRedirect = bEnable;
}

BOOL HttpRequest::Redirect() const
{
	return m_bRedirect;
}

void HttpRequest::SetOpType(HTTP_OP_TYPE enumOpType)
{
	m_enumOpType = enumOpType;
}

HTTP_OP_TYPE HttpRequest::OpType() const
{
	return m_enumOpType;
}

void HttpRequest::SaveAs(const std::wstring::value_type* save_as)
{
    if (save_as == nullptr)
    {
        return;
    }

	ASSERT(m_enumOpType == HTTP_OP_TYPE::FILE_DOWNLOAD);
	m_strSaveAs = save_as;
}

const std::wstring& HttpRequest::LocalSaveFileName() const
{
	return m_strSaveAs;
}

void HttpRequest::Init()
{
#ifdef _MEM_TRACE
	INFO_LOG("Create HttpRequest Object at [%p]", this);
#endif
	//modify by liuchengzhu 20200312
	//m_nReqID = InterlockedAdd64(&NEXT_GLOBE_HTTP_REQUEST_ID, 1);
	m_nReqID = NEXT_GLOBE_HTTP_REQUEST_ID + 1;
	m_pHeaderList = NULL;
	m_bSSL = FALSE;
	m_enumMethod = HTTP_METHOD::GET;
	m_enumOpType = HTTP_OP_TYPE::DATA_REQUEST;
	m_bRedirect = FALSE;
	m_bDebug = FALSE;
	m_hFile = INVALID_HANDLE_VALUE;
    m_timeout = 0;
	for (HEADERFIELDVECITR itr = s_vecHeader.begin(); itr != s_vecHeader.end(); ++itr)
	{
		AddHeader(itr->first.c_str(), itr->second.c_str());
	}

	for (PARAMENTVECITR itr = s_vecParam.begin(); itr != s_vecParam.end(); ++itr)
	{
		AddParamentImpl(m_vecParam, itr->first.c_str(), itr->second.c_str(), FALSE);
	}
}

HEADERFIELDVEC HttpRequest::s_vecHeader;

HEADERFIELDVEC HttpRequest::s_vecParam;

STRING_CODE_PAGE HttpRequest::s_enumCodePage = SCP_ANSI;

void HttpRequest::AddHeader(const char* key, const char* value)
{
	char header_field[256];
	sprintf_s(header_field, "%s: %s", key, value);

	m_pHeaderList = curl_slist_append(m_pHeaderList, header_field);
}

void HttpRequest::AddHeader(const char* key, int nValue)
{
	char value[128];
	sprintf_s(value, "%d", nValue);

	AddHeader(key, value);
}

void HttpRequest::AddHeader(const char* key, __int64 nValue)
{
	char value[128];
	sprintf_s(value, "%I64d", nValue);
	
	AddHeader(key, value);
}

void HttpRequest::AddGlobalHeader(const char* key, const char* value)
{
	s_vecHeader.push_back(KEYVALUEPAIR(key, value));
}

void HttpRequest::AddGlobalHeader(const char* key, int nValue)
{
	char value[128];
	sprintf_s(value, "%d", nValue);

	AddGlobalHeader(key, value);
}

void HttpRequest::AddGlobalHeader(const char* key, __int64 nValue)
{
	char value[128];
	sprintf_s(value, "%I64d", nValue);

	AddGlobalHeader(key, value);
}

void HttpRequest::AddParament(const char* key, const char* value, BOOL need_urlencode /*= TRUE*/)
{
	AddParamentImpl(m_vecParam, key, value, need_urlencode);
}

void HttpRequest::AddParament(const char* key, int nValue)
{
	char value[128];
	sprintf_s(value, "%d", nValue);

	AddParament(key, value);
}

void HttpRequest::AddParament(const char* key, __int64 nValue)
{
	char value[128];
	sprintf_s(value, "%I64d", nValue);

	AddParament(key, value);
}

void HttpRequest::AddGlobalParament(const char* key, const char* value)
{
	AddParamentImpl(s_vecParam, key, value);
}

void HttpRequest::AddGlobalParament(const char* key, int nValue)
{
	char value[128];
	sprintf_s(value, "%d", nValue);

	AddGlobalParament(key, value);
}

void HttpRequest::AddGlobalParament(const char* key, __int64 nValue)
{
	char value[128];
	sprintf_s(value, "%I64d", nValue);

	AddGlobalParament(key, value);
}

void HttpRequest::AddParamentImpl(PARAMENTVEC& param_vec, const char* key, const char* value, BOOL need_urlencode /*= TRUE*/)
{
    if ((key == nullptr) ||
        (value == nullptr))
    {
        return;
    }

	if (need_urlencode)
	{
        std::string strKeyTmp = UrlEncode(key);
        std::string strValueTmp = UrlEncode(value);
		StaticString key_as_ut8(strKeyTmp.c_str());
		StaticString value_as_ut8(strValueTmp.c_str());

        std::string strKey;
        std::string strValue;

        const char* pKey = key_as_ut8.AsU8String();
        if (pKey != nullptr)
        {
            strKey = pKey;
        }

        const char* pValue = value_as_ut8.AsU8String();
        if (pValue != nullptr)
        {
            strValue = pValue;
        }
		param_vec.push_back(KEYVALUEPAIR(strKey, strValue));

	}
	else
	{
		param_vec.push_back(KEYVALUEPAIR(key, value));
	}
}

RequestPtr HttpRequest::Create()
{
	RequestPtr pRequest(new HttpRequest);
	return pRequest;
}

RequestPtr HttpRequest::Create(const char* lpUrl, HTTP_METHOD enumMethod /*= HTTP_METHOD::GET*/)
{
	RequestPtr pRequest(new HttpRequest(lpUrl, enumMethod));
	return pRequest;
}

RequestPtr HttpRequest::Create(const char* lpUrl, const char* pszPostData, const char* pzsContentType /*= CONTENT_TYPE_DEFAULT*/)
{
	RequestPtr pRequest(new HttpRequest(lpUrl, pszPostData, pzsContentType));
	return pRequest;
}

const struct curl_slist* HttpRequest::HeaderList() const
{
	return m_pHeaderList;
}

const std::string HttpRequest::RequestString(CURL* curl_handle)
{
	ASSERT(curl_handle != NULL);

	if (m_vecParam.empty())
	{
		return m_strUrl;
	}

	std::string strRequestString = m_strUrl.c_str();

	BOOL bFirstParam = TRUE;

	//int nPos = strRequestString.ReverseFind('?');
	int nPos = strRequestString.find('?');
	
	if (nPos > 0)
	{
		bFirstParam = FALSE;
	}
	else
	{
		//strRequestString.AppendChar('?');
		strRequestString += "?";
	}

	for (PARAMENTVECITR itr = m_vecParam.begin(); itr != m_vecParam.end(); ++itr)
	{
		if (bFirstParam)
		{
			//strRequestString.AppendFormat("%s=%s",
			//	itr->first.c_str(),
			//	itr->second.c_str());

			strRequestString = UtilsString::FormatString("%s%s=%s",
				strRequestString.c_str(),
				itr->first.c_str(),
				itr->second.c_str());

			bFirstParam = FALSE;
		}
		else
		{
			//strRequestString.AppendFormat("&%s=%s",
			//	itr->first.c_str(),
			//	itr->second.c_str());

			strRequestString = UtilsString::FormatString("%s&%s=%s",
				strRequestString.c_str(),
				itr->first.c_str(),
				itr->second.c_str());
		}
	}

	return strRequestString;
}

unsigned char ToHex(unsigned char x)
{
    return  x > 9 ? x + 55 : x + 48;
}

unsigned char FromHex(unsigned char x)
{
    unsigned char y;
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
    return y;
}

std::string HttpRequest::UrlEncode(const std::string& str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (isalnum((unsigned char)str[i]) ||
            (str[i] == '-') ||
            (str[i] == '_') ||
            (str[i] == '.') ||
            (str[i] == '~'))
            strTemp += str[i];
        else if (str[i] == ' ')
            strTemp += "+";
        else
        {
            strTemp += '%';
            strTemp += ToHex((unsigned char)str[i] >> 4);
            strTemp += ToHex((unsigned char)str[i] % 16);
        }
    }
    return strTemp;
}

std::string HttpRequest::UrlDecode(const std::string& str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (str[i] == '+') strTemp += ' ';
        else if (str[i] == '%')
        {
            unsigned char high = FromHex((unsigned char)str[++i]);
            unsigned char low = FromHex((unsigned char)str[++i]);
            strTemp += high * 16 + low;
        }
        else strTemp += str[i];
    }
    return strTemp;
}


HttpResponse::HttpResponse() : m_nCurlCode(CURLE_OK)
{
#ifdef _MEM_TRACE
	INFO_LOG("Create HttpResponse Object at [%p]", this);
#endif
}

HttpResponse::~HttpResponse()
{
#ifdef _MEM_TRACE
	INFO_LOG("Delete HttpResponse Object [%p]", this);
#endif
}

UINT HttpResponse::AppendData(const char* pData, UINT nLen)
{
	 m_clsData.append(pData, nLen);
     return m_clsData.size();
}

UINT HttpResponse::AppendHeader(const char* pData, UINT nLen)
{
	m_clsHeader.append(pData, nLen);
    return m_clsHeader.size();
}

const char* HttpResponse::Data()
{
	return m_clsData.data();
}

const char* HttpResponse::Head()
{
	return m_clsHeader.data();
}

UINT HttpResponse::HeadLen() const
{
	return m_clsData.size();
}

std::string HttpResponse::GetHeaderValue(const char* pKey)
{
	const char* pHead = Head();
	if (nullptr == pHead  || nullptr == pKey) return "";

	std::istringstream resp(pHead);
	std::string header;
	std::string::size_type index;
	std::string::size_type idx_r;
	while (std::getline(resp, header))
	{
        if (header == "\r")
        {
            continue;
        }
		index = header.find(':', 0);
		idx_r = header.find("\r", 0);
		if ( (index != std::string::npos) && (idx_r != std::string::npos)) 
		{
			std::string sKey = header.substr(0, index);
			std::string sValue = header.substr(index + 1, idx_r-index-1);

			if (sKey == pKey)
				return sValue;			
		}
	}
	
	return "";
}

UINT HttpResponse::Len() const
{
	return m_clsData.size();
}

void HttpResponse::SetCurlCode(CURLcode code)
{
	m_nCurlCode = code;
}

CURLcode HttpResponse::CurlCode() const
{
	return m_nCurlCode;
}

void HttpResponse::SetRequestString(const char* pRequestString)
{
    if (pRequestString == nullptr)
    {
        return;
    }

	m_strRequsetString = pRequestString;
}

const char* HttpResponse::RequestString()
{
	return m_strRequsetString.c_str();
}

void HttpResponse::SetTotalTime(long long lTotalTime)
{
	m_lTotalTime = lTotalTime;
}

long HttpResponse::TotalTime() const
{
	return (long)(m_lTotalTime / 1000);
}

void HttpResponse::SetHttpStatusCode(int nHttpStatusCode)
{
	m_nHttpStatusCode = nHttpStatusCode;
}

int HttpResponse::HttpStatusCode() const
{
	return m_nHttpStatusCode;
}

void HttpResponse::DebugPrintHeader()
{
	if (!m_clsHeader.empty())
	{
		//DEBUG_LOG(m_clsHeader.Data());
	}
}

void HttpResponse::DebugPrintBody()
{
	//DEBUG_LOG(m_clsData.Data());

// 	const char* p = Data();
// 	while (*p++)
// 	{
// 		printf("%c", *p);
// 	}

}

void HttpResponse::DebugPrintSummary()
{
	//DEBUG_LOG("请求地址 [%s]\n请求耗时 [%d] 毫秒，http协议头长度 [%d] bytes,数据长度 [%d] bytes", RequestString(), TotalTime(), m_clsHeader.Length(), m_clsData.Length());
}

void HttpResponse::DebugPrint()
{
	DebugPrintSummary();
	DebugPrintHeader();
	DebugPrintBody();
}

BOOL HttpResponse::Succeed()
{
	return (m_nCurlCode == CURLE_OK) && (m_nHttpStatusCode == HTTP_STATUS_CODE_OK);
}

HStringPtr HttpResponse::ResponseString()
{
	HStringPtr pResponseString(new StaticString(Data(), SCP_UTF8));

	return pResponseString;
}

ResponsePtr HttpResponse::Create()
{
	ResponsePtr pResponse(new HttpResponse);
	return pResponse;
}