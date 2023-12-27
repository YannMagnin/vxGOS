#include <setjmp.h>

/* Jumping there will properly unwind and leave the kernel */
extern jmp_buf vhex_exitbuf;
/* Return value of main() */
extern int vhex_exitcode;

/* Standard _Exit, used by the fxlibc exit() to leave control */
void _Exit(int rc)
{
	vhex_exitcode = rc;
	longjmp(vhex_exitbuf, 1);
}
