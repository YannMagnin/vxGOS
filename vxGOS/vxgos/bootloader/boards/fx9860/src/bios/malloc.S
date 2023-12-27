#include "fx9860/asm_utils.h"

.text

!---
! Public
!---

/* _bios_malloc() (malloc) : allocate memory */
function(_bios_malloc):
    syscall(0xacd)
