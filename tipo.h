typedef enum {
    i_none,
    
    //triop
    i_add,
    i_sub, 
    i_mul, 
    i_div, 
    i_mod, 
    i_and, 
    i_or , 
    i_xor, 
    i_mov,
    i_get,

    //biop
    i_not, 

    i_jzero,
    i_jnzero,
    i_jeven,
    i_jodd,
    i_jpos,
    i_jneg,
    i_jzneg,
    i_jzpos, 
    i_jmp, 

    i_call, 
    i_push,
        //biop que nao pode valor absoluto
    i_free,
    i_pop,
    i_top,

    //siop?
    i_return,
    i_debug,

} tipo_instrucao;