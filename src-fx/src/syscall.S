	.text

    .global _Calljmp
    .global _SysCalljmp
    .global _SysCalljmp12
    
	.balign 4

_Calljmp:
    mov.l	@r15,r1
    jmp	@r1
    nop

_SysCalljmp:
    mov.l	SYSCALL,r1
    mov.l	@r15,r0
    jmp	@r1
    nop

_SysCalljmp12:
    mov.l	@(32,R15),r0
    mov.l	SYSCALL,r1
    jmp	@r1
    nop

    .balign 4

SYSCALL: .long 0x80010070
