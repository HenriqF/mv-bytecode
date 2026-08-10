jmp @inicio


fib_recursivo:
    push r0
    push r1
    push r2
    top r2
    sub r2 4
    mov r0 s2

    push r0
    sub r0 1
    pop r0
    jzneg @fim_funcao

    mov r1 r0

    sub r0 1
    push r0
    call @fib_recursivo
    pop r0

    sub r1 2
    push r1
    call @fib_recursivo
    pop r1

    add r0 r1

    fim_funcao:
        mov s2 r0
        pop r2
        pop r1
        pop r0
        return


inicio:
    mov r0 30

    push r0
    call @fib_recursivo
    pop r0

    debug
