#include "stack.h"

void mostrar_stack(stack* stack){
    printf("(%zu %zu)[ ", stack->topo, stack->tamanho);

    for (size_t i = 0 ; i < stack->topo; i++){
        printf("%lld ", stack->itens[i]);
    }
    printf("]\n");
}

stack* novo_stack(){
    stack* new_stack = malloc(sizeof(stack));
    new_stack->itens = malloc(TAMANHO_SIG*sizeof(long long));//mn
    new_stack->tamanho = TAMANHO_SIG;
    new_stack->topo = 0;
    return new_stack;
}

int crescer(stack* stack){
    // printf("TAMANHO: %d\n", stack->tamanho);

    long long* re = realloc(stack->itens, (stack->tamanho*2) * sizeof(long long));
    if (re == NULL){
        return -1;
    }

    stack->itens = re;
    stack->tamanho *= 2;

    return 0;
}

long long stack_add(stack* stack, long long num){
    if (stack->topo >= stack->tamanho-1){
        int res = crescer(stack);
        if (res == -1) return res;
    }

    stack->itens[stack->topo] = num;
    return stack->topo++;
}

long long stack_pop(stack* stack){
    if (stack->topo <= 0) return 0;

    return(stack->itens[--stack->topo]);
}

long long stack_ver(stack* stack){
    if (stack->topo <= 0) return 0;

    return(stack->itens[stack->topo-1]);
}

