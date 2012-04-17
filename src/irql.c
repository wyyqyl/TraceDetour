#include <irql.h>

KIRQL RaiseIRQL()
{
	KIRQL curr;
	KIRQL prev;
	curr = prev = KeGetCurrentIrql();
	if (curr < DISPATCH_LEVEL)
	{
		KeRaiseIrql(DISPATCH_LEVEL, &prev);
	}
	return prev;
}

void LowerIRQL(KIRQL prev)
{
	KeLowerIrql(prev);
	return;
}