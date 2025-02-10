// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#include "NVHelper.h"
//NvAPI_GPU_GetPCIIdentifiers
NVGpuHelper::NVGpuHelper(unsigned long vendor_id, unsigned long device_id) :_handle(nullptr) {
	NvPhysicalGpuHandle Gpuhandle[NVAPI_MAX_PHYSICAL_GPUS] = { 0 };
	int gpuId = 0;
	NvU32 match_device_id = (NvU32)device_id << 16;
	match_device_id += vendor_id;
	_is_init = NvAPI_Initialize();
	if (_is_init == NVAPI_OK)
	{
		NvU32 gpuCount = 0;
		if ((NvAPI_EnumPhysicalGPUs(Gpuhandle, &gpuCount) != NVAPI_OK) || (gpuCount < 1))
		{
			DBG_PRINTF_FL((L"NVGpuHelper Enumerate NV API failed %x\n", GetLastError()));
		}
		else
		{
			for (int i = 0; i < gpuCount; i++) {
				NvU32 DeviceId, SubSystemId, RevisionId, ExtDeviceId;
				if (NvAPI_GPU_GetPCIIdentifiers(Gpuhandle[i], &DeviceId, &SubSystemId, &RevisionId, &ExtDeviceId) == NVAPI_OK) {
					DBG_PRINTF_FL((L"NVGpuHelper device ,device id %x\n", DeviceId));
					if (DeviceId == match_device_id) {
						DBG_PRINTF_FL((L"NVGpuHelper found device vendorid %x,device id %x\n",vendor_id,device_id));
						_handle = Gpuhandle[i];
					}
				}
			}
		}
	}
	else {
		DBG_PRINTF_FL((L"NVGpuHelper Init NV API failed %x\n", GetLastError()));
	}
}

NVGpuHelper::NVGpuHelper() :_handle(nullptr) {
	NvPhysicalGpuHandle Gpuhandle[NVAPI_MAX_PHYSICAL_GPUS] = { 0 };
	_is_init = NvAPI_Initialize();
	if (_is_init == NVAPI_OK)
	{
		NvU32 gpuCount = 0;
		if ((NvAPI_EnumPhysicalGPUs(Gpuhandle, &gpuCount) != NVAPI_OK) || (gpuCount < 1))
		{
			DBG_PRINTF_FL((L"NVGpuHelper Enumerate NV API failed %x\n", GetLastError()));
		}
		else
			_handle = Gpuhandle[0];
	}
	else {
		DBG_PRINTF_FL((L"NVGpuHelper Init NV API failed %x\n", GetLastError()));
	}
}

bool NVGpuHelper::SetTGP(int level)
{
	if (_handle != nullptr) {
		NV_GPU_CLIENT_POWER_POLICIES_STATUS GPU_POWER_Status = { 0 };
		GPU_POWER_Status.version = NV_GPU_CLIENT_POWER_POLICIES_STATUS_VER;
		GPU_POWER_Status.numPolicies = 1;
		GPU_POWER_Status.policies[0].policyId = NV_GPU_CLIENT_POWER_POLICIES_POLICY_TOTAL_GPU_POWER;
		GPU_POWER_Status.policies[0].powerLimit.bMilliWattValid = 0;
		GPU_POWER_Status.policies[0].policyClass = NV_GPU_CLIENT_POWER_POLICIES_POLICY_CLASS_BASE;
		GPU_POWER_Status.policies[0].powerLimit.mp = level*1000;  // Setting the GPU's target TDP
		GPU_POWER_Status.policies[0].classStatus.pff.bPffCurveChanged = 0;
		
		NvAPI_Status status = NvAPI_GPU_ClientPowerPoliciesSetStatus(_handle, &GPU_POWER_Status);
		if(status != NVAPI_OK)
			DBG_PRINTF_FL((L"NVGpuHelper SetTGP %x\n", status));
		return status == NVAPI_OK?true:false;
	}

	return false;
}

