.macro push_al
	pushq 		%rax
    pushq 		%rbx
    pushq 		%rcx
    pushq 		%rdx
    pushq 		%rsi
    pushq 		%rdi
    pushq 		%rbp
    pushq 		%rsp
    pushq 		%r8
    pushq 		%r9
    pushq 		%r10
    pushq 		%r11
    pushq 		%r12
    pushq 		%r13
    pushq 		%r14
    pushq 		%r15
.endm

.macro pop_al
    popq		%r15
    popq		%r14
    popq		%r13
    popq		%r12
    popq		%r11
    popq		%r10
    popq		%r9
    popq		%r8
    popq		%rsp
    popq		%rbp
    popq		%rdi
    popq		%rsi
    popq		%rdx
    popq		%rcx
    popq		%rbx
    popq		%rax
.endm

    .globl isr32
isr32:
    cli
    push_al
    call    isr_timer
    pop_al
    sti
    iretq

    .globl isr33
isr33:
    cli
    push_al
    call    isr_keyboard
    pop_al
    sti
    iretq
