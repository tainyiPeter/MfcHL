// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include "ThirdParty/NVIDIA_API/include/nvapi.h"
#include "DebugUtil.h"

class GpuThermalInterface
{
public:
	virtual bool SetTGP(int level) = 0; 
	virtual bool SetTargetTemperature(int temperature) = 0;
};

class NVI2CInterface
{
public:
	virtual bool ReadI2C(BYTE addr, BYTE data, BYTE devAddr = 0x49) = 0;
	virtual bool WriteI2C(BYTE addr, BYTE data, BYTE devAddr = 0x49) = 0;
	virtual bool IsHandleEmpty() = 0;
	virtual ~NVI2CInterface() {};
	virtual NvPhysicalGpuHandle get_nv_physical_gpu_handle() = 0;
};

class NVGpuHelper : public GpuThermalInterface, public NVI2CInterface
{
public:
	NVGpuHelper(unsigned long vendor_id, unsigned long device_id);
	NVGpuHelper();
	~NVGpuHelper() {
		if (_is_init == NVAPI_OK) {
			NvAPI_Unload();
		}
	}
	// Inherited via GpuThermalInterface
	virtual bool SetTGP(int level) override;
	virtual bool SetTargetTemperature(int temperature) override;
	virtual bool ReadI2C(BYTE addr, BYTE data, BYTE devAddr = 0x49) override;
	virtual bool WriteI2C(BYTE addr, BYTE data, BYTE devAddr = 0x49) override;

	virtual bool IsHandleEmpty() override
	{
		return  _handle == nullptr;
	}

	[[nodiscard]] NvPhysicalGpuHandle get_nv_physical_gpu_handle() override
	{
		return _handle;
	}

private:
	NvAPI_Status _is_init;
	NvPhysicalGpuHandle _handle;
};