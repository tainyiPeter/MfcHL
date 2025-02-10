// Copyright (C) 2021-present Lenovo. All right reserved.
// Confidential and Proprietary

#pragma once
#include "windows.h"
#pragma warning(disable:4005)
#pragma warning(disable:4311)
#pragma warning(disable:4302)
#pragma warning(disable:4312)
#pragma warning(disable:4459)
#include "winnt.h"
#include "ntstatus.h"
#include <vector>
#include "DebugUtil.h"
#include "wintrust.h"
#include <map>
#include "wininfo.h"
using namespace std;
//#pragma comment(lib,"ntdll.lib")

BOOL NTSUCCESS(NTSTATUS status)
{
	return NT_SUCCESS(status);
}

typedef _Success_(return >= 0) LONG NTSTATUS;
typedef NTSTATUS *PNTSTATUS;

typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaximumLength;
	ULONG	  Reserve;
	PWCH	 Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _STRING32
{
	USHORT Length;
	USHORT MaximumLength;
	ULONG Buffer;
} STRING32, *PSTRING32;

typedef STRING32 UNICODE_STRING32, *PUNICODE_STRING32;
// NT status macros

typedef LONG(WINAPI *PROCNTQSIP)(HANDLE, UINT, PVOID, ULONG, PULONG);

#define WINDOWS_ANCIENT 0
#define WINDOWS_XP 51
#define WINDOWS_SERVER_2003 52
#define WINDOWS_VISTA 60
#define WINDOWS_7 61
#define WINDOWS_8 62
#define WINDOWS_8_1 63
#define WINDOWS_10 100
#define WINDOWS_NEW MAXLONG

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#define NT_INFORMATION(Status) ((((ULONG)(Status)) >> 30) == 1)
#define NT_WARNING(Status) ((((ULONG)(Status)) >> 30) == 2)
#define NT_ERROR(Status) ((((ULONG)(Status)) >> 30) == 3)

#define NT_FACILITY_MASK 0xfff
#define NT_FACILITY_SHIFT 16
#define NT_FACILITY(Status) ((((ULONG)(Status)) >> NT_FACILITY_SHIFT) & NT_FACILITY_MASK)

#define NT_NTWIN32(Status) (NT_FACILITY(Status) == FACILITY_NTWIN32)
#define WIN32_FROM_NTSTATUS(Status) (((ULONG)(Status)) & 0xffff)

#define WOW64_POINTER(Type) ULONG

#define GDI_HANDLE_BUFFER_SIZE32 34
#define GDI_HANDLE_BUFFER_SIZE64 60

#ifndef WIN64
#define GDI_HANDLE_BUFFER_SIZE GDI_HANDLE_BUFFER_SIZE32
#else
#define GDI_HANDLE_BUFFER_SIZE GDI_HANDLE_BUFFER_SIZE64
#endif

typedef ULONG GDI_HANDLE_BUFFER[GDI_HANDLE_BUFFER_SIZE];

typedef ULONG GDI_HANDLE_BUFFER32[GDI_HANDLE_BUFFER_SIZE32];
typedef ULONG GDI_HANDLE_BUFFER64[GDI_HANDLE_BUFFER_SIZE64];


#define PTR_ADD_OFFSET(Pointer, Offset) ((PVOID)((ULONG_PTR)(Pointer) + (ULONG_PTR)(Offset)))

typedef struct _RTL_BALANCED_NODE32
{
	union
	{
		WOW64_POINTER(struct _RTL_BALANCED_NODE *) Children[2];
		struct
		{
			WOW64_POINTER(struct _RTL_BALANCED_NODE *) Left;
			WOW64_POINTER(struct _RTL_BALANCED_NODE *) Right;
		};
	};
	union
	{
		WOW64_POINTER(UCHAR) Red : 1;
		WOW64_POINTER(UCHAR) Balance : 2;
		WOW64_POINTER(ULONG_PTR) ParentValue;
	};
} RTL_BALANCED_NODE32, *PRTL_BALANCED_NODE32;

// symbols
typedef enum _LDR_DLL_LOAD_REASON
{
	LoadReasonStaticDependency,
	LoadReasonStaticForwarderDependency,
	LoadReasonDynamicForwarderDependency,
	LoadReasonDelayloadDependency,
	LoadReasonDynamicLoad,
	LoadReasonAsImageLoad,
	LoadReasonAsDataLoad,
	LoadReasonUnknown = -1
} LDR_DLL_LOAD_REASON, *PLDR_DLL_LOAD_REASON;

typedef struct _LDR_DATA_TABLE_ENTRY32
{
	LIST_ENTRY32 InLoadOrderLinks;
	LIST_ENTRY32 InMemoryOrderLinks;
	union
	{
		LIST_ENTRY32 InInitializationOrderLinks;
		LIST_ENTRY32 InProgressLinks;
	};
	WOW64_POINTER(PVOID) DllBase;
	WOW64_POINTER(PVOID) EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING32 FullDllName;
	UNICODE_STRING32 BaseDllName;
	union
	{
		UCHAR FlagGroup[4];
		ULONG Flags;
		struct
		{
			ULONG PackagedBinary : 1;
			ULONG MarkedForRemoval : 1;
			ULONG ImageDll : 1;
			ULONG LoadNotificationsSent : 1;
			ULONG TelemetryEntryProcessed : 1;
			ULONG ProcessStaticImport : 1;
			ULONG InLegacyLists : 1;
			ULONG InIndexes : 1;
			ULONG ShimDll : 1;
			ULONG InExceptionTable : 1;
			ULONG ReservedFlags1 : 2;
			ULONG LoadInProgress : 1;
			ULONG LoadConfigProcessed : 1;
			ULONG EntryProcessed : 1;
			ULONG ProtectDelayLoad : 1;
			ULONG ReservedFlags3 : 2;
			ULONG DontCallForThreads : 1;
			ULONG ProcessAttachCalled : 1;
			ULONG ProcessAttachFailed : 1;
			ULONG CorDeferredValidate : 1;
			ULONG CorImage : 1;
			ULONG DontRelocate : 1;
			ULONG CorILOnly : 1;
			ULONG ReservedFlags5 : 3;
			ULONG Redirected : 1;
			ULONG ReservedFlags6 : 2;
			ULONG CompatDatabaseProcessed : 1;
		};
	};
	USHORT ObsoleteLoadCount;
	USHORT TlsIndex;
	LIST_ENTRY32 HashLinks;
	ULONG TimeDateStamp;
	WOW64_POINTER(struct _ACTIVATION_CONTEXT *) EntryPointActivationContext;
	WOW64_POINTER(PVOID) Lock;
	WOW64_POINTER(PLDR_DDAG_NODE) DdagNode;
	LIST_ENTRY32 NodeModuleLink;
	WOW64_POINTER(struct _LDRP_LOAD_CONTEXT *) LoadContext;
	WOW64_POINTER(PVOID) ParentDllBase;
	WOW64_POINTER(PVOID) SwitchBackContext;
	RTL_BALANCED_NODE32 BaseAddressIndexNode;
	RTL_BALANCED_NODE32 MappingInfoIndexNode;
	WOW64_POINTER(ULONG_PTR) OriginalBase;
	LARGE_INTEGER LoadTime;
	ULONG BaseNameHashValue;
	LDR_DLL_LOAD_REASON LoadReason;
	ULONG ImplicitPathOptions;
	ULONG ReferenceCount;
} LDR_DATA_TABLE_ENTRY32, *PLDR_DATA_TABLE_ENTRY32;

// symbols
typedef struct _LDR_SERVICE_TAG_RECORD
{
	struct _LDR_SERVICE_TAG_RECORD *Next;
	ULONG ServiceTag;
} LDR_SERVICE_TAG_RECORD, *PLDR_SERVICE_TAG_RECORD;

// symbols
typedef struct _LDRP_CSLIST
{
	PSINGLE_LIST_ENTRY Tail;
} LDRP_CSLIST, *PLDRP_CSLIST;


// symbols
typedef enum _LDR_DDAG_STATE
{
	LdrModulesMerged = -5,
	LdrModulesInitError = -4,
	LdrModulesSnapError = -3,
	LdrModulesUnloaded = -2,
	LdrModulesUnloading = -1,
	LdrModulesPlaceHolder = 0,
	LdrModulesMapping = 1,
	LdrModulesMapped = 2,
	LdrModulesWaitingForDependencies = 3,
	LdrModulesSnapping = 4,
	LdrModulesSnapped = 5,
	LdrModulesCondensed = 6,
	LdrModulesReadyToInit = 7,
	LdrModulesInitializing = 8,
	LdrModulesReadyToRun = 9
} LDR_DDAG_STATE;


// symbols
typedef struct _LDR_DDAG_NODE
{
	LIST_ENTRY Modules;
	PLDR_SERVICE_TAG_RECORD ServiceTagList;
	ULONG LoadCount;
	ULONG ReferenceCount;
	ULONG DependencyCount;
	union
	{
		LDRP_CSLIST Dependencies;
		SINGLE_LIST_ENTRY RemovalLink;
	};
	LDRP_CSLIST IncomingDependencies;
	LDR_DDAG_STATE State;
	SINGLE_LIST_ENTRY CondenseLink;
	ULONG PreorderNumber;
	ULONG LowestLink;
} LDR_DDAG_NODE, *PLDR_DDAG_NODE;

typedef struct _RTL_BALANCED_NODE
{
	union
	{
		struct _RTL_BALANCED_NODE *Children[2];
		struct
		{
			struct _RTL_BALANCED_NODE *Left;
			struct _RTL_BALANCED_NODE *Right;
		};
	};
	union
	{
		UCHAR Red : 1;
		UCHAR Balance : 2;
		ULONG_PTR ParentValue;
	};
} RTL_BALANCED_NODE, *PRTL_BALANCED_NODE;


