#ifndef _PATCH_H
#define _PATCH_H

#include <datatype.h>

#define SZ_SIG_MAX		128
#define SZ_PATCH_MAX	32

typedef struct _PATCH_INFO
{
	BYTE *SystemCall;
	BYTE Signature[SZ_SIG_MAX];
	DWORD SignatureSize;
	
	BYTE *PrologDetour;
	BYTE *EpilogDetour;
	
	BYTE PrologPatch[SZ_PATCH_MAX];
	BYTE PrologOriginal[SZ_PATCH_MAX];
	DWORD SizePrologPatch;
	//DWORD PrologPatchOffset;
	
	BYTE EpilogPatch[SZ_PATCH_MAX];
	BYTE EpilogOriginal[SZ_PATCH_MAX];
	DWORD SizeEpilogPatch;
	DWORD EpilogPatchOffset;
	
} PATCH_INFO, *PPATCH_INFO;

#endif