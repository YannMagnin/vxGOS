#include <stdlib.h>
#include <setjmp.h>

#include <stdio.h>

extern void kinit(void);

__attribute__((constructor(101)))
int _initialize(void)
{
	printf("early kernel initialisation...\n");
	kinit();
	return 0;
}
