#include <wdm.h>
#include <driver.h>
#include <patch.h>
#include <datatype.h>
#include <ntsetvaluekey.h>
#include <modwp.h>
#include <irql.h>


PATCH_INFO patchInfo;

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	KIRQL irql;
	KdPrint(("[yorath]Unload"));
	
	disableWP_CR0();
	irql = RaiseIRQL();
	
	memcpy(patchInfo.SystemCall, patchInfo.PrologOriginal, patchInfo.SizePrologPatch);
	memcpy(patchInfo.SystemCall + patchInfo.EpilogPatchOffset, patchInfo.EpilogOriginal, patchInfo.SizeEpilogPatch);
	
	LowerIRQL(irql);
	enableWP_CR0();
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS ntStatus = STATUS_SUCCESS;
	KIRQL irql;
	PKDPC dpcPtr;
	
	DriverObject->DriverUnload = Unload;
	
	InitPatchInfo_NtSetValueKey(&patchInfo);
	
	ntStatus = VerifySignature(patchInfo.SystemCall, patchInfo.Signature, patchInfo.SignatureSize);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;
	
	disableWP_CR0();
	irql = RaiseIRQL();
	
	memcpy(patchInfo.SystemCall, patchInfo.PrologPatch, patchInfo.SizePrologPatch);
	memcpy(patchInfo.SystemCall + patchInfo.EpilogPatchOffset, patchInfo.EpilogPatch, patchInfo.SizeEpilogPatch);
	
	LowerIRQL(irql);
	enableWP_CR0();
	
	return ntStatus;
}