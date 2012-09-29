#include "Driver.h"

ULONG Fixup_Tramp_NtSetValueKey;
ULONG Fixup_Remainder_NtSetValueKey;

ULONG RetValue_NtSetValueKey;
ULONG ValueName_NtSetValueKey;
ULONG Type_NtSetValueKey;
ULONG Data_NtSetValueKey;

void displayMsg()
{
	KdPrint(("Prolog function is called\n"));
}

NTSTATUS FilterParameters()
{
	if (NT_SUCCESS(RetValue_NtSetValueKey))
	{
		KdPrint(("[FilterParameters]:\tValue Name=%S\n", ((PUNICODE_STRING)ValueName_NtSetValueKey)->Buffer));
		switch (Type_NtSetValueKey)
		{
		case REG_BINARY:
			KdPrint(("\t\tType==REG_BINARY\n"));
			break;
		case REG_DWORD:
			KdPrint(("\t\tType==REG_DWORD\n"));
			break;
		case REG_EXPAND_SZ:
			KdPrint(("\t\tType==REG_EXPAND_SZ\n"));
			break;
		case REG_LINK:
			KdPrint(("\t\tType==REG_LINK\n"));
			break;
		case REG_MULTI_SZ:
			KdPrint(("\t\tType==REG_MULTI_SZ\n"));
			break;
		case REG_NONE:
			KdPrint(("\t\tType==REG_NONE\n"));
			break;
		case REG_SZ:
			KdPrint(("\t\tType==REG_SZ\tData=%S\n", Data_NtSetValueKey));
			break;
		default:
			KdPrint(("\t\tType==Unknown\n"));
			break;
		}
	}

	return RetValue_NtSetValueKey;
}

__declspec(naked) void Prolog_NtSetValueKey()
{	
	__asm
	{
		call displayMsg;
		// recover
		push 0x78;
		push Fixup_Tramp_NtSetValueKey;
		// return
		push Fixup_Remainder_NtSetValueKey
		ret
	}
}

__declspec(naked) void Epilog_NtSetValueKey()
{
	__asm
	{
		mov RetValue_NtSetValueKey, eax;
		mov eax, [esp+8];
		mov ValueName_NtSetValueKey, eax;
		mov eax, [esp+16];
		mov Type_NtSetValueKey, eax;
		mov eax, [esp+20];
		mov Data_NtSetValueKey, eax;
		call FilterParameters;
		ret 0x18;
		nop;
		nop;
	}
}

void InitPatchInfo_NtSetValueKey(PATCH_INFO *pInfo)
{
	BYTE PrologPatch[] = { 0x68, 0xBE, 0xBA, 0xFE, 0xCA, 0xC3, 0x90};
	BYTE EpilogPatch[] = { 0x68, 0xBE, 0xBA, 0xFE, 0xCA, 0xC3 };

	pInfo->SystemCall = (BYTE *)SYSTEM_SERVICE(ZwSetValueKey);
	KdPrint(("pInfo->SystemCall: %p\n", pInfo->SystemCall));

	pInfo->SignatureSize = 3;
	memcpy(pInfo->Signature, pInfo->SystemCall, pInfo->SignatureSize);

	pInfo->SizePrologPatch = sizeof(PrologPatch);
	pInfo->PrologDetour = (BYTE *)Prolog_NtSetValueKey;
	*(ULONG *)(PrologPatch + 1) = (ULONG)(pInfo->PrologDetour);
	memcpy(pInfo->PrologPatch, PrologPatch, pInfo->SizePrologPatch);
	memcpy(pInfo->PrologOriginal, pInfo->SystemCall, pInfo->SizePrologPatch);

	pInfo->EpilogPatchOffset = 0x3C4;
	pInfo->SizeEpilogPatch = sizeof(EpilogPatch);
	pInfo->EpilogDetour = (BYTE *)Epilog_NtSetValueKey;
	*(ULONG *)(EpilogPatch + 1) = (ULONG)(pInfo->EpilogDetour);
	memcpy(pInfo->EpilogPatch, EpilogPatch, pInfo->SizeEpilogPatch);
	memcpy(pInfo->EpilogOriginal, pInfo->SystemCall + pInfo->EpilogPatchOffset, pInfo->SizeEpilogPatch);

	Fixup_Tramp_NtSetValueKey = *(ULONG *)(pInfo->PrologOriginal + pInfo->SignatureSize);
	Fixup_Remainder_NtSetValueKey = (ULONG)(pInfo->SystemCall) + pInfo->SizePrologPatch;
	KdPrint(("Fixup_Tramp_NtSetValueKey: 0x%08x", Fixup_Tramp_NtSetValueKey));
	KdPrint(("Fixup_Remainder_NtSetValueKey: 0x%08x", Fixup_Remainder_NtSetValueKey));
}