// symbols
typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	union
	{
		LIST_ENTRY InInitializationOrderLinks;
		LIST_ENTRY InProgressLinks;
	};
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	union
	{
		UCHAR FlagGroup[4];
		ULONG Flags;
		struct
		{
			ULONG PackagedBinary : 1;
			ULONG MarkedForRemoval : 1;
			ULONG ImageDll : 1;
			ULONG LoadNotificationsSent : 1;
			ULONG TelemetryEntryProcessed : 1;
			ULONG ProcessStaticImport : 1;
			ULONG InLegacyLists : 1;
			ULONG InIndexes : 1;
			ULONG ShimDll : 1;
			ULONG InExceptionTable : 1;
			ULONG ReservedFlags1 : 2;
			ULONG LoadInProgress : 1;
			ULONG LoadConfigProcessed : 1;
			ULONG EntryProcessed : 1;
			ULONG ProtectDelayLoad : 1;
			ULONG ReservedFlags3 : 2;
			ULONG DontCallForThreads : 1;
			ULONG ProcessAttachCalled : 1;
			ULONG ProcessAttachFailed : 1;
			ULONG CorDeferredValidate : 1;
			ULONG CorImage : 1;
			ULONG DontRelocate : 1;
			ULONG CorILOnly : 1;
			ULONG ReservedFlags5 : 3;
			ULONG Redirected : 1;
			ULONG ReservedFlags6 : 2;
			ULONG CompatDatabaseProcessed : 1;
		};
	};
	USHORT ObsoleteLoadCount;
	USHORT TlsIndex;
	LIST_ENTRY HashLinks;
	ULONG TimeDateStamp;
	struct _ACTIVATION_CONTEXT *EntryPointActivationContext;
	PVOID Lock;
	PLDR_DDAG_NODE DdagNode;
	LIST_ENTRY NodeModuleLink;
	struct _LDRP_LOAD_CONTEXT *LoadContext;
	PVOID ParentDllBase;
	PVOID SwitchBackContext;
	RTL_BALANCED_NODE BaseAddressIndexNode;
	RTL_BALANCED_NODE MappingInfoIndexNode;
	ULONG_PTR OriginalBase;
	LARGE_INTEGER LoadTime;
	ULONG BaseNameHashValue;
	LDR_DLL_LOAD_REASON LoadReason;
	ULONG ImplicitPathOptions;
	ULONG ReferenceCount;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef struct _PEB_LDR_DATA32
{
	ULONG Length;
	BOOLEAN Initialized;
	WOW64_POINTER(HANDLE) SsHandle;
	LIST_ENTRY32 InLoadOrderModuleList;
	LIST_ENTRY32 InMemoryOrderModuleList;
	LIST_ENTRY32 InInitializationOrderModuleList;
	WOW64_POINTER(PVOID) EntryInProgress;
	BOOLEAN ShutdownInProgress;
	WOW64_POINTER(HANDLE) ShutdownThreadId;
} PEB_LDR_DATA32, *PPEB_LDR_DATA32;





typedef struct _PEB32
{
	BOOLEAN InheritedAddressSpace;
	BOOLEAN ReadImageFileExecOptions;
	BOOLEAN BeingDebugged;
	union
	{
		BOOLEAN BitField;
		struct
		{
			BOOLEAN ImageUsesLargePages : 1;
			BOOLEAN IsProtectedProcess : 1;
			BOOLEAN IsLegacyProcess : 1;
			BOOLEAN IsImageDynamicallyRelocated : 1;
			BOOLEAN SkipPatchingUser32Forwarders : 1;
			BOOLEAN IsPackagedProcess : 1;
			BOOLEAN IsAppContainer : 1;
			BOOLEAN SpareBits : 1;
		};
	};
	WOW64_POINTER(HANDLE) Mutant;

	WOW64_POINTER(PVOID) ImageBaseAddress;
	WOW64_POINTER(PPEB_LDR_DATA) Ldr;
	WOW64_POINTER(PRTL_USER_PROCESS_PARAMETERS) ProcessParameters;
	WOW64_POINTER(PVOID) SubSystemData;
	WOW64_POINTER(PVOID) ProcessHeap;
	WOW64_POINTER(PRTL_CRITICAL_SECTION) FastPebLock;
	WOW64_POINTER(PVOID) AtlThunkSListPtr;
	WOW64_POINTER(PVOID) IFEOKey;
	union
	{
		ULONG CrossProcessFlags;
		struct
		{
			ULONG ProcessInJob : 1;
			ULONG ProcessInitializing : 1;
			ULONG ProcessUsingVEH : 1;
			ULONG ProcessUsingVCH : 1;
			ULONG ProcessUsingFTH : 1;
			ULONG ReservedBits0 : 27;
		};
		ULONG EnvironmentUpdateCount;
	};
	union
	{
		WOW64_POINTER(PVOID) KernelCallbackTable;
		WOW64_POINTER(PVOID) UserSharedInfoPtr;
	};
	ULONG SystemReserved[1];
	ULONG AtlThunkSListPtr32;
	WOW64_POINTER(PVOID) ApiSetMap;
	ULONG TlsExpansionCounter;
	WOW64_POINTER(PVOID) TlsBitmap;
	ULONG TlsBitmapBits[2];
	WOW64_POINTER(PVOID) ReadOnlySharedMemoryBase;
	WOW64_POINTER(PVOID) HotpatchInformation;
	WOW64_POINTER(PVOID *) ReadOnlyStaticServerData;
	WOW64_POINTER(PVOID) AnsiCodePageData;
	WOW64_POINTER(PVOID) OemCodePageData;
	WOW64_POINTER(PVOID) UnicodeCaseTableData;

	ULONG NumberOfProcessors;
	ULONG NtGlobalFlag;

	LARGE_INTEGER CriticalSectionTimeout;
	WOW64_POINTER(SIZE_T) HeapSegmentReserve;
	WOW64_POINTER(SIZE_T) HeapSegmentCommit;
	WOW64_POINTER(SIZE_T) HeapDeCommitTotalFreeThreshold;
	WOW64_POINTER(SIZE_T) HeapDeCommitFreeBlockThreshold;

	ULONG NumberOfHeaps;
	ULONG MaximumNumberOfHeaps;
	WOW64_POINTER(PVOID *) ProcessHeaps;

	WOW64_POINTER(PVOID) GdiSharedHandleTable;
	WOW64_POINTER(PVOID) ProcessStarterHelper;
	ULONG GdiDCAttributeList;

	WOW64_POINTER(PRTL_CRITICAL_SECTION) LoaderLock;

	ULONG OSMajorVersion;
	ULONG OSMinorVersion;
	USHORT OSBuildNumber;
	USHORT OSCSDVersion;
	ULONG OSPlatformId;
	ULONG ImageSubsystem;
	ULONG ImageSubsystemMajorVersion;
	ULONG ImageSubsystemMinorVersion;
	WOW64_POINTER(ULONG_PTR) ImageProcessAffinityMask;
	GDI_HANDLE_BUFFER32 GdiHandleBuffer;
	WOW64_POINTER(PVOID) PostProcessInitRoutine;

	WOW64_POINTER(PVOID) TlsExpansionBitmap;
	ULONG TlsExpansionBitmapBits[32];

	ULONG SessionId;

	ULARGE_INTEGER AppCompatFlags;
	ULARGE_INTEGER AppCompatFlagsUser;
	WOW64_POINTER(PVOID) pShimData;
	WOW64_POINTER(PVOID) AppCompatInfo;

	UNICODE_STRING32 CSDVersion;

	WOW64_POINTER(PVOID) ActivationContextData;
	WOW64_POINTER(PVOID) ProcessAssemblyStorageMap;
	WOW64_POINTER(PVOID) SystemDefaultActivationContextData;
	WOW64_POINTER(PVOID) SystemAssemblyStorageMap;

	WOW64_POINTER(SIZE_T) MinimumStackCommit;

	WOW64_POINTER(PVOID *) FlsCallback;
	LIST_ENTRY32 FlsListHead;
	WOW64_POINTER(PVOID) FlsBitmap;
	ULONG FlsBitmapBits[FLS_MAXIMUM_AVAILABLE / (sizeof(ULONG) * 8)];
	ULONG FlsHighIndex;

	WOW64_POINTER(PVOID) WerRegistrationData;
	WOW64_POINTER(PVOID) WerShipAssertPtr;
	WOW64_POINTER(PVOID) pContextData;
	WOW64_POINTER(PVOID) pImageHeaderHash;
	union
	{
		ULONG TracingFlags;
		struct
		{
			ULONG HeapTracingEnabled : 1;
			ULONG CritSecTracingEnabled : 1;
			ULONG LibLoaderTracingEnabled : 1;
			ULONG SpareTracingBits : 29;
		};
	};
	ULONGLONG CsrServerReadOnlySharedMemoryBase;
} PEB32, *PPEB32;

typedef enum _PROCESSINFOCLASS
{
	ProcessBasicInformation, // 0, q: PROCESS_BASIC_INFORMATION, PROCESS_EXTENDED_BASIC_INFORMATION
	ProcessQuotaLimits, // qs: QUOTA_LIMITS, QUOTA_LIMITS_EX
	ProcessIoCounters, // q: IO_COUNTERS
	ProcessVmCounters, // q: VM_COUNTERS, VM_COUNTERS_EX, VM_COUNTERS_EX2
	ProcessTimes, // q: KERNEL_USER_TIMES
	ProcessBasePriority, // s: KPRIORITY
	ProcessRaisePriority, // s: ULONG
	ProcessDebugPort, // q: HANDLE
	ProcessExceptionPort, // s: HANDLE
	ProcessAccessToken, // s: PROCESS_ACCESS_TOKEN
	ProcessLdtInformation, // 10, qs: PROCESS_LDT_INFORMATION
	ProcessLdtSize, // s: PROCESS_LDT_SIZE
	ProcessDefaultHardErrorMode, // qs: ULONG
	ProcessIoPortHandlers, // (kernel-mode only)
	ProcessPooledUsageAndLimits, // q: POOLED_USAGE_AND_LIMITS
	ProcessWorkingSetWatch, // q: PROCESS_WS_WATCH_INFORMATION[]; s: void
	ProcessUserModeIOPL,
	ProcessEnableAlignmentFaultFixup, // s: BOOLEAN
	ProcessPriorityClass, // qs: PROCESS_PRIORITY_CLASS
	ProcessWx86Information,
	ProcessHandleCount, // 20, q: ULONG, PROCESS_HANDLE_INFORMATION
	ProcessAffinityMask, // s: KAFFINITY
	ProcessPriorityBoost, // qs: ULONG
	ProcessDeviceMap, // qs: PROCESS_DEVICEMAP_INFORMATION, PROCESS_DEVICEMAP_INFORMATION_EX
	ProcessSessionInformation, // q: PROCESS_SESSION_INFORMATION
	ProcessForegroundInformation, // s: PROCESS_FOREGROUND_BACKGROUND
	ProcessWow64Information, // q: ULONG_PTR
	ProcessImageFileName, // q: UNICODE_STRING
	ProcessLUIDDeviceMapsEnabled, // q: ULONG
	ProcessBreakOnTermination, // qs: ULONG
	ProcessDebugObjectHandle, // 30, q: HANDLE
	ProcessDebugFlags, // qs: ULONG
	ProcessHandleTracing, // q: PROCESS_HANDLE_TRACING_QUERY; s: size 0 disables, otherwise enables
	ProcessIoPriority, // qs: ULONG
	ProcessExecuteFlags, // qs: ULONG
	ProcessResourceManagement,
	ProcessCookie, // q: ULONG
	ProcessImageInformation, // q: SECTION_IMAGE_INFORMATION
	ProcessCycleTime, // q: PROCESS_CYCLE_TIME_INFORMATION // since VISTA
	ProcessPagePriority, // q: ULONG
	ProcessInstrumentationCallback, // 40
	ProcessThreadStackAllocation, // s: PROCESS_STACK_ALLOCATION_INFORMATION, PROCESS_STACK_ALLOCATION_INFORMATION_EX
	ProcessWorkingSetWatchEx, // q: PROCESS_WS_WATCH_INFORMATION_EX[]
	ProcessImageFileNameWin32, // q: UNICODE_STRING
	ProcessImageFileMapping, // q: HANDLE (input)
	ProcessAffinityUpdateMode, // qs: PROCESS_AFFINITY_UPDATE_MODE
	ProcessMemoryAllocationMode, // qs: PROCESS_MEMORY_ALLOCATION_MODE
	ProcessGroupInformation, // q: USHORT[]
	ProcessTokenVirtualizationEnabled, // s: ULONG
	ProcessConsoleHostProcess, // q: ULONG_PTR
	ProcessWindowInformation, // 50, q: PROCESS_WINDOW_INFORMATION
	ProcessHandleInformation, // q: PROCESS_HANDLE_SNAPSHOT_INFORMATION // since WIN8
	ProcessMitigationPolicy, // s: PROCESS_MITIGATION_POLICY_INFORMATION
	ProcessDynamicFunctionTableInformation,
	ProcessHandleCheckingMode,
	ProcessKeepAliveCount, // q: PROCESS_KEEPALIVE_COUNT_INFORMATION
	ProcessRevokeFileHandles, // s: PROCESS_REVOKE_FILE_HANDLES_INFORMATION
	ProcessWorkingSetControl, // s: PROCESS_WORKING_SET_CONTROL
	ProcessHandleTable, // since WINBLUE
	ProcessCheckStackExtentsMode,
	ProcessCommandLineInformation, // 60, q: UNICODE_STRING
	ProcessProtectionInformation, // q: PS_PROTECTION
	ProcessMemoryExhaustion, // PROCESS_MEMORY_EXHAUSTION_INFO // since THRESHOLD
	ProcessFaultInformation, // PROCESS_FAULT_INFORMATION
	ProcessTelemetryIdInformation, // PROCESS_TELEMETRY_ID_INFORMATION
	ProcessCommitReleaseInformation, // PROCESS_COMMIT_RELEASE_INFORMATION
	ProcessDefaultCpuSetsInformation,
	ProcessAllowedCpuSetsInformation,
	ProcessReserved1Information,
	ProcessReserved2Information,
	ProcessSubsystemProcess, // 70
	ProcessJobMemoryInformation, // PROCESS_JOB_MEMORY_INFO
	MaxProcessInfoClass
} PROCESSINFOCLASS;




