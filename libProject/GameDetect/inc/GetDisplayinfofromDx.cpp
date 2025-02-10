#include <D3D10_1.h>
#include <dxgi1_6.h>
#include <DXGI.h>
#include <functional>
#include <algorithm>
#include <vector>
#include "Registry.h"
#include "DebugUtil.h"
#include "GetDisplayinfofromDx.h"

#pragma comment(lib, "dxgi.lib")

using namespace registry;


void ForEachGraphicCard(std::function<bool(const wchar_t * displayName, const wchar_t * cardInfo, const wchar_t  * LowPart, const wchar_t * HighPart) > func)
{
	IDXGIFactory1 *factory = NULL;
	if (SUCCEEDED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory)))
	{
		UINT i = 0;
		IDXGIAdapter1 *giAdapter = NULL;
		while (factory->EnumAdapters1(i++, &giAdapter) == S_OK)
		{
			DXGI_ADAPTER_DESC adapterDesc;
			if (SUCCEEDED(giAdapter->GetDesc(&adapterDesc)))
			{
				giAdapter->Release();
				CString strLow;
				CString strHigh;
				strLow.Format(L"0x%08x", adapterDesc.AdapterLuid.LowPart);
				strHigh.Format(L"0x%08x", adapterDesc.AdapterLuid.HighPart);
				CString strCardInfo;
				strCardInfo.Format(L"ven_%x&dev_%x&subsys_%x&rev_%x", adapterDesc.VendorId, adapterDesc.DeviceId, adapterDesc.SubSysId, adapterDesc.Revision);
				if (func(adapterDesc.Description, strCardInfo.GetBuffer(), strLow.GetBuffer(), strHigh.GetBuffer()))
					break;
			}			
		}
		factory->Release();
	}
}