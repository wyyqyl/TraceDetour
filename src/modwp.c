#include <modwp.h>

void enableWP_CR0()
{
	__asm
	{
		PUSH EAX
		MOV EAX, CR0
		OR EAX, 0x00010000
		MOV CR0, EAX
		POP EAX
	}
	return;
}

void disableWP_CR0()
{
	__asm
	{
		PUSH EAX
		MOV EAX, CR0
		AND EAX, 0xFFFEFFFF
		MOV CR0, EAX
		POP EAX
	}
}