mov r1 5
get r0 r1
mov r2 r1

debug

loop:
    sub r2 1

    add r0 r2
    mov h0 r2
    mov r8 h0
    debug
    sub r0 r2

    cmp r2 1
    jzpos @loop


loop2:
    add r0 r2
    mov r8 h0
    debug
    sub r0 r2


    add r2 1
    cmp r2 r1
    jzneg @loop2