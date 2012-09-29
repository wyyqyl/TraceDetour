#include "Driver.h"

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD TraceDetourEvtDeviceAdd;
EVT_WDF_DRIVER_UNLOAD TraceDetourEvtDriverUnload;

PATCH_INFO patchInfo;

void DisableWriteProtection()
{
	__asm
	{
		push ebx;
		mov ebx, cr0;
		and ebx, 0xfffeffff;
		mov cr0, ebx;
		pop ebx;
	}
}

void EnableWriteProtection()
{
	__asm
	{
		push ebx;
		mov ebx, cr0;
		or ebx, 0x00010000;
		mov cr0, ebx;
		pop ebx;
	}
}

NTSTATUS VerifySignature(BYTE *fptr, BYTE *signature, ULONG sigSize)
{
	ULONG idx;
	for (idx = 0; idx < sigSize; ++idx)
	{
		if (fptr[idx] != signature[idx])
			return STATUS_UNSUCCESSFUL;
	}
	return STATUS_SUCCESS;
}

NTSTATUS patchRoutine()
{
	NTSTATUS status;

	InitPatchInfo_NtSetValueKey(&patchInfo);
	status = VerifySignature(patchInfo.SystemCall, patchInfo.Signature, patchInfo.SignatureSize);
	if (!NT_SUCCESS(status))
		return status;
	
	DisableWriteProtection();
	memcpy(patchInfo.SystemCall, patchInfo.PrologPatch, patchInfo.SizePrologPatch);
	memcpy(patchInfo.SystemCall + patchInfo.EpilogPatchOffset, patchInfo.EpilogPatch, patchInfo.SizeEpilogPatch);
	EnableWriteProtection();

	return status;
}

void recoverRoutine()
{
	DisableWriteProtection();
	memcpy(patchInfo.SystemCall, patchInfo.PrologOriginal, patchInfo.SizePrologPatch);
	memcpy(patchInfo.SystemCall + patchInfo.EpilogPatchOffset, patchInfo.EpilogOriginal, patchInfo.SizeEpilogPatch);
	EnableWriteProtection();
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	NTSTATUS status = STATUS_SUCCESS;
	WDF_DRIVER_CONFIG config = {0};

	KdPrint(("[DriverEntry]\n"));
	WDF_DRIVER_CONFIG_INIT(&config, TraceDetourEvtDeviceAdd);
	config.EvtDriverUnload = TraceDetourEvtDriverUnload;
	status = WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &config, WDF_NO_HANDLE);

	return patchRoutine();
}

NTSTATUS TraceDetourEvtDeviceAdd(WDFDRIVER Driver, PWDFDEVICE_INIT DeviceInit)
{
	NTSTATUS status = STATUS_SUCCESS;
	WDFDEVICE hDevice;

	KdPrint(("[TraceDetourEvtDeviceAdd]\n"));
	status = WdfDeviceCreate(&DeviceInit, WDF_NO_OBJECT_ATTRIBUTES, &hDevice);
	return status;
}

void TraceDetourEvtDriverUnload(WDFDRIVER Driver)
{
	KdPrint(("[TraceDetourEvtDriverUnload]\n"));
	recoverRoutine();
}