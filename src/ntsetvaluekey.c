#include <patch.h>
#include <ntaddress.h>
#include <ntsetvaluekey.h>

DWORD Fixup_Tramp_NtSetValueKey;
DWORD Fixup_Remainder_NtSetValueKey;

static void displayMsg()
{
	KdPrint(("[yorath]Prolog"));
}

__declspec(naked) Prolog_NtSetValueKey()
{
	__asm
	{
		call displayMsg
	}
	
	// Trampoline	
	__asm
	{
		PUSH 0x78
		PUSH Fixup_Tramp_NtSetValueKey
	}
	
	__asm
	{
		PUSH Fixup_Remainder_NtSetValueKey
		RET
	}
}

DWORD RetValue_NtSetValueKey;
DWORD KeyHandle_NtSetValueKey;
DWORD ValueName_NtSetValueKey;
DWORD Type_NtSetValueKey;
DWORD Data_NtSetValueKey;
DWORD DataSize_NtSetValueKey;

static void FilterParameters()
{
	KdPrint(("[yorath]Epilog"));
}

__declspec(naked) Epilog_NtSetValueKey()
{
	__asm
	{
		MOV RetValue_NtSetValueKey, EAX
		
		MOV EAX, [ESP + 8]
		MOV Type_NtSetValueKey, EAX
		
		MOV EAX, [ESP + 16]
		MOV Data_NtSetValueKey, EAX
		
		MOV EAX, [ESP + 20]
		MOV DataSize_NtSetValueKey, EAX
		
		CALL FilterParameters
	}
	
	__asm
	{
		MOV EAX, RetValue_NtSetValueKey
		RET 0x18
		NOP
		NOP
	}
}


NTSTATUS VerifySignature(BYTE *fptr, BYTE *signature, DWORD sigSize)
{
	DWORD idx;
	for (idx = 0; idx < sigSize; ++idx)
	{
		if (fptr[idx] != signature[idx])
			return STATUS_UNSUCCESSFUL;
	}
	return STATUS_SUCCESS;
}

void InitPatchInfo_NtSetValueKey(PATCH_INFO *pInfo)
{
	DWORD idx;
	BYTE PrologPatch[] = { 0x68, 0xBE, 0xBA, 0xFE, 0xCA, 0xC3, 0x90};
	BYTE EpilogPatch[] = { 0x68, 0xBE, 0xBA, 0xFE, 0xCA, 0xC3 };
	
	pInfo->SystemCall = SYSTEM_SERVICE(ZwSetValueKey);
	KdPrint(("[yorath]pInfo->SystemCall: %08x", pInfo->SystemCall));
	
	pInfo->SignatureSize = 3;
	memcpy(pInfo->Signature, pInfo->SystemCall, pInfo->SignatureSize);
	
	pInfo->SizePrologPatch = sizeof(PrologPatch);
	pInfo->PrologDetour = (BYTE *)Prolog_NtSetValueKey;
	*(DWORD *)&(PrologPatch[1]) = (DWORD)(pInfo->PrologDetour);
	memcpy(pInfo->PrologPatch, PrologPatch, pInfo->SizePrologPatch);
	memcpy(pInfo->PrologOriginal, pInfo->SystemCall, pInfo->SizePrologPatch);
	
	pInfo->EpilogPatchOffset = 0x3C4;
	pInfo->SizeEpilogPatch = sizeof(EpilogPatch);
	pInfo->EpilogDetour = (BYTE *)Epilog_NtSetValueKey;
	*(DWORD *)&(EpilogPatch[1]) = (DWORD)(pInfo->EpilogDetour);
	memcpy(pInfo->EpilogPatch, EpilogPatch, pInfo->SizeEpilogPatch);
	memcpy(pInfo->EpilogOriginal, pInfo->SystemCall + pInfo->EpilogPatchOffset, pInfo->SizeEpilogPatch);
	
	Fixup_Tramp_NtSetValueKey = *((DWORD *)&((pInfo->PrologOriginal)[pInfo->SignatureSize]));
	Fixup_Remainder_NtSetValueKey = (DWORD)(pInfo->SystemCall) + pInfo->SizePrologPatch;
	KdPrint(("[yorath]Fixup_Tramp_NtSetValueKey: %08x", Fixup_Tramp_NtSetValueKey));
	KdPrint(("[yorath]Fixup_Remainder_NtSetValueKey: %08x", Fixup_Remainder_NtSetValueKey));
}