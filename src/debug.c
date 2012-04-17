#include <debug.h>

void print_mem(const BYTE *ptr, DWORD size, const BYTE *tag)
{
	DWORD i = 0;
	for(i = 0; i < size; i += 4)
	{
		KdPrint(("[yorath]%s: %02X%02X%02X%02X", tag, ptr[i], ptr[i+1], ptr[i+2], ptr[i+3]));
	}
}