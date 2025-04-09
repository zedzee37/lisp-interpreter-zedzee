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

    ; Allocate space for line pointer and size
    sub rsp, 16 ; rsp - 24

    .no_arg_loop:
    
    ; Initialize the pointers to NULL
    mov qword [rsp], 0    ; char* lineptr = NULL
    mov qword [rsp+8], 0  ; size_t size = 0

    sub rsp, 8 ; rsp - 32, align to 16 bits
    mov rdi, prompt_text
    xor rax, rax
    call printf

    ; Call getline properly
    lea rdi, [rsp + 8]        ; address of lineptr
    lea rsi, [rsp + 16]      ; address of size
    mov rdx, [rel stdin]
    call getline
    
    add rsp, 8 ; rsp - 24

    ; Check return value
    cmp rax, -1
    je .no_input          ; getline failed or EOF

    push rax ; rsp - 32

    ; free the data
    mov rdi, [rsp + 8]
    xor rax, rax
    call free

    pop rax ; rsp - 24

    cmp rax, 2
    jl .no_input

    ; Prepare for next iteration
    jmp .no_arg_loop

    .no_input:
    add rsp, 16
    jmp .done

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


