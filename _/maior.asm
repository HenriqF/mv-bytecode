jmp @inicio

maior:
    push r0
    push r1
    push r2
    
    top r2
    sub r2 4

    mov r1 s2
    sub r2 1
    mov r0 s2
    sub r0 r1

    jzneg @menor_igual
        jmp @fim_maior

    menor_igual:
        add r2 1
        mov r0 s2
        sub r2 1
        mov s2 r0
        jmp @fim_maior


    fim_maior:
        pop r2
        pop r1
        pop r0
        return


inicio:
    push 1
    push 3
    call @maior
    pop r0
    
    push 2
    call @maior
    pop r0
    pop r0

    debug