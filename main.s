    section .bss

    section .rodata

    section .text

    extern printf, getline, stdin
    extern malloc, free

    global main

main:
    push rbp ; rsp - 8
    mov rbp, rsp

    sub rsp, 8 ; rsp - 16

    add rsp, 8 ; rsp - 8

    pop rbp

    xor rax, rax ; return 0
    ret

