#include "fxcg50/asm_utils.h"

.text

!---
! Public
!---

/* _bios_free() (free) : free allocated memory */
function(_bios_free):
    syscall(0x1f42)