bool NVGpuHelper::SetTargetTemperature(int temperature)
{
	if (_handle != nullptr) {
		NV_GPU_CLIENT_THERMAL_POLICIES_STATUS polStat = { 0 };
		polStat.version = NV_GPU_CLIENT_THERMAL_POLICIES_STATUS_VER;
		polStat.numPolicies = 1;
		polStat.policies[0].policyId = NV_GPU_CLIENT_THERMAL_POLICIES_POLICY_ACOUSTIC;
		polStat.policies[0].thermalLimit = (NvS32)(temperature * 256.0);    // Set target temperature.   
		NvAPI_Status status = NvAPI_GPU_ClientThermalPoliciesSetStatus(_handle, &polStat);
		if (status != NVAPI_OK)
			DBG_PRINTF_FL((L"NVGpuHelper SetTargetTemperature %x\n", status));
		return status == NVAPI_OK;
	}
	
	return false;
}

bool NVGpuHelper::ReadI2C(BYTE addr, BYTE data, BYTE devAddr)
{
	if (_handle == nullptr)
		return false;

	NV_I2C_INFO pI2cInfo = { 0 };
	pI2cInfo.version = NV_I2C_INFO_VER;
	pI2cInfo.displayMask = 1;
	pI2cInfo.bIsDDCPort = 1;
	NvU8 i2cDevAddr = devAddr;
	NvU8 i2cReadDeviceAdd = (i2cDevAddr << 1) | 1;
	BYTE destADDR[1] = { 0 };
	BYTE databuffBytes[1] = { 0 };
	pI2cInfo.bIsDDCPort = TRUE;
	pI2cInfo.i2cDevAddress = i2cReadDeviceAdd;
	pI2cInfo.pbI2cRegAddress = (BYTE*)&destADDR;
	pI2cInfo.regAddrSize = sizeof(destADDR);
	pI2cInfo.pbData = (BYTE*)&databuffBytes;
	pI2cInfo.cbSize = sizeof(databuffBytes);
	pI2cInfo.i2cSpeed = NVAPI_I2C_SPEED_DEPRECATED;
	pI2cInfo.i2cSpeedKhz = NVAPI_I2C_SPEED_400KHZ;
	pI2cInfo.bIsPortIdSet = TRUE;
	pI2cInfo.portId = 1;

	destADDR[0] = { addr };
	databuffBytes[0] = { data };

	NvAPI_Status ret = NvAPI_I2CRead(_handle, &pI2cInfo);
	DBG_PRINTF_FL((L"NVLedAPI WriteI2C return with status %d key=%x value=%x\n", ret, addr, data));
	if (ret == NVAPI_OK)
		return true;

	return false;
}

bool NVGpuHelper::WriteI2C(BYTE addr, BYTE data, BYTE devAddr)
{
	if (_handle == 0)
		return false;

	NV_I2C_INFO pI2cInfo = { 0 };
	pI2cInfo.version = NV_I2C_INFO_VER;
	pI2cInfo.displayMask = 1;
	pI2cInfo.bIsDDCPort = 1;
	NvU8 i2cDevAddr = devAddr;
	NvU8 i2cWriteDeviceAdd = i2cDevAddr << 1;
	pI2cInfo.bIsDDCPort = TRUE;
	pI2cInfo.i2cDevAddress = i2cWriteDeviceAdd;
	pI2cInfo.pbI2cRegAddress = (BYTE*)&addr;
	pI2cInfo.regAddrSize = sizeof(BYTE);
	pI2cInfo.pbData = (BYTE*)&data;
	pI2cInfo.cbSize = sizeof(data);
	pI2cInfo.i2cSpeed = NVAPI_I2C_SPEED_DEPRECATED;
	pI2cInfo.i2cSpeedKhz = NVAPI_I2C_SPEED_400KHZ;
	pI2cInfo.bIsPortIdSet = TRUE;
	pI2cInfo.portId = 1;

	NvAPI_Status ret = NvAPI_I2CWrite(_handle, &pI2cInfo);
	DBG_PRINTF_FL((L"NVLedAPI WriteI2C return with status %d key=%x value=%x\n", ret, addr, data));
	if (ret == NVAPI_OK)
		return true;
	
	return false;
}
