jmp @inicio


recursivo:
    push r1
    push r2

    top r2
    sub r2 3

    mov r1 s2
    sub r1 1
    debug

    jzneg @fim_recursivo
        push r1
        call @recursivo
        pop r1

    fim_recursivo:
    mov s2 r1
    pop r2
    pop r1
    return


inicio:
    mov r0 20
    debug


    push r0
    call @recursivo
    pop r0

    debug