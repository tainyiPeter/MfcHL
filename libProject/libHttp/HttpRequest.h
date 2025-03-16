#pragma once

#include "PrepareDef.h"

#ifdef HTTPLIB_PROJECT
#define EXTERN_CLASS _declspec(dllexport)
#else
#define EXTERN_CLASS _declspec(dllimport)
#endif


#include "Utils/std_macro.h"
#include "Utils/SmartBuffer.h"
#include "Utils/StaticString.h"

class HttpRequest;
class HttpResponse;

typedef std::shared_ptr<HttpRequest>	RequestPtr;
typedef std::shared_ptr<HttpResponse>	ResponsePtr;

extern const char CONTENT_TYPE_JSON[];
extern const char CONTENT_TYPE_FORM_DATA[];
extern const char CONTENT_TYPE_URLENCODED[];
extern const char CONTENT_TYPE_STREAM[];

#define CONTENT_TYPE_DEFAULT					CONTENT_TYPE_URLENCODED


enum class HTTP_METHOD
{
	GET = 0,
	POST
};

enum class HTTP_OP_TYPE
{
	DATA_REQUEST = 0,            // 数据请求
	FILE_DOWNLOAD                // 下载文件
};

DECLARE_PAIR(std::string, std::string, KEYVALUEPAIR)
DECLARE_VECTOR(KEYVALUEPAIR, HEADERFIELDVEC)
DECLARE_VECTOR(KEYVALUEPAIR, PARAMENTVEC)

class EXTERN_CLASS HttpRequest
{
public:
	~HttpRequest();

	void SetMethod(HTTP_METHOD enumMethod);
	HTTP_METHOD Method() const;

	void SetUrl(const std::string& strUrl);
    const LPCSTR Url();

	void SetSSL(BOOL bSSL = FALSE);
	BOOL SSL() const;

	void SetContentType(const char* pszContentType);
	const LPCSTR ContentType();

    void SetTimeOut(uint32_t timeout);
    uint32_t GetTimeOut();

	void SetPostData(const char* pszPostData);
	void SetPostData(const char* pszPostData, UINT nLen);
	const char* PostData();
	const UINT PostDataLen() const;

	void AddHeader(const char* key, const char* value);
	void AddHeader(const char* key, int nValue);
	void AddHeader(const char* key, __int64 nValue);

	void AddParament(const char* key, const char* value, BOOL need_urlencode = TRUE);
	void AddParament(const char* key, int nValue);
	void AddParament(const char* key, __int64 nValue);

	const struct curl_slist* HeaderList() const;
	const std::string RequestString(CURL* curl_handle);
	LONG64 RequestID() const;

	void EnableDebug(BOOL bEnable = TRUE);
	BOOL Debug() const;

	void SetReferer(const char* referer);
	const std::string& Referer() const;

	void EnableRedirect(BOOL bEnable = TRUE);
	BOOL Redirect() const;

	void SetOpType(HTTP_OP_TYPE enumOpType);
	HTTP_OP_TYPE OpType() const;

	void SaveAs(const std::wstring::value_type* save_as);
	const std::wstring& LocalSaveFileName() const;

    static std::string UrlEncode(const std::string& str);
    static std::string UrlDecode(const std::string& str);

public:
	static void AddGlobalHeader(const char* key, const char* value);
	static void AddGlobalHeader(const char* key, int nValue);
	static void AddGlobalHeader(const char* key, __int64 nValue);

	static void AddGlobalParament(const char* key, const char* value);
	static void AddGlobalParament(const char* key, int nValue);
	static void AddGlobalParament(const char* key, __int64 nValue);

	static RequestPtr Create();
	static RequestPtr Create(const char* lpUrl, HTTP_METHOD enumMethod = HTTP_METHOD::GET);
	static RequestPtr Create(const char* lpUrl, const char* pszPostData, const char* pzsContentType = CONTENT_TYPE_DEFAULT);

protected:
	HttpRequest(const char* lpUrl, HTTP_METHOD enumMethod = HTTP_METHOD::GET);
	HttpRequest(const char* lpUrl, const char* pszPostData, const char* pzsContentType = CONTENT_TYPE_DEFAULT);
	HttpRequest();

	void Init();

	static void AddParamentImpl(PARAMENTVEC& param_vec, const char* key, const char* value, BOOL need_urlencode = TRUE);
private:
	HTTP_METHOD m_enumMethod;
	HTTP_OP_TYPE m_enumOpType;

	std::string m_strUrl;
    std::string m_strContentType;
	std::wstring m_strSaveAs;
	std::string m_strReferer;
	
	HANDLE m_hFile;
	BOOL m_bSSL;
	BOOL m_bDebug;
	BOOL m_bRedirect;
	SmartBuffer	m_clsData;
	LONG64 m_nReqID;
    uint32_t m_timeout;

	static HEADERFIELDVEC s_vecHeader;

	PARAMENTVEC m_vecParam;
	static HEADERFIELDVEC s_vecParam;

	struct curl_slist* m_pHeaderList;

	static STRING_CODE_PAGE s_enumCodePage;			// 预留一个编码控制变量
};

class EXTERN_CLASS HttpResponse
{
public:
	~HttpResponse();

	UINT AppendData(const char* pData, UINT nLen);
	UINT AppendHeader(const char* pData, UINT nLen);

	const char* Data();

	const char* Head();
	UINT HeadLen() const;
	std::string GetHeaderValue(const char* pKey);

	UINT Len() const;
	UINT Capacity() const;

	void SetCurlCode(CURLcode code);
	CURLcode CurlCode() const;

	void SetRequestString(const char* pRequestString);
    const char* RequestString();

	void SetTotalTime(long long lTotalTime);
	long TotalTime() const;

	void SetHttpStatusCode(int nHttpStatusCode);
	int HttpStatusCode() const;

	void DebugPrintHeader();
	void DebugPrintBody();
	void DebugPrintSummary();
	void DebugPrint();

	BOOL Succeed();
	HStringPtr ResponseString();
public:
	static ResponsePtr Create();

protected:
	HttpResponse();

private:
	int m_nHttpStatusCode;
    std::string m_clsData;
    std::string m_clsHeader;

    std::string m_strRequsetString;

	CURLcode m_nCurlCode;
	HEADERFIELDVEC m_vecHeader;

	int64_t m_lTotalTime;
};