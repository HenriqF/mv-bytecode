#include <stdio.h>
#include <math.h>

#define UTIL_FILE
#define UTIL_STRING
#define UTIL_DEBUG
#define UTIL_IMP
#include "util.h"

#include "stack.h"

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
    i_push,
    i_pop,

    //biop
    i_not, 
    i_jeq, 
    i_jlt, 
    i_jle, 
    i_jgt, 
    i_jge, 
    i_jev,
    i_jod,
    i_jmp, 
    i_call, 
    i_return,
    i_free,

} instrucao;

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

#define REG_COUNT 50

stack* stak;
reg regs[50];

operando opA;
operando opB;
dlong ultimo_valor;

