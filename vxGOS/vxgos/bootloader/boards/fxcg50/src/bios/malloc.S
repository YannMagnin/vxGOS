#include "fxcg50/asm_utils.h"

.text

!---
! Public
!---

/* _bios_malloc() (malloc) : allocate memory */
function(_bios_malloc):
    syscall(0x1f44)
