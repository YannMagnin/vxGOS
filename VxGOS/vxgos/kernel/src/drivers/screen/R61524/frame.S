#if 0
.text

.global _r61524_frame_frag_send

.balign 4

! r61524_frame_frag_send() : send fragment in screen
_r61524_frame_frag_send:

    ! prepare loop information
    mov #0xb4, r3       ! EX (1)
    mov.l   @(8, r4), r0        ! LS
    shll8   r3          ! EX (2)
    mov.l   @(0, r4), r2        ! LS
    shll16  r3          ! EX (3)
    mov.w   long_frag, r1       ! LS
    cmp/eq  #4, r0          ! EX (4)
    bf  1f          ! BR (?)
    mov.w   short_frag, r1      ! LS
    nop             ! MT

    ! prepare loop
1:  ldrs    2f
    ldre    3f
    nop
    ldrc    r1

2:  mov.w   @r2+, r0
3:  mov.w   r0, @r3

    rts
    nop

long_frag:
    .word   3960
short_frag:
    .word   1584
#endif

#if 0
    ! check number of word to be send
    mov.l   @(20, r4), r0       ! LS
    cmp/eq  #220, r0        ! EX (1)
    mov.w   long_frag, r1       ! LS
    bf  prepare_loop        ! BR (2)
    mov.w   short_frag, r1      ! LS
    nop             ! MT (3)

prepare_loop:

    mov.l   @(4, r4), r0        ! LS (4)
    mov.l   r61524_interface, r3    ! LS (5)

send_loop:

    mov.w   @r0+, r2        ! LS
    dt  r1          ! EX (6)
    mov.w   r2, @r3         ! LS
    bf  send_loop       ! BR (7)

send_epilogue:

    rts
    xor r0, r0

long_frag:
    .word   3960
short_frag:
    .word   1584
r61524_interface:
    .long   0xb4000000
#endif
