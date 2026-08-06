#ifndef STACK
#define STACK

#include <stdio.h>
#include <stdlib.h>

#define TAMANHO_SIG 2000

typedef struct {
    long long* itens;
    size_t tamanho;
    size_t topo;
} stack;

void mostrar_stack(stack* stack);

stack* novo_stack();
long long stack_add(stack* stack, long long num);
long long stack_pop(stack* stack);
long long stack_ver(stack* stack);

#endif