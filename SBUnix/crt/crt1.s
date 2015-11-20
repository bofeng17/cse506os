    .globl _start
_start:
    # need to verify rsp & offset based on rsp (stack alignment)

    mov    %rsp,%rax
    mov    (%rax),%edi              # value of argc
    lea    0x8(%rax),%rsi           # addr of argv
    movslq %edi,%rax
    lea    0x8(%rsi,%rax,8),%rdx    # addr of envp
    callq  main
    mov    %eax,%edi                # pass ret_val of main to exit()

    # exit doesn't implemented yet
    # callq  exit

    # no need to do this:
    # retq