// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once

#include "nt_system_information_undoc_struct.h"
#include "inc/TimeOutThread.hpp"

class cpu_frequency
{
public:
	cpu_frequency() :
	time_out_thread_(k_time_out_times_, [this] { update_frequency(); })
	{
		time_out_thread_.set_helper_desc("cpu_frequency");
		init();
	}
	double get_current_frequency()
	{
		time_out_thread_.Start();
		return current_cpu_frequency_;
	}
	std::string s_get_current_frequency(){
		stringstream ss;
		ss.setf(ios::fixed);
		ss.precision(2);
		ss << get_current_frequency() << "GHz";
		return ss.str();
	}

	~cpu_frequency()
	{
		time_out_thread_.Stop();
		DBG_PRINTF_FL((L"time out thread stopped %d",time_out_thread_.get_thread_id()));


		free(power_information_information_);

		if (current_performance_distribution_distribution_)
			free(current_performance_distribution_distribution_);
		if (previous_performance_distribution_distribution_)
			free(previous_performance_distribution_distribution_);

		if (nt_dll_handle_)
			FreeLibrary(nt_dll_handle_);

	}

private:
	TimeOutThread time_out_thread_;
	PSYSTEM_PROCESSOR_PERFORMANCE_DISTRIBUTION current_performance_distribution_distribution_ = nullptr;
	PSYSTEM_PROCESSOR_PERFORMANCE_DISTRIBUTION previous_performance_distribution_distribution_ = nullptr;
	ULONG number_of_processors_processors_{};
	PPROCESSOR_POWER_INFORMATION power_information_information_{};
	SYSTEM_BASIC_INFORMATION ph_system_basic_information_information_{};
	HMODULE            nt_dll_handle_{};
	double current_cpu_frequency_ = 0;
	static constexpr int k_time_out_times_ = 1;

	static NTSTATUS QueryProcessorPerformanceDistribution(
		_Out_ PVOID *out_buffer
		)
	{
		NTSTATUS status = 0;
		PVOID buffer = nullptr;
		ULONG bufferSize = 0;
		ULONG attempts = 0;

		bufferSize = 0x100;
		buffer = malloc(bufferSize);

		status = NtQuerySystemInformation(
			SystemProcessorPerformanceDistribution,
			buffer,
			bufferSize,
			&bufferSize
			);
		attempts = 0;

		while (status == STATUS_INFO_LENGTH_MISMATCH && attempts < 8)
		{
			free(buffer);
#pragma warning(suppress: 6102)
			buffer = malloc(bufferSize);

			status = NtQuerySystemInformation(
				SystemProcessorPerformanceDistribution,
				buffer,
				bufferSize,
				&bufferSize
				);
			attempts++;
		}

		if (NT_SUCCESS(status))
			*out_buffer = buffer;
		else
			free(buffer);

		return status;
	}

	void query_frequency()
	{
		if (!NT_SUCCESS(NtPowerInformation(
			ProcessorInformation,
			NULL,
			0,
			power_information_information_,
			sizeof(PROCESSOR_POWER_INFORMATION) * number_of_processors_processors_
			)))
		{
			SecureZeroMemory(power_information_information_, sizeof(PROCESSOR_POWER_INFORMATION) * number_of_processors_processors_);
		}

		if (previous_performance_distribution_distribution_)
			free(previous_performance_distribution_distribution_);

		previous_performance_distribution_distribution_ = current_performance_distribution_distribution_;
		current_performance_distribution_distribution_ = nullptr;
		QueryProcessorPerformanceDistribution(reinterpret_cast<PVOID*>(&current_performance_distribution_distribution_));
	}


	void update_frequency()
	{
		double cpuFraction = 0;
		double cpuGhz = 0;
		bool distributionSucceeded = false;
		query_frequency();
		if (current_performance_distribution_distribution_ && previous_performance_distribution_distribution_)
		{
			if (GetCpuFrequencyFromDistribution(&cpuFraction))
			{
				cpuGhz = static_cast<DOUBLE>(power_information_information_[0].MaxMhz) * cpuFraction / 1000;
				distributionSucceeded = true;
			}
		}

		if (!distributionSucceeded)
			cpuGhz = static_cast<DOUBLE>(power_information_information_[0].CurrentMhz) / 1000;

		current_cpu_frequency_ = cpuGhz;
	}

