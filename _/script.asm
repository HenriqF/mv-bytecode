get r0 5
mov r1 5

mov r2 r1
loop:
    sub r2 1
    add r0 r2

    add r2 63
    mov h0 r2
    sub r2 63

    sub r0 r2

    cmp r2 1
    jeqbigger @loop


mov r2 0
loop2:
    add r0 r2
    mov r8 h0
    sub r0 r2

    debug

    add r2 1
    cmp r2 r1
    jsmaller @loop2
