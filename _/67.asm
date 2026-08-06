
mov r1 3
loop:
    add r0 20

    #if r1 ==2
    push r1
    sub r1 2
    jnzero @fim_if
    
        add r0 7

    fim_if:
        pop r1


    sub r1 1
    jnzero @loop

debug