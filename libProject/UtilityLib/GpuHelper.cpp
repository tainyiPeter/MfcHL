#include "GpuHelper.h"



#pragma comment(lib, "dxgi.lib")



int GupDisplayer::GetAdaptersByPreference(std::vector<GPUAdapterInfo>& vecAdapters)
{
	HRESULT hres = 0;
	std::vector<GPUAdapterInfo> adapters;

	Microsoft::WRL::ComPtr<IDXGIFactory6> pFactory = nullptr;
	REFIID iidVal = __uuidof(IDXGIFactory6);
	hres = CreateDXGIFactory2(0, iidVal, &pFactory);
	if (!SUCCEEDED(hres)) {
		wprintf(L"GetDGPUDeviceName, CreateDXGIFactory6 failed: %d", hres);
		return 1;
	}

	Microsoft::WRL::ComPtr<IDXGIAdapter1> pAdapter;
	UINT adapterIndex = 0;

	// DGPU（DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE）
	hres = pFactory->EnumAdapterByGpuPreference(
		0,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
		IID_PPV_ARGS(&pAdapter));
	DXGI_ADAPTER_DESC1 desc;
	if (SUCCEEDED(pAdapter->GetDesc1(&desc))) {
		GPUAdapterInfo info;
		info.Desc = desc;
		info.Description = desc.Description;

		info.IsDiscrete = true;	//指定DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE时，index 0是dgpu

		// 判断厂商
		switch (desc.VendorId) {
		case 0x10DE: info.Vendor = L"NVIDIA"; break;
		case 0x1002: info.Vendor = L"AMD"; break;
		case 0x8086: info.Vendor = L"Intel"; break;
		case 0x1414: info.Vendor = L"Microsoft"; break; // 软件适配器
		default: info.Vendor = L"Unknown"; break;
		}

		adapters.push_back(info);
	}
	pAdapter.Reset();

	// IGPU（DXGI_GPU_PREFERENCE_MINIMUM_POWER）
	hres = pFactory->EnumAdapterByGpuPreference(
		0,
		DXGI_GPU_PREFERENCE_MINIMUM_POWER,
		IID_PPV_ARGS(&pAdapter));
	//DXGI_ADAPTER_DESC1 desc;
	if (SUCCEEDED(pAdapter->GetDesc1(&desc))) {
		GPUAdapterInfo info;
		info.Desc = desc;
		info.Description = desc.Description;

		info.IsDiscrete = false;	//指定DXGI_GPU_PREFERENCE_MINIMUM_POWER时，index 0是igpu

		// 判断厂商
		switch (desc.VendorId) {
		case 0x10DE: info.Vendor = L"NVIDIA"; break;
		case 0x1002: info.Vendor = L"AMD"; break;
		case 0x8086: info.Vendor = L"Intel"; break;
		case 0x1414: info.Vendor = L"Microsoft"; break; // 软件适配器
		default: info.Vendor = L"Unknown"; break;
		}

		adapters.push_back(info);
	}
	pAdapter.Reset();

	vecAdapters = std::move(adapters);
	return 0;
}

int GupDisplayer::PrintDGpuAndIGpu()
{
	std::vector<GPUAdapterInfo> adapters;
	GetAdaptersByPreference(adapters);

	for (const auto& adapter : adapters) {
		std::wcout << L"desc: " << adapter.Description << std::endl;
		std::wcout << L"vendor: " << adapter.Vendor << std::endl;
		std::wcout << L"type: " << (adapter.IsDiscrete ? L"Discrete" : L"Integrated") << std::endl;
		std::wcout << L"memory: " << (adapter.Desc.DedicatedVideoMemory / 1024 / 1024) << L" MB" << std::endl;
		std::wcout << L"--------------------------------" << std::endl;
	}

	return 0;
}