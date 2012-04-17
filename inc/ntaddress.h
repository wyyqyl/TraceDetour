#ifndef _NT_ADDRESS
#define _NT_ADDRESS

#pragma pack(1)
typedef struct ServiceDescriptorEntry
{
	DWORD *KiServiceTable;
	DWORD *CounterBaseTable;
	DWORD nSystemCalls;
	DWORD *KiArgumentTable;
} SDE, *PSDE;
#pragma pack()

__declspec(dllimport) SDE KeServiceDescriptorTable;

#define SYSCALL_INDEX(address) *((PULONG)((BYTE *)(address) + 1))
#define SYSTEM_SERVICE(address) KeServiceDescriptorTable.KiServiceTable[SYSCALL_INDEX(address)]
#endif