	bool init()
	{
		__try
		{
			nt_dll_handle_ = LoadLibraryEx(L"NtDll.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
			if (nt_dll_handle_ == nullptr)
			{
				return false;
			}

			NtQuerySystemInformation = reinterpret_cast<NTQUERYSYSTEMINFORMATION>(GetProcAddress(nt_dll_handle_,
				"NtQuerySystemInformation"));
			if (NtQuerySystemInformation == nullptr)
			{
				return false;
			}

			NtPowerInformation = reinterpret_cast<NTPOWERINFORMATION>(GetProcAddress(nt_dll_handle_,
				"NtPowerInformation"));
			if (NtPowerInformation == nullptr)
			{
				return false;
			}
		}
#pragma warning(suppress: 6312)
		__except (EXCEPTION_CONTINUE_EXECUTION)
		{
			return 0;
		}

		if (!NT_SUCCESS(NtQuerySystemInformation(
			SystemBasicInformation,
			&ph_system_basic_information_information_,
			sizeof(SYSTEM_BASIC_INFORMATION),
			NULL
		))) {
			DBG_PRINTF_FL((L""));
			return false;
		}

		number_of_processors_processors_ = static_cast<ULONG>(ph_system_basic_information_information_.number_of_processors_processors_);
		power_information_information_ = static_cast<PPROCESSOR_POWER_INFORMATION>(
			malloc(sizeof(PROCESSOR_POWER_INFORMATION) * number_of_processors_processors_));

		if (power_information_information_ == nullptr)
			return false;

		query_frequency();
		return true;

	}

#pragma warning(suppress: 6101)
	BOOLEAN GetCpuFrequencyFromDistribution(
		_Out_ DOUBLE *Fraction
	)
	{
		ULONG stateSize = 0;
		PVOID differences = 0;
		PSYSTEM_PROCESSOR_PERFORMANCE_STATE_DISTRIBUTION stateDistribution = nullptr;
		PSYSTEM_PROCESSOR_PERFORMANCE_STATE_DISTRIBUTION stateDifference = nullptr;
		ULONG i = 0;
		ULONG j = 0;
		DOUBLE count = 0.0;
		DOUBLE total = 0.0;

		// Calculate the differences from the last performance distribution.
		if (Fraction == nullptr)
		{
			return FALSE;
		}

		if (current_performance_distribution_distribution_->ProcessorCount != number_of_processors_processors_ ||
			previous_performance_distribution_distribution_->ProcessorCount != number_of_processors_processors_)
			return FALSE;

		stateSize = FIELD_OFFSET(SYSTEM_PROCESSOR_PERFORMANCE_STATE_DISTRIBUTION, States) + sizeof(SYSTEM_PROCESSOR_PERFORMANCE_HITCOUNT) * 2;
		differences = malloc(stateSize * number_of_processors_processors_);

		for (i = 0; i < number_of_processors_processors_; i++)
		{
			stateDistribution = reinterpret_cast<PSYSTEM_PROCESSOR_PERFORMANCE_STATE_DISTRIBUTION>(reinterpret_cast<PCHAR>(
					current_performance_distribution_distribution_) +
				current_performance_distribution_distribution_->Offsets[i]);
			stateDifference = reinterpret_cast<PSYSTEM_PROCESSOR_PERFORMANCE_STATE_DISTRIBUTION>(static_cast<PCHAR>(differences) + stateSize * i);

			if (stateDistribution->StateCount != 2)
			{
				free(differences);
				return FALSE;
			}

			for (j = 0; j < stateDistribution->StateCount; j++)
			{
				if (stateDifference)
					stateDifference->States[j] = stateDistribution->States[j];
			}
		}

		for (i = 0; i < number_of_processors_processors_; i++)
		{
			stateDistribution = reinterpret_cast<PSYSTEM_PROCESSOR_PERFORMANCE_STATE_DISTRIBUTION>(reinterpret_cast<PCHAR>(
					previous_performance_distribution_distribution_) +
				previous_performance_distribution_distribution_->Offsets[i]);
			stateDifference = reinterpret_cast<PSYSTEM_PROCESSOR_PERFORMANCE_STATE_DISTRIBUTION>(static_cast<PCHAR>(differences) + stateSize * i);

			if (stateDistribution->StateCount != 2)
			{
				free(differences);
				return FALSE;
			}

			for (j = 0; j < stateDistribution->StateCount; j++)
			{

				if (stateDifference)
					stateDifference->States[j].Hits.QuadPart -= stateDistribution->States[j].Hits.QuadPart;
			}
		}

		// Calculate the frequency.

		count = 0;
		total = 0;

		for (i = 0; i < number_of_processors_processors_; i++)
		{
			stateDifference = reinterpret_cast<PSYSTEM_PROCESSOR_PERFORMANCE_STATE_DISTRIBUTION>(static_cast<PCHAR>(differences) + stateSize * i);

			if (stateDifference)
			{
				for (j = 0; j < 2; j++)
				{
					count += static_cast<ULONGLONG>(stateDifference->States[j].Hits.QuadPart);
					total += static_cast<ULONGLONG>(stateDifference->States[j].Hits.QuadPart) * stateDifference->States[j].PercentFrequency;
				}
			}
		}

		free(differences);

		if (count == 0)
			return FALSE;

		total /= count;
		total /= 100;
		*Fraction = total;

		return TRUE;
	}
};


