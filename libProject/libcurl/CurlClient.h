#pragma once

#include <string>
#include <functional>
#include <atlstr.h>
#include <curl/curl.h>

using namespace std;

class CCurlClient 
{
public:
	CCurlClient(CString url);

	~CCurlClient();

	int HttpGet(string& response, long& statusCode, int timeout);

	int HttpGet(long& statusCode, int timeout);

	int HttpPost(string& response, string requestBody, long& statusCode, string contenttype, int timeout);

	BOOL DownloadFile(CString localPath, BOOL breakContinue = FALSE, std::function<void(INT64 complete, INT64 totle, double speed)> progressCallback = NULL);

	int32_t TestDLFile(CString localPath);

	//localpath��ָ�����ص�Ŀ¼�� filePath �������ص��ļ�·�����ļ�������http��Э��������
	//���û�м�����ļ����������ʹ��һ��exe���ļ��� ������ȶ��
	BOOL DownloadFile(CString localDictory, CString& filePath, BOOL breakContinue = FALSE, std::function<void(INT64 complete, INT64 totle, double speed)> progressCallback = NULL);

	BOOL InitDownloadPath();

	BOOL CancelDownload();

	/** only test
	*/
	int32_t TestPost();

private:
	long GetDownloadFileLength();
	CString GetDownloadFileName();
	CString GetFileNameByUrl(string url);
	UINT64 GetFileSize(const CString& FilePath);
public:
	BOOL m_canceled = FALSE;
	std::string m_url;
	std::function<void(INT64 complete, INT64 totle, double speed)> m_processCallback;
	int m_connectTimeout;

	void* m_curHandle;
	CString m_downloadDictory;
	CString m_downloadFilePath;

	//�ض����URL��ַ,��httpheader ��ȡ
	string m_lastRedirectUrl;
	//header content-disposition: ������ļ���
	string m_dispositionFileName;
};

struct Curl_Progress_User_Data
{
	CCurlClient* client;
	CURL* handle;
	curl_off_t breakPoint = 0;
	curl_off_t length = 0;
	time_t lasttime = 0;
	int errorCode = 0;

	curl_off_t lastdl = 0;
	time_t lastdltime = 0;
};


#define HTTP_ERROR_UNKNOW			99
#define HTTP_ERROR_FILELENGTH		100		//100 ��ȡ��������
#define HTTP_ERROR_OPENFILE			101		//�ļ��޷���
#define HTTP_ERROR_CURINIT_FAILED	102		//102 curl ��ʼ��ʧ��
#define HTTP_ERROR_CANELED			103		//ȡ������
#define HTTP_ERROR_SETTINGS			104		//��ȡ�ļ���С��ʱ�� ����ʧ��
#define HTTP_ERROR_GET_LENGTH		105		//��ȡ�ļ���С��ʱ�� ִ��ʧ��
#define HTTP_ERROR_GET_INFO_FAILED	106		//curl_easy_getinfo ��ȡ��Сʧ��
#define HTTP_ERROR_CURSET_FAILED	107		//107 ����ʧ��
#define HTTP_ERROR_DWONLOAD_ERROR	108
#define HTTP_ERROR_FILENAME_NONE	109

#define CURL_CANCEL_NODATA			9998	//������Ѷ����ʧ�ܵ���� 5�볢��
#define CURL_CANCEL_NET_NODATA		9999	//�������ع����У������ն� 10�볢��
