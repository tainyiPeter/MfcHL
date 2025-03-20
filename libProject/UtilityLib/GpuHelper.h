#pragma once

#include <Windows.h>  
#include <pdh.h>
#include <pdhmsg.h>
#include <dxgi1_6.h>
#include <DXGI.h>
#include <wrl/client.h>

#include <cstdio>
#include <vector>
#include <string>
#include <set>
#include <iostream>

struct GPUAdapterInfo {
	std::wstring Description;
	DXGI_ADAPTER_DESC1 Desc;
	bool IsDiscrete;  // 是否是独立显卡
	std::wstring Vendor; // 厂商名称
};

class GupDisplayer
{
public:

	static int GetAdaptersByPreference(std::vector<GPUAdapterInfo>& vecAdapters);
	static int PrintDGpuAndIGpu();

};