typedef struct _STRING
{
	USHORT Length;
	USHORT MaximumLength;
	_Field_size_bytes_part_opt_(MaximumLength, Length) PCHAR Buffer;
} STRING, *PSTRING, ANSI_STRING, *PANSI_STRING, OEM_STRING, *POEM_STRING;

// symbols
typedef struct _PEB_LDR_DATA
{
	ULONG Length;
	BOOLEAN Initialized;
	HANDLE SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID EntryInProgress;
	BOOLEAN ShutdownInProgress;
	HANDLE ShutdownThreadId;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _CURDIR
{
	UNICODE_STRING DosPath;
	HANDLE Handle;
} CURDIR, *PCURDIR;

typedef struct _RTL_DRIVE_LETTER_CURDIR
{
	USHORT Flags;
	USHORT Length;
	ULONG TimeStamp;
	STRING DosPath;
} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;

#define RTL_MAX_DRIVE_LETTERS 32
#define RTL_DRIVE_LETTER_VALID (USHORT)0x0001


typedef struct _RTL_USER_PROCESS_PARAMETERS
{
	ULONG MaximumLength;
	ULONG Length;

	ULONG Flags;
	ULONG DebugFlags;

	HANDLE ConsoleHandle;
	ULONG ConsoleFlags;
	HANDLE StandardInput;
	HANDLE StandardOutput;
	HANDLE StandardError;

	CURDIR CurrentDirectory;
	UNICODE_STRING DllPath;
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
	PVOID Environment;

	ULONG StartingX;
	ULONG StartingY;
	ULONG CountX;
	ULONG CountY;
	ULONG CountCharsX;
	ULONG CountCharsY;
	ULONG FillAttribute;

	ULONG WindowFlags;
	ULONG ShowWindowFlags;
	UNICODE_STRING WindowTitle;
	UNICODE_STRING DesktopInfo;
	UNICODE_STRING ShellInfo;
	UNICODE_STRING RuntimeData;
	RTL_DRIVE_LETTER_CURDIR CurrentDirectories[RTL_MAX_DRIVE_LETTERS];

	ULONG EnvironmentSize;
	ULONG EnvironmentVersion;
	PVOID PackageDependencyData;
	ULONG ProcessGroupId;
	ULONG LoaderThreads;
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

// symbols
typedef struct _PEB
{
	BOOLEAN InheritedAddressSpace;
	BOOLEAN ReadImageFileExecOptions;
	BOOLEAN BeingDebugged;
	union
	{
		BOOLEAN BitField;
		struct
		{
			BOOLEAN ImageUsesLargePages : 1;
			BOOLEAN IsProtectedProcess : 1;
			BOOLEAN IsImageDynamicallyRelocated : 1;
			BOOLEAN SkipPatchingUser32Forwarders : 1;
			BOOLEAN IsPackagedProcess : 1;
			BOOLEAN IsAppContainer : 1;
			BOOLEAN IsProtectedProcessLight : 1;
			BOOLEAN SpareBits : 1;
		};
	};
	HANDLE Mutant;

	PVOID ImageBaseAddress;
	PPEB_LDR_DATA Ldr;
	PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
	PVOID SubSystemData;
	PVOID ProcessHeap;
	PRTL_CRITICAL_SECTION FastPebLock;
	PVOID AtlThunkSListPtr;
	PVOID IFEOKey;
	union
	{
		ULONG CrossProcessFlags;
		struct
		{
			ULONG ProcessInJob : 1;
			ULONG ProcessInitializing : 1;
			ULONG ProcessUsingVEH : 1;
			ULONG ProcessUsingVCH : 1;
			ULONG ProcessUsingFTH : 1;
			ULONG ReservedBits0 : 27;
		};
		ULONG EnvironmentUpdateCount;
	};
	union
	{
		PVOID KernelCallbackTable;
		PVOID UserSharedInfoPtr;
	};
	ULONG SystemReserved[1];
	ULONG AtlThunkSListPtr32;
	PVOID ApiSetMap;
	ULONG TlsExpansionCounter;
	PVOID TlsBitmap;
	ULONG TlsBitmapBits[2];
	PVOID ReadOnlySharedMemoryBase;
	PVOID HotpatchInformation;
	PVOID *ReadOnlyStaticServerData;
	PVOID AnsiCodePageData;
	PVOID OemCodePageData;
	PVOID UnicodeCaseTableData;

	ULONG NumberOfProcessors;
	ULONG NtGlobalFlag;

	LARGE_INTEGER CriticalSectionTimeout;
	SIZE_T HeapSegmentReserve;
	SIZE_T HeapSegmentCommit;
	SIZE_T HeapDeCommitTotalFreeThreshold;
	SIZE_T HeapDeCommitFreeBlockThreshold;

	ULONG NumberOfHeaps;
	ULONG MaximumNumberOfHeaps;
	PVOID *ProcessHeaps;

	PVOID GdiSharedHandleTable;
	PVOID ProcessStarterHelper;
	ULONG GdiDCAttributeList;

	PRTL_CRITICAL_SECTION LoaderLock;

	ULONG OSMajorVersion;
	ULONG OSMinorVersion;
	USHORT OSBuildNumber;
	USHORT OSCSDVersion;
	ULONG OSPlatformId;
	ULONG ImageSubsystem;
	ULONG ImageSubsystemMajorVersion;
	ULONG ImageSubsystemMinorVersion;
	ULONG_PTR ImageProcessAffinityMask;
	GDI_HANDLE_BUFFER GdiHandleBuffer;
	PVOID PostProcessInitRoutine;

	PVOID TlsExpansionBitmap;
	ULONG TlsExpansionBitmapBits[32];

	ULONG SessionId;

	ULARGE_INTEGER AppCompatFlags;
	ULARGE_INTEGER AppCompatFlagsUser;
	PVOID pShimData;
	PVOID AppCompatInfo;

	UNICODE_STRING CSDVersion;

	PVOID ActivationContextData;
	PVOID ProcessAssemblyStorageMap;
	PVOID SystemDefaultActivationContextData;
	PVOID SystemAssemblyStorageMap;

	SIZE_T MinimumStackCommit;

	PVOID *FlsCallback;
	LIST_ENTRY FlsListHead;
	PVOID FlsBitmap;
	ULONG FlsBitmapBits[FLS_MAXIMUM_AVAILABLE / (sizeof(ULONG) * 8)];
	ULONG FlsHighIndex;

	PVOID WerRegistrationData;
	PVOID WerShipAssertPtr;
	PVOID pContextData;
	PVOID pImageHeaderHash;
	union
	{
		ULONG TracingFlags;
		struct
		{
			ULONG HeapTracingEnabled : 1;
			ULONG CritSecTracingEnabled : 1;
			ULONG LibLoaderTracingEnabled : 1;
			ULONG SpareTracingBits : 29;
		};
	};
	ULONGLONG CsrServerReadOnlySharedMemoryBase;
} PEB, *PPEB;

typedef LONG KPRIORITY;

typedef struct _PROCESS_BASIC_INFORMATION
{
	NTSTATUS ExitStatus;
	PPEB PebBaseAddress;
	ULONG_PTR AffinityMask;
	KPRIORITY BasePriority;
	HANDLE UniqueProcessId;
	HANDLE InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;


// Use the size of the structure as it was in Windows XP.
#define LDR_DATA_TABLE_ENTRY_SIZE_WINXP FIELD_OFFSET(LDR_DATA_TABLE_ENTRY, DdagNode)
#define LDR_DATA_TABLE_ENTRY_SIZE_WIN7 FIELD_OFFSET(LDR_DATA_TABLE_ENTRY, BaseNameHashValue)
#define LDR_DATA_TABLE_ENTRY_SIZE_WIN8 FIELD_OFFSET(LDR_DATA_TABLE_ENTRY, ImplicitPathOptions)

#define LDR_DATA_TABLE_ENTRY_SIZE_WINXP_32 FIELD_OFFSET(LDR_DATA_TABLE_ENTRY32, DdagNode)
#define LDR_DATA_TABLE_ENTRY_SIZE_WIN7_32 FIELD_OFFSET(LDR_DATA_TABLE_ENTRY32, BaseNameHashValue)
#define LDR_DATA_TABLE_ENTRY_SIZE_WIN8_32 FIELD_OFFSET(LDR_DATA_TABLE_ENTRY32, ImplicitPathOptions)

#define PH_ENUM_PROCESS_MODULES_LIMIT 0x800

typedef struct _PH_STRINGREF
{
	/** The length, in bytes, of the string. */
	SIZE_T Length;
	/** The buffer containing the contents of the string. */
	PWCH Buffer;
} PH_STRINGREF, *PPH_STRINGREF;

typedef
NTSTATUS
(NTAPI *_NtQueryInformationProcess)(
	_In_ HANDLE ProcessHandle,
	_In_ PROCESSINFOCLASS ProcessInformationClass,
	_Out_writes_bytes_(ProcessInformationLength) PVOID ProcessInformation,
	_In_ ULONG ProcessInformationLength,
	_Out_opt_ PULONG ReturnLength
	);

_NtQueryInformationProcess NtQueryInformationProcess;

typedef
NTSTATUS
(NTAPI *_NtReadVirtualMemory)(
	_In_ HANDLE ProcessHandle,
	_In_opt_ PVOID BaseAddress,
	_Out_writes_bytes_(BufferSize) PVOID Buffer,
	_In_ SIZE_T BufferSize,
	_Out_opt_ PSIZE_T NumberOfBytesRead
	);

_NtReadVirtualMemory NtReadVirtualMemory;

typedef struct _OBJECT_ATTRIBUTES
{
	ULONG Length;
	HANDLE RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG Attributes;
	PVOID SecurityDescriptor; // PSECURITY_DESCRIPTOR;
	PVOID SecurityQualityOfService; // PSECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;


typedef struct _CLIENT_ID
{
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef
NTSTATUS
(NTAPI* _NtOpenProcess)(
_Out_ PHANDLE ProcessHandle,
_In_ ACCESS_MASK DesiredAccess,
_In_ POBJECT_ATTRIBUTES ObjectAttributes,
_In_opt_ PCLIENT_ID ClientId
);

_NtOpenProcess NtOpenProcess;

// private
typedef enum _MEMORY_INFORMATION_CLASS
{
	MemoryBasicInformation, // MEMORY_BASIC_INFORMATION
	MemoryWorkingSetInformation, // MEMORY_WORKING_SET_INFORMATION
	MemoryMappedFilenameInformation, // UNICODE_STRING
	MemoryRegionInformation, // MEMORY_REGION_INFORMATION
	MemoryWorkingSetExInformation, // MEMORY_WORKING_SET_EX_INFORMATION
	MemorySharedCommitInformation // MEMORY_SHARED_COMMIT_INFORMATION
} MEMORY_INFORMATION_CLASS;


typedef
NTSTATUS
(NTAPI* _NtQueryVirtualMemory)(
_In_ HANDLE ProcessHandle,
_In_ PVOID BaseAddress,
_In_ MEMORY_INFORMATION_CLASS MemoryInformationClass,
_Out_writes_bytes_(MemoryInformationLength) PVOID MemoryInformation,
_In_ SIZE_T MemoryInformationLength,
_Out_opt_ PSIZE_T ReturnLength
);

_NtQueryVirtualMemory NtQueryVirtualMemory;

NTSTATUS QueryVirtualMemory(
	_In_ INT_PTR ProcessHandle,
	_In_ PVOID BaseAddress,
	_In_ MEMORY_INFORMATION_CLASS MemoryInformationClass,
	_Out_writes_bytes_(MemoryInformationLength) PVOID MemoryInformation,
	_In_ SIZE_T MemoryInformationLength,
	_Out_opt_ PSIZE_T ReturnLength)
{
	NTSTATUS statusQVM = NtQueryVirtualMemory(
		(HANDLE)ProcessHandle,
		BaseAddress,
		MemoryBasicInformation,
		MemoryInformation,
		MemoryInformationLength,
		ReturnLength);

	return statusQVM;
}

#define InitializeObjectAttributes(p, n, a, r, s) { \
    (p)->Length = sizeof(OBJECT_ATTRIBUTES); \
    (p)->RootDirectory = r; \
    (p)->Attributes = a; \
    (p)->ObjectName = n; \
    (p)->SecurityDescriptor = s; \
    (p)->SecurityQualityOfService = NULL; \
    }

class CProcessInfo
{
public:

	CProcessInfo()
	{
		CWindowsInfo().GetWindowsVersion(WindowsVersion);
	}

	~CProcessInfo()
	{
	}

	ULONG *GetWindowsVersionAddress()
	{
		return &WindowsVersion;
	}

private:
	ULONG WindowsVersion;
	NTSTATUS
	GetProcessIsWow64(
			_In_ HANDLE ProcessHandle,
			_Out_ PBOOLEAN IsWow64
			)
	{
		NTSTATUS status;
		ULONG_PTR wow64;

		status = NtQueryInformationProcess(
			ProcessHandle,
			ProcessWow64Information,
			&wow64,
			sizeof(ULONG_PTR),
			NULL
			);

		if (NT_SUCCESS(status))
		{
			*IsWow64 = !!wow64;
		}

		return status;
	}

	NTSTATUS ReadVirtualMemory(
		_In_ HANDLE ProcessHandle,
		_In_ PVOID BaseAddress,
		_Out_writes_bytes_(BufferSize) PVOID Buffer,
		_In_ SIZE_T BufferSize,
		_Out_opt_ PSIZE_T NumberOfBytesRead
		)
	{
		NTSTATUS status;

		// KphReadVirtualMemory is much slower than
		// NtReadVirtualMemory, so we'll stick to
		// the using the original system call whenever possible.

		status = NtReadVirtualMemory(
			ProcessHandle,
			BaseAddress,
			Buffer,
			BufferSize,
			NumberOfBytesRead
			);

	
		return status;
	}

	NTSTATUS
	GetProcessPeb32(
			_In_ HANDLE ProcessHandle,
			_Out_ PVOID *Peb32
			)
	{
		NTSTATUS status;
		ULONG_PTR wow64;

		status = NtQueryInformationProcess(
			ProcessHandle,
			ProcessWow64Information,
			&wow64,
			sizeof(ULONG_PTR),
			NULL
			);

		if (NT_SUCCESS(status))
		{
			*Peb32 = (PVOID)wow64;
		}

		return status;
	}

	VOID
		PhUnicodeStringToStringRef(
			_In_ PUNICODE_STRING UnicodeString,
			_Out_ PPH_STRINGREF String
			)
	{
		String->Length = UnicodeString->Length;
		String->Buffer = UnicodeString->Buffer;
	}

#pragma warning(suppress: 6101)
	NTSTATUS PhpEnumProcessModules32(
		_In_ HANDLE ProcessHandle,
		_Out_ map<wstring, wstring> &modulelist)
	{
		NTSTATUS status;
		PPEB32 peb;
		ULONG ldr; // PEB_LDR_DATA32 *32
		PEB_LDR_DATA32 pebLdrData;
		ULONG startLink; // LIST_ENTRY32 *32
		ULONG currentLink; // LIST_ENTRY32 *32
		ULONG dataTableEntrySize;
		LDR_DATA_TABLE_ENTRY32 currentEntry;
		ULONG i;

		// Get the 32-bit PEB address.
		status = GetProcessPeb32(ProcessHandle, (PVOID*)&peb);
		if (!NTSUCCESS(status))
		{
			DBG_PRINTF_FL((L"Get ProcessPeb32 failed! error 0x%x\n", status));
			return status;
		}
		if (!peb)
		{
			DBG_PRINTF_FL((L"Not a WOW64 process\n"));
			return STATUS_NOT_SUPPORTED; // not a WOW64 process
		}

										 // Read the address of the loader data.
		status = ReadVirtualMemory(
			ProcessHandle,
			PTR_ADD_OFFSET(peb, FIELD_OFFSET(PEB32, Ldr)),
			&ldr,
			sizeof(ULONG),
			NULL
			);

		if (!NT_SUCCESS(status))
		{
			DBG_PRINTF_FL((L"ReadVirtualMemory PEB32 failed! error 0x%x\n", status));
			return status;
		}

		// Read the loader data.
		status = ReadVirtualMemory(
			ProcessHandle,
			UlongToPtr(ldr),
			&pebLdrData,
			sizeof(PEB_LDR_DATA32),
			NULL
			);

		if (!NT_SUCCESS(status))
		{
			DBG_PRINTF_FL((L"ReadVirtualMemory failed! error 0x%x\n", status));
			return status;
		}

		if (!pebLdrData.Initialized)
		{
			DBG_PRINTF_FL((L"pebLdrData is not initialized\n"));
			return STATUS_UNSUCCESSFUL;
		}

		if (WindowsVersion >= WINDOWS_8)
			dataTableEntrySize = LDR_DATA_TABLE_ENTRY_SIZE_WIN8_32;
		else if (WindowsVersion >= WINDOWS_7)
			dataTableEntrySize = LDR_DATA_TABLE_ENTRY_SIZE_WIN7_32;
		else
			dataTableEntrySize = LDR_DATA_TABLE_ENTRY_SIZE_WINXP_32;

		// Traverse the linked list (in load order).

		i = 0;
		startLink = (ULONG)(ldr + FIELD_OFFSET(PEB_LDR_DATA32, InLoadOrderModuleList));
		currentLink = pebLdrData.InLoadOrderModuleList.Flink;

		while (
			currentLink != startLink &&
			i <= PH_ENUM_PROCESS_MODULES_LIMIT
			)
		{
			ULONG addressOfEntry;

			addressOfEntry = (ULONG)CONTAINING_RECORD(UlongToPtr(currentLink), LDR_DATA_TABLE_ENTRY32, InLoadOrderLinks);
			status = ReadVirtualMemory(
				ProcessHandle,
				UlongToPtr(addressOfEntry),
				&currentEntry,
				dataTableEntrySize,
				NULL
				);

			if (!NT_SUCCESS(status))
				return status;

			// Make sure the entry is valid.
			if (currentEntry.DllBase)
			{
				//printf("module name %S\n", currentEntry.BaseDllName.Buffer);
				WCHAR* wdllname = new WCHAR[currentEntry.BaseDllName.MaximumLength];

				SecureZeroMemory(wdllname, currentEntry.BaseDllName.MaximumLength);
				// Execute the callback.
				status = ReadVirtualMemory(ProcessHandle,
					(PVOID)currentEntry.BaseDllName.Buffer,
					wdllname,
					currentEntry.BaseDllName.Length,
					NULL);

				WCHAR* wfullname = new WCHAR[currentEntry.FullDllName.MaximumLength];

				SecureZeroMemory(wfullname, currentEntry.FullDllName.MaximumLength);
				// Execute the callback.
				status = ReadVirtualMemory(ProcessHandle,
					(PVOID)currentEntry.FullDllName.Buffer,
					wfullname,
					currentEntry.FullDllName.Length,
					NULL);

				if (NT_SUCCESS(status))
				{
					wstring lowdllname = TransformCase(wstring(wdllname));
					if (wfullname)
					{
						modulelist.insert(make_pair(lowdllname, wfullname));
						DBG_PRINTF_FL((L"module dll name %s, full name %s\n", wdllname, wfullname));
					}
					else
					{
						modulelist.insert(make_pair(lowdllname, L""));
						DBG_PRINTF_FL((L"module dll name %s\n", wdllname));
					}
				}
				if (wdllname)
					delete[]wdllname;
				if (wfullname)
					delete[]wfullname;
			}

			currentLink = currentEntry.InLoadOrderLinks.Flink;
			i++;
		}

		return status;
	}

	NTSTATUS
	GetProcessBasicInformation(
			_In_ HANDLE ProcessHandle,
			_Out_ PPROCESS_BASIC_INFORMATION BasicInformation
			)
	{
		return NtQueryInformationProcess(
			ProcessHandle,
			ProcessBasicInformation,
			BasicInformation,
			sizeof(PROCESS_BASIC_INFORMATION),
			NULL
			);
	}


#pragma warning(suppress: 6101)
	NTSTATUS PhpEnumProcessModules(
		_In_ HANDLE ProcessHandle,
		_Out_ map<wstring, wstring> &modulelist)
	{
		NTSTATUS status;
		PROCESS_BASIC_INFORMATION basicInfo;
		PVOID ldr;
		PEB_LDR_DATA pebLdrData;
		PLIST_ENTRY startLink;
		PLIST_ENTRY currentLink;
		ULONG dataTableEntrySize;
		LDR_DATA_TABLE_ENTRY currentEntry;
		ULONG i;

		// Get the PEB address.
		status = GetProcessBasicInformation(ProcessHandle, &basicInfo);

		if (!NT_SUCCESS(status))
			return status;

		// Read the address of the loader data.
		status = ReadVirtualMemory(
			ProcessHandle,
			PTR_ADD_OFFSET(basicInfo.PebBaseAddress, FIELD_OFFSET(PEB, Ldr)),
			&ldr,
			sizeof(PVOID),
			NULL
			);

		if (!NT_SUCCESS(status))
			return status;

		// Read the loader data.
		status = ReadVirtualMemory(
			ProcessHandle,
			ldr,
			&pebLdrData,
			sizeof(PEB_LDR_DATA),
			NULL
			);

		if (!NT_SUCCESS(status))
			return status;

		if (!pebLdrData.Initialized)
			return STATUS_UNSUCCESSFUL;

		if (WindowsVersion >= WINDOWS_8)
			dataTableEntrySize = LDR_DATA_TABLE_ENTRY_SIZE_WIN8;
		else if (WindowsVersion >= WINDOWS_7)
			dataTableEntrySize = LDR_DATA_TABLE_ENTRY_SIZE_WIN7;
		else
			dataTableEntrySize = LDR_DATA_TABLE_ENTRY_SIZE_WINXP;

		// Traverse the linked list (in load order).

		i = 0;
		startLink = (PLIST_ENTRY)PTR_ADD_OFFSET(ldr, FIELD_OFFSET(PEB_LDR_DATA, InLoadOrderModuleList));
		currentLink = pebLdrData.InLoadOrderModuleList.Flink;

		while (
			currentLink != startLink &&
			i <= PH_ENUM_PROCESS_MODULES_LIMIT
			)
		{
			PVOID addressOfEntry;

			addressOfEntry = CONTAINING_RECORD(currentLink, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
			status = ReadVirtualMemory(
				ProcessHandle,
				addressOfEntry,
				&currentEntry,
				dataTableEntrySize,
				NULL
				);

			if (!NT_SUCCESS(status))
				return status;

			PH_STRINGREF String;
			// Make sure the entry is valid.
			if (currentEntry.DllBase)
			{
				//printf("module name %S\n", currentEntry.BaseDllName.Buffer);
				WCHAR* wdllname = new WCHAR[currentEntry.BaseDllName.MaximumLength];

				SecureZeroMemory(wdllname, currentEntry.BaseDllName.MaximumLength);
				// Execute the callback.
				PhUnicodeStringToStringRef(&currentEntry.BaseDllName, &String);
				status = ReadVirtualMemory(ProcessHandle,
					currentEntry.BaseDllName.Buffer,
					wdllname,
					currentEntry.BaseDllName.Length,
					NULL);

				WCHAR* wfullname = new WCHAR[currentEntry.FullDllName.MaximumLength];

				SecureZeroMemory(wfullname, currentEntry.FullDllName.MaximumLength);
				// Execute the callback.
				PhUnicodeStringToStringRef(&currentEntry.FullDllName, &String);
				status = ReadVirtualMemory(ProcessHandle,
					currentEntry.FullDllName.Buffer,
					wfullname,
					currentEntry.FullDllName.Length,
					NULL);


				if (NT_SUCCESS(status))
				{					
					wstring lowdllname = TransformCase(wstring(wdllname));
					if (wfullname)
					{
						modulelist.insert(make_pair(lowdllname, wfullname));
						DBG_PRINTF_FL((L"module dll name %s, full name %s\n", wdllname, wfullname));
					}
					else
					{
						modulelist.insert(make_pair(lowdllname, L""));
						DBG_PRINTF_FL((L"module dll name %s\n", wdllname));
					}
				}
				if (wdllname)
					delete[]wdllname;
				if (wfullname)
					delete[]wfullname;
			}

			currentLink = currentEntry.InLoadOrderLinks.Flink;
			i++;
		}

		return status;
	}

	
#define PH_MODULE_TYPE_MODULE 1
#define PH_MODULE_TYPE_MAPPED_FILE 2
#define PH_MODULE_TYPE_WOW64_MODULE 3
#define PH_MODULE_TYPE_KERNEL_MODULE 4
#define PH_MODULE_TYPE_MAPPED_IMAGE 5

#define PH_ENUM_GENERIC_MAPPED_FILES 0x1
#define PH_ENUM_GENERIC_MAPPED_IMAGES 0x2


	NTSTATUS PhGetProcessMappedFileName(
		_In_ HANDLE ProcessHandle,
		_In_ PVOID BaseAddress,
		_Out_ wstring *FileName
		)
	{
		NTSTATUS status = STATUS_NO_MEMORY;
		PVOID buffer;
		SIZE_T bufferSize;
		SIZE_T returnLength;
		PUNICODE_STRING unicodeString;

		bufferSize = 0x100;
		buffer = malloc(bufferSize);
		if (NULL == buffer)
			return status;
		status = NtQueryVirtualMemory(
			ProcessHandle,
			BaseAddress,
			MemoryMappedFilenameInformation,
			buffer,
			bufferSize,
			&returnLength
			);

		if (status == STATUS_BUFFER_OVERFLOW)
		{
			free(buffer);
#pragma warning(suppress: 6102)
			bufferSize = returnLength;
			buffer = malloc(bufferSize);
			if (NULL == buffer)
				return status;
			status = NtQueryVirtualMemory(
				ProcessHandle,
				BaseAddress,
				MemoryMappedFilenameInformation,
				buffer,
				bufferSize,
				&returnLength
				);
		}

		if (!NT_SUCCESS(status))
		{
			free(buffer);
			return status;
		}

		unicodeString = (PUNICODE_STRING)buffer;
		*FileName = unicodeString->Buffer;
		
		free(buffer);

		return status;
	}


	VOID EnumGenericMappedFilesAndImages(
		_In_ HANDLE ProcessHandle,
		_In_ ULONG Flags,
		map<wstring, wstring> &modulelist)
	{
		BOOLEAN querySucceeded;
		PVOID baseAddress;
		MEMORY_BASIC_INFORMATION basicInfo;
		map<PVOID, PVOID> baseaddrlist;

		baseaddrlist.clear();
		baseAddress = (PVOID)0;

#pragma warning(suppress: 6387)
		NTSTATUS ntstate =  NtQueryVirtualMemory(
			ProcessHandle,
			baseAddress,
			MemoryBasicInformation,
			&basicInfo,
			sizeof(MEMORY_BASIC_INFORMATION),
			NULL
			);

		if (!NTSUCCESS(ntstate))
		{
			DBG_PRINTF_FL((L"NtQueryVirtualMemory failed!!! status 0x%x\n", ntstate));
			return;
		}

		querySucceeded = TRUE;

		while (querySucceeded)
		{
			PVOID allocationBase;
			SIZE_T allocationSize;
			ULONG type;
			wstring fileName;

			if (basicInfo.Type == MEM_MAPPED || basicInfo.Type == MEM_IMAGE)
			{
				if (basicInfo.Type == MEM_MAPPED)
					type = PH_MODULE_TYPE_MAPPED_FILE;
				else
					type = PH_MODULE_TYPE_MAPPED_IMAGE;

				// Find the total allocation size.

				allocationBase = basicInfo.AllocationBase;
				allocationSize = 0;

				do
				{
					baseAddress = (PVOID)((ULONG_PTR)baseAddress + basicInfo.RegionSize);
					allocationSize += basicInfo.RegionSize;

					NTSTATUS statusQVM = QueryVirtualMemory(
						(INT_PTR)ProcessHandle,
						baseAddress,
						MemoryBasicInformation,
						&basicInfo,
						sizeof(MEMORY_BASIC_INFORMATION),
						NULL);

					if (!NT_SUCCESS(statusQVM))
					{
						querySucceeded = FALSE;
						DBG_PRINTF_FL((L"NtQueryVirtualMemory -2 failed!!!\n"));
						break;
					}
				} while (basicInfo.AllocationBase == allocationBase);

				if ((type == PH_MODULE_TYPE_MAPPED_FILE && !(Flags & PH_ENUM_GENERIC_MAPPED_FILES)) ||
					(type == PH_MODULE_TYPE_MAPPED_IMAGE && !(Flags & PH_ENUM_GENERIC_MAPPED_IMAGES)))
				{
					// The user doesn't want this type of entry.
					continue;
				}

				// Check if we have a duplicate base address.
				{
					map<PVOID, PVOID>::iterator it = baseaddrlist.find(allocationBase);
					if (it != baseaddrlist.end())
					{
						DBG_PRINTF_FL((L"Already in the list!!!\n"));
						continue;
					}
				}

				if (!NT_SUCCESS(PhGetProcessMappedFileName(
					ProcessHandle,
					allocationBase,
					&fileName
					)))
				{
					DBG_PRINTF_FL((L"PhGetProcessMappedFileName failed!!!\n"));
					continue;
				}

				//add to map list
				{
					baseaddrlist.insert(make_pair(allocationBase, allocationBase));
				}

				int nPos = CString(fileName.c_str()).ReverseFind(_T('\\'));
				wstring dllname = static_cast<LPCTSTR>(CString(fileName.c_str()).Right(CString(fileName.c_str()).GetLength() - 1 - nPos).MakeLower());
				modulelist.insert(make_pair(dllname, fileName));

				DBG_PRINTF_FL((L"Module full name %s, dll name %s\n", fileName.c_str(), dllname.c_str()));

			}
			else
			{
				baseAddress = (PVOID)((ULONG_PTR)baseAddress + basicInfo.RegionSize);

				if (!NT_SUCCESS(NtQueryVirtualMemory(
					ProcessHandle,
					baseAddress,
					MemoryBasicInformation,
					&basicInfo,
					sizeof(MEMORY_BASIC_INFORMATION),
					NULL
					)))
				{
					querySucceeded = FALSE;
				}
			}
		}
	}


public:

	BOOL GetCurrentProcessList(map<ULONG, wstring> &pidlist)
	{
		PROCESSENTRY32 proc_entry;
		HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snap == INVALID_HANDLE_VALUE) {
			return FALSE;
		}
		SecureZeroMemory(&proc_entry, sizeof(proc_entry));
		proc_entry.dwSize = sizeof(PROCESSENTRY32);
		if (!Process32First(snap, &proc_entry)) {
			CloseHandle(snap);
			return FALSE;
		}
		do
		{
			pidlist.insert(make_pair(proc_entry.th32ProcessID, proc_entry.szExeFile));

		} while (Process32Next(snap, &proc_entry));


		CloseHandle(snap);
		return TRUE;
	}

	BOOL GetProcessModuleList(ULONG pid, map<wstring, wstring> &modulelist)
	{
		NTSTATUS status = STATUS_UNSUCCESSFUL;
		HANDLE phandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, pid);
		if (phandle)
		{
			HMODULE            hNtDll = NULL;
			__try
			{
				hNtDll = LoadLibraryEx(L"NtDll.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
				if (hNtDll == NULL)
				{
					DBG_PRINTF_FL((L"Load Ntdll failed!\n"));
					__leave;
				}

				NtOpenProcess = (_NtOpenProcess)GetProcAddress(hNtDll,
					"NtOpenProcess");
				if (NtOpenProcess == NULL)
				{
					DBG_PRINTF_FL((L"Get NtOpenProcess failed!\n"));
					__leave;
				}

				NtQueryInformationProcess = (_NtQueryInformationProcess)GetProcAddress(hNtDll,
					"NtQueryInformationProcess");
				if (NtQueryInformationProcess == NULL)
				{
					DBG_PRINTF_FL((L"Get NtQueryInformationProcess failed!\n"));
					__leave;
				}

				NtQueryVirtualMemory = (_NtQueryVirtualMemory)GetProcAddress(hNtDll,
					"NtQueryVirtualMemory");
				if (NtQueryVirtualMemory == NULL)
				{
					DBG_PRINTF_FL((L"Get NtQueryVirtualMemory failed!\n"));
					__leave;
				}

				

				NtReadVirtualMemory = (_NtReadVirtualMemory)GetProcAddress(hNtDll,
					"NtReadVirtualMemory");
				if (NtReadVirtualMemory == NULL)
				{
					DBG_PRINTF_FL((L"Get NtReadVirtualMemory failed!\n"));
					__leave;
				}
			}
#pragma warning(suppress: 6312)
			__except (EXCEPTION_CONTINUE_EXECUTION)
			{
				return FALSE;
			}

			if (nullptr == NtOpenProcess
				|| nullptr == NtQueryInformationProcess
				|| nullptr == NtQueryVirtualMemory
				|| nullptr == NtReadVirtualMemory)
			{
				CloseHandle(phandle);
				FreeLibrary(hNtDll);
				return FALSE;
			}

#ifdef _WIN64
			BOOLEAN isWow64 = FALSE;
#endif


#ifdef _WIN64
			status = GetProcessIsWow64(phandle, &isWow64);
			if (!NTSUCCESS(status))
				return FALSE;
			DBG_PRINTF_FL((L"Process %d is %d bit\n", pid, isWow64?32:64));
			if (isWow64)
				status = PhpEnumProcessModules32(phandle, modulelist);
			else
#endif
				status = PhpEnumProcessModules(phandle,modulelist);



			// Mapped files and mapped images
			// This is done last because it provides the least amount of information.

			EnumGenericMappedFilesAndImages(
				phandle,
				PH_ENUM_GENERIC_MAPPED_FILES | PH_ENUM_GENERIC_MAPPED_IMAGES,
				modulelist);
			


			CloseHandle(phandle);
#pragma warning(suppress: 6387)
			FreeLibrary(hNtDll);

		}
		else
			DBG_PRINTF_FL((L"Open process %d failed! - enum lasterror %d\n", pid,GetLastError()));
		return (status == STATUS_SUCCESS) ? TRUE:FALSE;
	}


	typedef LONG(WINAPI *PROCNTQSIP)(HANDLE, UINT, PVOID, ULONG, PULONG);

	DWORD GetParentProcessID(DWORD dwId)
	{
		LONG                      status;
		DWORD                     dwParentPID = (DWORD)-1;
		HANDLE                    hProcess;
		PROCESS_BASIC_INFORMATION pbi;

		PROCNTQSIP NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(
			GetModuleHandle(L"ntdll"), "NtQueryInformationProcess");
		if (NULL == NtQueryInformationProcess)
		{
			return (DWORD)-1;
		}

		// Get process handle   
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwId);
		if (!hProcess)
		{
			return (DWORD)-1;
		}

		// Retrieve information   
		status = NtQueryInformationProcess(hProcess,
			ProcessBasicInformation,
			(PVOID)&pbi,
			sizeof(PROCESS_BASIC_INFORMATION),
			NULL
			);

		// Copy parent Id on success   
		if (!status)
		{
			dwParentPID = (DWORD)pbi.InheritedFromUniqueProcessId;
		}
		CloseHandle(hProcess);

		return dwParentPID;
	}

private:


typedef LONG NTSTATUS;
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)


	typedef enum _SYSTEM_INFORMATION_CLASS
	{
		SystemBasicInformation, // q: SYSTEM_BASIC_INFORMATION
		SystemProcessorInformation, // q: SYSTEM_PROCESSOR_INFORMATION
		SystemPerformanceInformation, // q: SYSTEM_PERFORMANCE_INFORMATION
		SystemTimeOfDayInformation, // q: SYSTEM_TIMEOFDAY_INFORMATION
		SystemPathInformation, // not implemented
		SystemProcessInformation, // q: SYSTEM_PROCESS_INFORMATION
		SystemCallCountInformation, // q: SYSTEM_CALL_COUNT_INFORMATION
		SystemDeviceInformation, // q: SYSTEM_DEVICE_INFORMATION
		SystemProcessorPerformanceInformation, // q: SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION
		SystemFlagsInformation, // q: SYSTEM_FLAGS_INFORMATION
		SystemCallTimeInformation, // 10, not implemented
		SystemModuleInformation, // q: RTL_PROCESS_MODULES
		SystemLocksInformation,
		SystemStackTraceInformation,
		SystemPagedPoolInformation, // not implemented
		SystemNonPagedPoolInformation, // not implemented
		SystemHandleInformation, // q: SYSTEM_HANDLE_INFORMATION
		SystemObjectInformation, // q: SYSTEM_OBJECTTYPE_INFORMATION mixed with SYSTEM_OBJECT_INFORMATION
		SystemPageFileInformation, // q: SYSTEM_PAGEFILE_INFORMATION
		SystemVdmInstemulInformation, // q
		SystemVdmBopInformation, // 20, not implemented
		SystemFileCacheInformation, // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypeSystemCache)
		SystemPoolTagInformation, // q: SYSTEM_POOLTAG_INFORMATION
		SystemInterruptInformation, // q: SYSTEM_INTERRUPT_INFORMATION
		SystemDpcBehaviorInformation, // q: SYSTEM_DPC_BEHAVIOR_INFORMATION; s: SYSTEM_DPC_BEHAVIOR_INFORMATION (requires SeLoadDriverPrivilege)
		SystemFullMemoryInformation, // not implemented
		SystemLoadGdiDriverInformation, // s (kernel-mode only)
		SystemUnloadGdiDriverInformation, // s (kernel-mode only)
		SystemTimeAdjustmentInformation, // q: SYSTEM_QUERY_TIME_ADJUST_INFORMATION; s: SYSTEM_SET_TIME_ADJUST_INFORMATION (requires SeSystemtimePrivilege)
		SystemSummaryMemoryInformation, // not implemented
		SystemMirrorMemoryInformation, // 30, s (requires license value "Kernel-MemoryMirroringSupported") (requires SeShutdownPrivilege)
		SystemPerformanceTraceInformation, // s
		SystemObsolete0, // not implemented
		SystemExceptionInformation, // q: SYSTEM_EXCEPTION_INFORMATION
		SystemCrashDumpStateInformation, // s (requires SeDebugPrivilege)
		SystemKernelDebuggerInformation, // q: SYSTEM_KERNEL_DEBUGGER_INFORMATION
		SystemContextSwitchInformation, // q: SYSTEM_CONTEXT_SWITCH_INFORMATION
		SystemRegistryQuotaInformation, // q: SYSTEM_REGISTRY_QUOTA_INFORMATION; s (requires SeIncreaseQuotaPrivilege)
		SystemExtendServiceTableInformation, // s (requires SeLoadDriverPrivilege) // loads win32k only
		SystemPrioritySeperation, // s (requires SeTcbPrivilege)
		SystemVerifierAddDriverInformation, // 40, s (requires SeDebugPrivilege)
		SystemVerifierRemoveDriverInformation, // s (requires SeDebugPrivilege)
		SystemProcessorIdleInformation, // q: SYSTEM_PROCESSOR_IDLE_INFORMATION
		SystemLegacyDriverInformation, // q: SYSTEM_LEGACY_DRIVER_INFORMATION
		SystemCurrentTimeZoneInformation, // q
		SystemLookasideInformation, // q: SYSTEM_LOOKASIDE_INFORMATION
		SystemTimeSlipNotification, // s (requires SeSystemtimePrivilege)
		SystemSessionCreate, // not implemented
		SystemSessionDetach, // not implemented
		SystemSessionInformation, // not implemented
		SystemRangeStartInformation, // 50, q
		SystemVerifierInformation, // q: SYSTEM_VERIFIER_INFORMATION; s (requires SeDebugPrivilege)
		SystemVerifierThunkExtend, // s (kernel-mode only)
		SystemSessionProcessInformation, // q: SYSTEM_SESSION_PROCESS_INFORMATION
		SystemLoadGdiDriverInSystemSpace, // s (kernel-mode only) (same as SystemLoadGdiDriverInformation)
		SystemNumaProcessorMap, // q
		SystemPrefetcherInformation, // q: PREFETCHER_INFORMATION; s: PREFETCHER_INFORMATION // PfSnQueryPrefetcherInformation
		SystemExtendedProcessInformation, // q: SYSTEM_PROCESS_INFORMATION
		SystemRecommendedSharedDataAlignment, // q
		SystemComPlusPackage, // q; s
		SystemNumaAvailableMemory, // 60
		SystemProcessorPowerInformation, // q: SYSTEM_PROCESSOR_POWER_INFORMATION
		SystemEmulationBasicInformation, // q
		SystemEmulationProcessorInformation,
		SystemExtendedHandleInformation, // q: SYSTEM_HANDLE_INFORMATION_EX
		SystemLostDelayedWriteInformation, // q: ULONG
		SystemBigPoolInformation, // q: SYSTEM_BIGPOOL_INFORMATION
		SystemSessionPoolTagInformation, // q: SYSTEM_SESSION_POOLTAG_INFORMATION
		SystemSessionMappedViewInformation, // q: SYSTEM_SESSION_MAPPED_VIEW_INFORMATION
		SystemHotpatchInformation, // q; s
		SystemObjectSecurityMode, // 70, q
		SystemWatchdogTimerHandler, // s (kernel-mode only)
		SystemWatchdogTimerInformation, // q (kernel-mode only); s (kernel-mode only)
		SystemLogicalProcessorInformation, // q: SYSTEM_LOGICAL_PROCESSOR_INFORMATION
		SystemWow64SharedInformationObsolete, // not implemented
		SystemRegisterFirmwareTableInformationHandler, // s (kernel-mode only)
		SystemFirmwareTableInformation, // not implemented
		SystemModuleInformationEx, // q: RTL_PROCESS_MODULE_INFORMATION_EX
		SystemVerifierTriageInformation, // not implemented
		SystemSuperfetchInformation, // q: SUPERFETCH_INFORMATION; s: SUPERFETCH_INFORMATION // PfQuerySuperfetchInformation
		SystemMemoryListInformation, // 80, q: SYSTEM_MEMORY_LIST_INFORMATION; s: SYSTEM_MEMORY_LIST_COMMAND (requires SeProfileSingleProcessPrivilege)
		SystemFileCacheInformationEx, // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (same as SystemFileCacheInformation)
		SystemThreadPriorityClientIdInformation, // s: SYSTEM_THREAD_CID_PRIORITY_INFORMATION (requires SeIncreaseBasePriorityPrivilege)
		SystemProcessorIdleCycleTimeInformation, // q: SYSTEM_PROCESSOR_IDLE_CYCLE_TIME_INFORMATION[]
		SystemVerifierCancellationInformation, // not implemented // name:wow64:whNT32QuerySystemVerifierCancellationInformation
		SystemProcessorPowerInformationEx, // not implemented
		SystemRefTraceInformation, // q; s // ObQueryRefTraceInformation
		SystemSpecialPoolInformation, // q; s (requires SeDebugPrivilege) // MmSpecialPoolTag, then MmSpecialPoolCatchOverruns != 0
		SystemProcessIdInformation, // q: SYSTEM_PROCESS_ID_INFORMATION
		SystemErrorPortInformation, // s (requires SeTcbPrivilege)
		SystemBootEnvironmentInformation, // 90, q: SYSTEM_BOOT_ENVIRONMENT_INFORMATION
		SystemHypervisorInformation, // q; s (kernel-mode only)
		SystemVerifierInformationEx, // q; s
		SystemTimeZoneInformation, // s (requires SeTimeZonePrivilege)
		SystemImageFileExecutionOptionsInformation, // s: SYSTEM_IMAGE_FILE_EXECUTION_OPTIONS_INFORMATION (requires SeTcbPrivilege)
		SystemCoverageInformation, // q; s // name:wow64:whNT32QuerySystemCoverageInformation; ExpCovQueryInformation
		SystemPrefetchPatchInformation, // not implemented
		SystemVerifierFaultsInformation, // s (requires SeDebugPrivilege)
		SystemSystemPartitionInformation, // q: SYSTEM_SYSTEM_PARTITION_INFORMATION
		SystemSystemDiskInformation, // q: SYSTEM_SYSTEM_DISK_INFORMATION
		SystemProcessorPerformanceDistribution, // 100, q: SYSTEM_PROCESSOR_PERFORMANCE_DISTRIBUTION
		SystemNumaProximityNodeInformation, // q
		SystemDynamicTimeZoneInformation, // q; s (requires SeTimeZonePrivilege)
		SystemCodeIntegrityInformation, // q // SeCodeIntegrityQueryInformation
		SystemProcessorMicrocodeUpdateInformation, // s
		SystemProcessorBrandString, // q // HaliQuerySystemInformation -> HalpGetProcessorBrandString, info class 23
		SystemVirtualAddressInformation, // q: SYSTEM_VA_LIST_INFORMATION[]; s: SYSTEM_VA_LIST_INFORMATION[] (requires SeIncreaseQuotaPrivilege) // MmQuerySystemVaInformation
		SystemLogicalProcessorAndGroupInformation, // q: SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX // since WIN7 // KeQueryLogicalProcessorRelationship
		SystemProcessorCycleTimeInformation, // q: SYSTEM_PROCESSOR_CYCLE_TIME_INFORMATION[]
		SystemStoreInformation, // q; s // SmQueryStoreInformation
		SystemRegistryAppendString, // 110, s: SYSTEM_REGISTRY_APPEND_STRING_PARAMETERS
		SystemAitSamplingValue, // s: ULONG (requires SeProfileSingleProcessPrivilege)
		SystemVhdBootInformation, // q: SYSTEM_VHD_BOOT_INFORMATION
		SystemCpuQuotaInformation, // q; s // PsQueryCpuQuotaInformation
		SystemNativeBasicInformation, // not implemented
		SystemSpare1, // not implemented
		SystemLowPriorityIoInformation, // q: SYSTEM_LOW_PRIORITY_IO_INFORMATION
		SystemTpmBootEntropyInformation, // q: TPM_BOOT_ENTROPY_NT_RESULT // ExQueryTpmBootEntropyInformation
		SystemVerifierCountersInformation, // q: SYSTEM_VERIFIER_COUNTERS_INFORMATION
		SystemPagedPoolInformationEx, // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypePagedPool)
		SystemSystemPtesInformationEx, // 120, q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypeSystemPtes)
		SystemNodeDistanceInformation, // q
		SystemAcpiAuditInformation, // q: SYSTEM_ACPI_AUDIT_INFORMATION // HaliQuerySystemInformation -> HalpAuditQueryResults, info class 26
		SystemBasicPerformanceInformation, // q: SYSTEM_BASIC_PERFORMANCE_INFORMATION // name:wow64:whNtQuerySystemInformation_SystemBasicPerformanceInformation
		SystemQueryPerformanceCounterInformation, // q: SYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION // since WIN7 SP1
		SystemSessionBigPoolInformation, // since WIN8
		SystemBootGraphicsInformation,
		SystemScrubPhysicalMemoryInformation,
		SystemBadPageInformation,
		SystemProcessorProfileControlArea,
		SystemCombinePhysicalMemoryInformation, // 130
		SystemEntropyInterruptTimingCallback,
		SystemConsoleInformation,
		SystemPlatformBinaryInformation,
		SystemThrottleNotificationInformation,
		SystemHypervisorProcessorCountInformation,
		SystemDeviceDataInformation,
		SystemDeviceDataEnumerationInformation,
		SystemMemoryTopologyInformation,
		SystemMemoryChannelInformation,
		SystemBootLogoInformation, // 140
		SystemProcessorPerformanceInformationEx, // since WINBLUE
		SystemSpare0,
		SystemSecureBootPolicyInformation,
		SystemPageFileInformationEx,
		SystemSecureBootInformation,
		SystemEntropyInterruptTimingRawInformation,
		SystemPortableWorkspaceEfiLauncherInformation,
		SystemFullProcessInformation, // q: SYSTEM_PROCESS_INFORMATION with SYSTEM_PROCESS_INFORMATION_EXTENSION (requires admin)
		SystemKernelDebuggerInformationEx,
		SystemBootMetadataInformation, // 150
		SystemSoftRebootInformation,
		SystemElamCertificateInformation,
		SystemOfflineDumpConfigInformation,
		SystemProcessorFeaturesInformation,
		SystemRegistryReconciliationInformation,
		SystemEdidInformation,
		MaxSystemInfoClass
	} SYSTEM_INFORMATION_CLASS;


	typedef struct _IO_COUNTERSEX {
		LARGE_INTEGER ReadOperationCount;
		LARGE_INTEGER WriteOperationCount;
		LARGE_INTEGER OtherOperationCount;
		LARGE_INTEGER ReadTransferCount;
		LARGE_INTEGER WriteTransferCount;
		LARGE_INTEGER OtherTransferCount;
	} IO_COUNTERSEX, *PIO_COUNTERSEX;

	typedef enum {
		StateInitialized,
		StateReady,
		StateRunning,
		StateStandby,
		StateTerminated,
		StateWait,
		StateTransition,
		StateUnknown
	} THREAD_STATE;

	typedef struct _VM_COUNTERS {
		SIZE_T PeakVirtualSize;
		SIZE_T VirtualSize;
		ULONG PageFaultCount;
		SIZE_T PeakWorkingSetSize;
		SIZE_T WorkingSetSize;
		SIZE_T QuotaPeakPagedPoolUsage;
		SIZE_T QuotaPagedPoolUsage;
		SIZE_T QuotaPeakNonPagedPoolUsage;
		SIZE_T QuotaNonPagedPoolUsage;
		SIZE_T PagefileUsage;
		SIZE_T PeakPagefileUsage;
	} VM_COUNTERS;
	typedef VM_COUNTERS *PVM_COUNTERS;

	typedef struct _SYSTEM_THREADS {
		LARGE_INTEGER KernelTime;
		LARGE_INTEGER UserTime;
		LARGE_INTEGER CreateTime;
		ULONG WaitTime;
		PVOID StartAddress;
		CLIENT_ID ClientId;
		ULONG Priority;
		ULONG BasePriority;
		ULONG ContextSwitchCount;
		THREAD_STATE State;
		ULONG WaitReason;
	} SYSTEM_THREADS, *PSYSTEM_THREADS;

	typedef struct _SYSTEM_PROCESSES { // Information Class 5
		ULONG NextEntryDelta;
		ULONG ThreadCount;
		ULONG Reserved1[6];
		LARGE_INTEGER CreateTime;
		LARGE_INTEGER UserTime;
		LARGE_INTEGER KernelTime;
		UNICODE_STRING ProcessName;
		ULONG BasePriority;
		ULONG ProcessId;
		ULONG InheritedFromProcessId;
		ULONG HandleCount;
		ULONG Reserved2[2];
		VM_COUNTERS VmCounters;
		IO_COUNTERSEX IoCounters;  // Windows 2000 only
		SYSTEM_THREADS Threads[1];
	} SYSTEM_PROCESSES, *PSYSTEM_PROCESSES;

	typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO
	{
		ULONG		ProcessId;
		USHORT	Flags;
		USHORT	Handle;
		PVOID		Object;
		ULONG		GrantedAccess;
		ULONG		Reverse;
	} SYSTEM_HANDLE_TABLE_ENTRY_INFO, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

	typedef struct _SYSTEM_HANDLE_INFORMATION
	{
		ULONG_PTR NumberOfHandles;
		SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];
	} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;


	typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX
	{
		PVOID Object;
		ULONG_PTR UniqueProcessId;
		ULONG_PTR HandleValue;
		ULONG GrantedAccess;
		USHORT CreatorBackTraceIndex;
		USHORT ObjectTypeIndex;
		ULONG HandleAttributes;
		ULONG Reserved;
	} SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO_EX;

	typedef struct _SYSTEM_HANDLE_INFORMATION_EX
	{
		ULONG_PTR NumberOfHandles;
		ULONG_PTR Reserved;
		SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX Handles[1];
	} SYSTEM_HANDLE_INFORMATION_EX, *PSYSTEM_HANDLE_INFORMATION_EX;

	typedef enum _OBJECT_INFORMATION_CLASS {
		ObjectBasicInformation,
		ObjectNameInformation,
		ObjectTypeInformation,
		ObjectAllInformation,
		ObjectDataInformation
	} OBJECT_INFORMATION_CLASS;

	typedef struct _OBJECT_NAME_INFORMATION {
		UNICODE_STRING Name;
	} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

	typedef NTSTATUS(NTAPI *NTQUERYOBJECT)(
		_In_opt_   HANDLE Handle,
		_In_       OBJECT_INFORMATION_CLASS ObjectInformationClass,
		_Out_opt_  PVOID ObjectInformation,
		_In_       ULONG ObjectInformationLength,
		_Out_opt_  PULONG ReturnLength
		);




	typedef NTSTATUS
		(NTAPI *ZWQUERYSYSTEMINFORMATION)(
		IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
		OUT PVOID SystemInformation,
		IN ULONG SystemInformationLength,
		OUT PULONG ReturnLength OPTIONAL
		);



	typedef NTSTATUS(WINAPI *NTQUERYOBJECT)(
		_In_opt_ HANDLE Handle,
		_In_ OBJECT_INFORMATION_CLASS ObjectInformationClass,
		_Out_opt_ PVOID ObjectInformation,
		_In_ ULONG ObjectInformationLength,
		_Out_opt_ PULONG ReturnLength);

	// NTQUERYSYSTEMINFORMATION
	typedef struct _SYSTEM_HANDLE {
		DWORD dwProcessId;
		BYTE bObjectType;
		BYTE bFlags;
		WORD wValue;
		PVOID pAddress;
		DWORD GrantedAccess;
	} SYSTEM_HANDLE, *PSYSTEM_HANDLE;


	typedef struct _IO_STATUS_BLOCK {
		union {
			NTSTATUS Status;
			PVOID    Pointer;
		};
		ULONG_PTR Information;
	} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

	typedef NTSTATUS(WINAPI *NTQUERYSYSTEMINFORMATION)(
		IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
		OUT PVOID SystemInformation,
		IN ULONG SystemInformationLength,
		OUT PULONG ReturnLength OPTIONAL);

	//
	// NtQueryInformationFile
	//
	typedef enum _FILE_INFORMATION_CLASS {
		FileDirectoryInformation = 1,
		FileFullDirectoryInformation,
		FileBothDirectoryInformation,
		FileBasicInformation,
		FileStandardInformation,
		FileInternalInformation,
		FileEaInformation,
		FileAccessInformation,
		FileNameInformation,
		FileRenameInformation,
		FileLinkInformation,
		FileNamesInformation,
		FileDispositionInformation,
		FilePositionInformation,
		FileFullEaInformation,
		FileModeInformation,
		FileAlignmentInformation,
		FileAllInformation,
		FileAllocationInformation,
		FileEndOfFileInformation,
		FileAlternateNameInformation,
		FileStreamInformation,
		FilePipeInformation,
		FilePipeLocalInformation,
		FilePipeRemoteInformation,
		FileMailslotQueryInformation,
		FileMailslotSetInformation,
		FileCompressionInformation,
		FileObjectIdInformation,
		FileCompletionInformation,
		FileMoveClusterInformation,
		FileQuotaInformation,
		FileReparsePointInformation,
		FileNetworkOpenInformation,
		FileAttributeTagInformation,
		FileTrackingInformation,
		FileIdBothDirectoryInformation,
		FileIdFullDirectoryInformation,
		FileValidDataLengthInformation,
		FileShortNameInformation,
		FileIoCompletionNotificationInformation,
		FileIoStatusBlockRangeInformation,
		FileIoPriorityHintInformation,
		FileSfioReserveInformation,
		FileSfioVolumeInformation,
		FileHardLinkInformation,
		FileProcessIdsUsingFileInformation,
		FileNormalizedNameInformation,
		FileNetworkPhysicalNameInformation,
		FileIdGlobalTxDirectoryInformation,
		FileIsRemoteDeviceInformation,
		FileUnusedInformation,
		FileNumaNodeInformation,
		FileStandardLinkInformation,
		FileRemoteProtocolInformation,
		FileRenameInformationBypassAccessCheck,
		FileLinkInformationBypassAccessCheck,
		FileVolumeNameInformation,
		FileIdInformation,
		FileIdExtdDirectoryInformation,
		FileReplaceCompletionInformation,
		FileHardLinkFullIdInformation,
		FileIdExtdBothDirectoryInformation,
		FileMaximumInformation
	} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

	// typedef struct _FILE_NAME_INFORMATION {
	// 	ULONG FileNameLength;
	// 	WCHAR FileName[1];
	// } FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;

	typedef NTSTATUS(WINAPI *NTQUERYINFORMATIONFILE)(
		IN HANDLE FileHandle,
		OUT PIO_STATUS_BLOCK IoStatusBlock,
		OUT PVOID FileInformation,
		IN ULONG Length,
		IN FILE_INFORMATION_CLASS FileInformationClass);

	static void CheckBlockThreadFunc(void* param)
	{
		static NTQUERYINFORMATIONFILE fpNtQueryInformationFile =
			(NTQUERYINFORMATIONFILE)GetProcAddress(GetModuleHandle(_T("ntdll")), "NtQueryInformationFile");
		BYTE buf[1024] = {0};
		IO_STATUS_BLOCK ioStatus = {0};
		NTSTATUS status;
		if (fpNtQueryInformationFile != NULL)
		{
			status = fpNtQueryInformationFile((HANDLE)param, &ioStatus, buf, 1024, FileNameInformation);

		}
	}

	// IsBlockingHandle
	BOOL IsBlockingHandle(HANDLE handle)
	{
		HANDLE hThread = (HANDLE)_beginthread(CheckBlockThreadFunc, 0, (void*)handle);

		if (WaitForSingleObject(hThread, 100) != WAIT_TIMEOUT) {
			return FALSE;
		}

		TerminateThread(hThread, 0);
		return TRUE;
	}

	// GetDeviceDriveMap
	void GetDeviceDriveMap(std::map<wstring, wstring>& mapDeviceDrive)
	{
		TCHAR szDrives[512];
		if (!GetLogicalDriveStrings(_countof(szDrives) - 1, szDrives)) {
			return;
		}

		TCHAR* lpDrives = szDrives;
		TCHAR szDevice[MAX_PATH];
		TCHAR szDrive[3] = _T(" :");
		do {
			*szDrive = *lpDrives;

			if (QueryDosDevice(szDrive, szDevice, MAX_PATH)) {
				mapDeviceDrive[szDevice] = szDrive;
			}
			while (*lpDrives++);
		} while (*lpDrives);
	}

	// DevicePathToDrivePath
	BOOL DevicePathToDrivePath(wstring& path)
	{
		static std::map<wstring, wstring> mapDeviceDrive;

		if (mapDeviceDrive.empty()) {
			GetDeviceDriveMap(mapDeviceDrive);
		}

		for (std::map<wstring, wstring>::const_iterator it = mapDeviceDrive.begin(); it != mapDeviceDrive.end(); ++it) {
			size_t nLength = it->first.length();
			if (_tcsnicmp(it->first.c_str(), path.c_str(), nLength) == 0) {
				path.replace(0, nLength, it->second);
				return TRUE;
			}
		}

		return FALSE;
	}

public:
	void GetProcessFileHandleInfo(ULONG pid, vector<wstring> &itemlist)
	{
		DWORD dwSize = 0;
		PSYSTEM_HANDLE_TABLE_ENTRY_INFO pmodule = NULL;
		POBJECT_NAME_INFORMATION pNameInfo;
		POBJECT_NAME_INFORMATION pNameType;
		PVOID pbuffer = NULL;
		NTSTATUS Status;
		DWORD nIndex = 0;
		DWORD dwFlags = 0;
		int count = 0;
		HANDLE targetproc = NULL;

		ZWQUERYSYSTEMINFORMATION ZwQuerySystemInformation = (ZWQUERYSYSTEMINFORMATION)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "ZwQuerySystemInformation");
		NTQUERYOBJECT    NtQueryObject = (NTQUERYOBJECT)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQueryObject");

		if (!ZwQuerySystemInformation)
		{
			goto Exit0;
		}

		pbuffer = VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_READWRITE);

		if (!pbuffer)
		{
			goto Exit0;
		}

		Status = ZwQuerySystemInformation(SystemHandleInformation, pbuffer, 0x1000, &dwSize);

		if (!NT_SUCCESS(Status))
		{
			if (STATUS_INFO_LENGTH_MISMATCH != Status)
			{
				goto Exit0;
			}
			else
			{
				if (NULL != pbuffer)
				{
					VirtualFree(pbuffer, 0, MEM_RELEASE);
					pbuffer = NULL;
				}

				if (dwSize * 2 > 0x4000000)  // MAXSIZE
				{
					goto Exit0;
				}

				pbuffer = VirtualAlloc(NULL, dwSize * 2, MEM_COMMIT, PAGE_READWRITE);

				if (!pbuffer)
				{
					goto Exit0;
				}

				Status = ZwQuerySystemInformation(SystemHandleInformation, pbuffer, dwSize * 2, NULL);

				if (!NT_SUCCESS(Status))
				{
					goto Exit0;
				}
			}
		}

		pmodule = ((PSYSTEM_HANDLE_INFORMATION)pbuffer)->Handles;
		dwSize = *((PULONG)pbuffer);


		targetproc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		if (targetproc == NULL)
			goto Exit0;
		for (nIndex = 0; nIndex < dwSize; nIndex++)
		{
			char szType[128] = { 0 };
			char szName[512] = { 0 };
			wstring wstrfilename;
			WCHAR typebuffer[10] = { 0 };

			dwFlags = 0;
			WCHAR filenamebuffer[MAX_PATH * 2] = { 0 };
			if (pmodule[nIndex].ProcessId != pid)
				continue;

			HANDLE newhandle = NULL;
			if (FALSE == DuplicateHandle(targetproc, (HANDLE)pmodule[nIndex].Handle, GetCurrentProcess(), &newhandle, DUPLICATE_SAME_ACCESS, FALSE, DUPLICATE_SAME_ACCESS))
				continue;
			if (newhandle == NULL)
				goto Clean;



			Status = NtQueryObject((HANDLE)newhandle, ObjectTypeInformation, szType, 128, &dwFlags);

			if (!NT_SUCCESS(Status))
			{
				goto Clean;
			}
			pNameType = (POBJECT_NAME_INFORMATION)szType;
			if (pNameType->Name.Length < 8)
				goto Clean;
			memcpy_s(typebuffer, 8, pNameType->Name.Buffer, 8);
			
			if (wcscmp(typebuffer, L"File") != 0)
				goto Clean;

			//in case some name pipe handle with block in get filename with NtQueryObject
			if (IsBlockingHandle((HANDLE)newhandle))
				goto Clean;

			Status = NtQueryObject((HANDLE)newhandle, ObjectNameInformation, szName, 512, &dwFlags);

			if (!NT_SUCCESS(Status))
			{
				goto Clean;
			}

			pNameInfo = (POBJECT_NAME_INFORMATION)szName;


			memcpy_s(filenamebuffer, pNameInfo->Name.Length*sizeof(WCHAR), pNameInfo->Name.Buffer, pNameInfo->Name.Length*sizeof(WCHAR));
			wstrfilename = filenamebuffer;
			DevicePathToDrivePath(wstrfilename);

#ifdef _DEBUG
			printf("[%d], FileName:%S\n", count, wstrfilename.c_str());
#endif
			std::transform(wstrfilename.begin(), wstrfilename.end(), wstrfilename.begin(), towlower);
			itemlist.push_back(wstrfilename);

			count++;
		Clean:
			if (newhandle)
				CloseHandle(newhandle);
		}

	Exit0:
		if (NULL != pbuffer)
		{
			VirtualFree(pbuffer, 0, MEM_RELEASE);
		}
		if (targetproc)
			CloseHandle(targetproc);
	}


	bool GetProcessCreateTime(ULONG processid, FILETIME &timeCreate)
	{
		bool result = false;
		FILETIME time1;
		FILETIME time2;
		FILETIME time3;
		HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processid);
		if (nullptr == hProc)
		{
			return false;
		}
		
		if (GetProcessTimes(hProc, &timeCreate, &time1, &time2, &time3))
			result = true;
		ULONG err = GetLastError();

		CloseHandle(hProc);

		return result;
	}

};


