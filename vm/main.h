#include <stdio.h>
#include <math.h>

#define UTIL_FILE
#define UTIL_STRING
#define UTIL_DEBUG
#define UTIL_IMP
#include "../util.h"

#include "stack.h"

#define REG_COUNT 9

char* script;
size_t script_size = 0;

typedef long long dlong;
typedef long long reg;

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

} tipo_instrucao;

typedef enum {
    t_absoluto,
    t_registro,
    t_stack,
    t_heap,
} valor_tipo;

typedef struct {
    dlong valor;
    int tamanho;
    valor_tipo tipo;
} operando;

typedef struct{
    tipo_instrucao tipo;
    operando opA;
    operando opB;
    int opq;
} instrucao;

instrucao** instrucoes;
size_t instrucoes_size = 0;
size_t qtd_instrucao = 0;
size_t pos_instrucao = 0;

stack* stak;
reg regs[50];

operando opA;
operando opB;
dlong ultimo_valor;

