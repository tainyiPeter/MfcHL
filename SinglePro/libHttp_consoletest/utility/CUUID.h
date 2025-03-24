#pragma once

#include <atlstr.h>
#include <string>

namespace SmartEngine
{
	class CUUID
	{

	public:
		static UUID GenerateUUID()
		{
			UUID uuid = { 0 };
			if (UuidCreate(&uuid) != RPC_S_OK)
			{
				ZeroMemory(&uuid, sizeof(UUID));
				uuid.Data1 = GetTickCount();
			}
			return uuid;
		}

		static UUID GenerateUUID(std::string& result)
		{
			UUID uuid = { 0 };
			if (UuidCreate(&uuid) != RPC_S_OK)
			{
				ZeroMemory(&uuid, sizeof(UUID));
				uuid.Data1 = GetTickCount();
			}
			unsigned char* str = NULL;
			UuidToStringA(&uuid, &str);
			if (str != NULL)
			{
				result = reinterpret_cast<char*>(str);
			}
			RpcStringFreeA(&str);
			return uuid;
		}

		static UUID GenerateUUID(CString& result)
		{
			UUID uuid = { 0 };
			if (UuidCreate(&uuid) != RPC_S_OK)
			{
				ZeroMemory(&uuid, sizeof(UUID));
				uuid.Data1 = GetTickCount();
			}
			unsigned short* str = NULL;
			UuidToStringW(&uuid, &str);
			if (str != NULL)
			{
				result = reinterpret_cast<wchar_t*>(str);
			}
			RpcStringFreeW(&str);
			return uuid;
		}
	};
}

