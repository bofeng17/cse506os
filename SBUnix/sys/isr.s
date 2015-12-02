.macro push_al
	pushq 		%rax
    pushq 		%rbx
    pushq 		%rcx
    pushq 		%rdx
    pushq 		%rsi
    pushq 		%rdi
    pushq 		%rbp
    pushq 		%rsp # %rsp is saved in task_struct rather than on stack in context swtich
    pushq 		%r8
    pushq 		%r9
    pushq 		%r10
    pushq 		%r11
    pushq 		%r12
    pushq 		%r13
    pushq 		%r14
    pushq 		%r15
    pushfq # must be saved on context switch. not sure for other interrupts
.endm

.macro pop_al
    popfq
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
    mov 0x90(%rsp), %rdi
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

    .globl co_yield
co_yield:
    cli
    push_al
    call    schedule
    pop_al
    sti
    iretq

 .macro CPU_Exceptions m
  .globl exception\m
  exception\m:
    cli
    mov $\m, %rdi
    jmp cpu_exception_handler
.endm

CPU_Exceptions 0
CPU_Exceptions 1
CPU_Exceptions 2
CPU_Exceptions 3
CPU_Exceptions 4
CPU_Exceptions 5
CPU_Exceptions 6
CPU_Exceptions 7
CPU_Exceptions 8
CPU_Exceptions 9
CPU_Exceptions 10
CPU_Exceptions 11
CPU_Exceptions 12
CPU_Exceptions 13

# CPU_Exceptions 14
  .globl exception14
  exception14:
    cli
    push_al # TODO: do we need to manually push all?
    lea 0x90(%rsp), %rdi #
    mov 0x88(%rsp), %rsi # error-prone: 16*8 for general-purpose registers, 8 for rflags
    call page_fault_handler
    pop_al
    add $0x8, %rsp
    sti
    iretq

CPU_Exceptions 15
CPU_Exceptions 16
CPU_Exceptions 17
CPU_Exceptions 18
CPU_Exceptions 19
CPU_Exceptions 20
CPU_Exceptions 21
CPU_Exceptions 22
CPU_Exceptions 23
CPU_Exceptions 24
CPU_Exceptions 25
CPU_Exceptions 26
CPU_Exceptions 27
CPU_Exceptions 28
CPU_Exceptions 29
CPU_Exceptions 30
CPU_Exceptions 31
