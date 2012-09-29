#include <ntddk.h>
#include <wdf.h>

#ifndef __DRIVER_H
#define __DRIVER_H

#define SZ_SIG_MAX		128
#define SZ_PATCH_MAX	32

#pragma warning(disable:4100)
#pragma warning(disable:4054)

typedef struct _PATCH_INFO
{
	BYTE *SystemCall;
	BYTE Signature[SZ_SIG_MAX];
	ULONG SignatureSize;

	BYTE *PrologDetour;
	BYTE *EpilogDetour;

	BYTE PrologPatch[SZ_PATCH_MAX];
	BYTE PrologOriginal[SZ_PATCH_MAX];
	ULONG SizePrologPatch;
	//DWORD PrologPatchOffset;

	BYTE EpilogPatch[SZ_PATCH_MAX];
	BYTE EpilogOriginal[SZ_PATCH_MAX];
	ULONG SizeEpilogPatch;
	ULONG EpilogPatchOffset;

} PATCH_INFO, *PPATCH_INFO;

#pragma pack(1)
typedef struct ServiceDescriptorEntry
{
	ULONG *KiServiceTable;
	ULONG *CounterBaseTable;
	ULONG nSystemCalls;
	ULONG *KiArgumentTable;
} SDE, *PSDE;
#pragma pack()

__declspec(dllimport) SDE KeServiceDescriptorTable;

#define SYSCALL_INDEX(address) *((PULONG)((BYTE *)(address) + 1))
#define SYSTEM_SERVICE(address) KeServiceDescriptorTable.KiServiceTable[SYSCALL_INDEX(address)]

void InitPatchInfo_NtSetValueKey(PATCH_INFO *pInfo);

#endif