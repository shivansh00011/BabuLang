section .data
value0 dq 10
value1 dq 20
section .text
    global _start
_start:
    mov rax, [rel value0]
    push rax
    mov rax, [rel value0]
    push rax
    mov rax, [rel value1]
    push rax
    push QWORD [rsp + 16]
    pop rax
    push rax
    sub rsp, 32
    mov rcx, 31
    mov BYTE [rsp + rcx], 10
    dec rcx
    cmp rax, 0
    jne .convert_0
    mov BYTE [rsp + rcx], '0'
    dec rcx
    jmp .print_0
.convert_0:
    xor rdx, rdx
    mov rbx, 10
    div rbx
    add dl, '0'
    mov BYTE [rsp + rcx], dl
    dec rcx
    test rax, rax
    jnz .convert_0
.print_0:
    inc rcx
    mov rdx, 32
    sub rdx, rcx
    mov rax, 0x2000004
    mov rdi, 1
    lea rsi, [rsp + rcx]
    syscall
    add rsp, 32
    pop rax
    mov rax, 0x2000001
    mov rdi, 0
    syscall
