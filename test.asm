section .data
    message db "10", 10

section .text
    global _start
    default rel

_start:
    ; macOS sys_write (0x2000004)
    mov rax, 0x2000004      ; syscall: sys_write
    mov rdi, 1              ; file descriptor: stdout
    lea rsi, [message]      ; Address of message
    mov rdx, 3              ; Length of string (including newline)
    syscall

    ; macOS sys_exit (0x2000001)
    mov rax, 0x2000001
    xor rdi, rdi
    syscall


    
    ;nasm -f macho64 test.asm -o test.o
    ;ld -o test test.o -macosx_version_min 10.13 -lSystem -syslibroot `xcrun --show-sdk-path` -e _start
    ;./test
    ;echo $?