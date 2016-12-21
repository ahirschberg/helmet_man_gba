@ at top of your file
    .text           @ aka .section .text
    .code 16        @ aka .thumb

@ VBLANK
    .align 2
    .global VBlankIntrWait
    .thumb_func
VBlankIntrWait:
    swi     0x05
    bx      lr
