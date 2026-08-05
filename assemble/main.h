#include <stdio.h>
#include <math.h>
#include "hash.h"

#define UTIL_FILE
#define UTIL_STRING
#define UTIL_DEBUG
#define UTIL_IMP
#include "../util.h"

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
    i_jze,
    i_jnz, 

    i_jev,
    i_jod,

    i_jeq,

    i_jlt, 
    i_jleq, 

    i_jgt, 
    i_jgeq, 

    i_jmp, 
    i_call, 
    i_push,
        //biop que nao pode valor absoluto
    i_free,
    i_pop,
    i_top,

    //siop?
    i_return,

} tipo_instrucao;

typedef enum {
    t_absoluto,
    t_registro,
    t_stack,
    t_heap,
} valor_tipo;