#include <stdio.h>
#include <math.h>

#define UTIL_FILE
#define UTIL_STRING
#define UTIL_DEBUG
#define UTIL_IMP
#include "../util.h"

#include "stack.h"
#include "../tipo.h"

#define REG_COUNT 9

char* script;
size_t script_size = 0;

typedef long long dlong;
typedef long long reg;


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

