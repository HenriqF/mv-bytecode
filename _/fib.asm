mov r0 0
mov r1 1
mov r2 0

#calcula o numero 15 de fibos e coloca em r0
mov r3 15

    mov r2 r0
    add r2 r1
    mov r1 r0
    mov r0 r2

    sub r3 1
    jnz 4
