#ifndef INCLUDED_JSON_WRAP
#define INCLUDED_JSON_WRAP

class JsonArray;
class JsonObject;

#include "../include/json.h"
#include <atlstr.h>

// json��ȡ��װ�����ڽ�����Ͷ�ȡ����ʱ�����̱���������

class JsonDoc
{
public:
    bool  Parse(LPCWSTR, JsonObject&);
    bool  Parse(LPCSTR, JsonObject&);

private:
    Json::Reader  m_reader;
};

// ��������
class JsonObject
{
public:
    bool  GetString(const char* key, CStringA&);
    bool  GetString(const char* key, CString&);
    bool  GetInt(const char* key, int&);
	bool  GetBool(const char* key, bool&);
    bool  GetArray(const char* key, JsonArray&);
    bool  GetObject(const char* key, JsonObject&);

	bool  IsNull() const;
private:
    // ����Json::Reader::parseʱ��ȡroot value
    operator Json::Value&() { return m_value; }
    
private:
    Json::Value  m_value;
    friend class JsonArray;
    friend class JsonDoc;
};

// ��������
class JsonArray
{
public:
    bool  GetSubString(ULONG nIndex, CStringA&);
    bool  GetSubInt(ULONG nIndex, int&);
    bool  GetSubArray(ULONG nIndex, JsonArray&);
    bool  GetSubObject(ULONG nIndex, JsonObject&);
    ULONG  GetSize();

private:
    Json::Value  m_value;
    friend class JsonObject;
};

#endif // INCLUDED_JSON_WRAP