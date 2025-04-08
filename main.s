    section .bss

    section .rodata
    too_many_args_msg db "Too many arguments provided, expected at most 1.", 0xA, 0
    prompt_text db " > ", 0

    section .text

    extern printf, getline, stdin
    extern malloc, free

    global main
    global lex

main:
    push rbp ; rsp - 8
    mov rbp, rsp
    
    cmp rdi, 2
    jg .too_many
    je .file_arg
    jmp .no_arg

    .file_arg: ; worry about this later
    jmp .done

    .no_arg:

    ; allocate memory on the stack for the line
    sub rsp, 16 ; rsp - 24

    sub rsp, 8 ; rsp - 32
    mov rdi, prompt_text
    xor rax, rax
    call printf
    add rsp, 8 ; rsp - 24

    lea rdi, [rsp]
    lea rsi, [rsp + 8]
    mov rdx, [rel stdin]
    call getline

    push rax ; rsp - 32, store the length of the input

    mov rdi, [rsp + 8] ; + 8 because the push of rax
    call free

    ; reset the pointers to avoid double frees
    mov qword [rsp + 8], 0
    mov qword [rsp + 16], 0

    pop rax ; rsp - 24
    add rsp, 16

    cmp rax, 2 ; if the length = 0, then exit the loop
    jl .done
    jmp .no_arg

    .too_many:
    sub rsp, 8 ; rsp - 16
    mov rdi, too_many_args_msg
    xor rax, rax
    call printf
    add rsp, 8 ; rsp - 8

    .done:
    pop rbp
    xor rax, rax ; return 0
    ret


