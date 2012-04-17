#ifndef __NTSETVALUEKEY_H
#define __NTSETVALUEKEY_H

#include <wdm.h>

NTSTATUS VerifySignature(BYTE *fptr, BYTE *signature, DWORD sigSzie);
void InitPatchInfo_NtSetValueKey(PATCH_INFO *pInfo);

#endif