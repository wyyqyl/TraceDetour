#ifndef __IRQL_H
#define __IRQL_H

#include <wdm.h>

KIRQL RaiseIRQL();
void LowerIRQL(KIRQL prev);

#endif