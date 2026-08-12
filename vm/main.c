#include "main.h"

void mostrar_memoria(int i){
    printf("\n");
    for (int i = 0 ; i < REG_COUNT; i++){
        if (i % 3 == 0 && i != 0){
            printf("\n");
        }
        printf("%2d[%16lld]  ", i, regs[i]);
    }
    printf("\n  ");
    mostrar_stack(stak);
    printf("[%d]\n", i);
}


//PARSING

size_t get_operando_info(size_t pos, operando* op){
    int op_info = script[pos];
    
    size_t tamanho = 0;
    if (op_info >= 60){
        tamanho = op_info-60;
        op->tipo = t_heap;
    }
    else if (op_info >= 40){
        tamanho = op_info-40;
        op->tipo = t_stack;
    }
    else if (op_info >= 20){
        tamanho = op_info-20;
        op->tipo = t_registro;
    }
    else {
        tamanho = op_info;
        op->tipo = t_absoluto;
    }
    if (tamanho > 8) tamanho = 8;
    
    size_t limit = (++pos) + tamanho;
    if (limit > script_size){
        eprintf("instrucao malformada \n");
    }

    op->valor = 0;
    for (size_t i = 0; i < tamanho; i++) {
        // printf("[%d]", script[pos]);
        op->valor |= ((dlong)(unsigned char) script[pos++]) << (i*8);
    }
    // printf(": [%lld]\n", op->valor);

    return pos;
}

static inline instrucao* pegar_proxima_inst(){
    if (qtd_instrucao >= instrucoes_size){
        size_t nt = 2*instrucoes_size;
        instrucao* temp = realloc(instrucoes, nt*sizeof(instrucao));
        if (temp == NULL){
            eprintf("erro ao criar espaco para novas instrucoes\n");
        }

        instrucoes = temp;
        instrucoes_size = nt;
    }

    return &instrucoes[qtd_instrucao++];
}

void append_instrucao_siop(tipo_instrucao tipo){
    instrucao* inst = pegar_proxima_inst();

    inst->tipo = tipo;
    inst->opq = 0;
}

void append_instrucao_biop(tipo_instrucao tipo, operando opA){
    instrucao* inst = pegar_proxima_inst();
    
    inst->tipo = tipo;
    inst->opA = opA;
    inst->opq = 1;
}

void append_instrucao_triop(tipo_instrucao tipo, operando opA, operando opB){
    instrucao* inst = pegar_proxima_inst();

    inst->tipo = tipo;
    inst->opA = opA;
    inst->opB = opB;
    inst->opq = 2;
}


size_t processar_triop(size_t pos){
    tipo_instrucao tipo = script[pos++];
    operando opA;
    operando opB;
    
    pos = get_operando_info(pos, &opA);
    pos = get_operando_info(pos, &opB);

    if (opA.tipo == t_absoluto){
        eprintf("destino de operacação nao pode ser valor absoluto... \n");
    };

    append_instrucao_triop(tipo, opA, opB);
    return pos;
}

size_t processar_biop(size_t pos){
    tipo_instrucao tipo = script[pos++];
    operando opA;

    pos = get_operando_info(pos, &opA);

    append_instrucao_biop(tipo, opA);
    return pos;
}

size_t processar_siop(size_t pos){
    tipo_instrucao tipo = script[pos++];

    append_instrucao_siop(tipo);
    return pos;
}

void parse_programa(){
    size_t i = 0;

    while (i < script_size){
        if (script[i] >= i_add && script[i] <= i_cmp){
            i = processar_triop(i);
        }
        else if (script[i] >= i_not && script[i] <= i_top){
            i = processar_biop(i);
        }
        else if ((script[i] >= i_return && script[i] <= i_fim) || script[i] == i_none ){
            i = processar_siop(i);
        } 
        else {
            eprintf("instrução desconhecida... pos[%d] n[%d]\n", i, script[i]);
        }
    }

    append_instrucao_siop(i_fim);
}

//=======


static inline dlong* get_valor_source(operando* op){
    switch (op->tipo){
        case t_absoluto:
            return &op->valor;
        case t_registro:
            return &regs[op->valor];
        case t_stack:
            dlong pos = regs[op->valor]; 
            return &stak->itens[pos];
        case t_heap:
            // printf("{%p, %zu}\n", (dlong*)op->valor, op->valor);
            return (dlong*)(regs[op->valor] * 8);
        default:
            eprintf("nem eu sei");
    }
    eprintf("nem eu sei");
    return NULL;
}

void executar_programa(){
    static void* instrucao_tabela[] = {
        &&op_none,
        
        &&op_add,
        &&op_sub, 
        &&op_mul, 
        &&op_div, 
        &&op_mod, 
        &&op_and, 
        &&op_or, 
        &&op_xor, 
        &&op_mov,
        &&op_get,
        &&op_cmp,

        &&op_not, 
        &&op_jzero,
        &&op_jnzero,
        &&op_jeven,
        &&op_jodd,
        &&op_jpos,
        &&op_jneg,
        &&op_jzneg,
        &&op_jzpos, 
        &&op_jmp, 
        &&op_call, 
        &&op_push,
        
        &&op_free,
        &&op_pop,
        &&op_top,

        &&op_return,
        &&op_debug,
        &&op_fim
    };
    instrucao* inst = instrucoes-1;

    dlong* destino;
    dlong* secundario;

    // #define prox() printf("\n%zu ", (size_t)(inst-instrucoes+1)); goto *instrucao_tabela[(++inst)->tipo]; 
    #define prox() goto *instrucao_tabela[(++inst)->tipo]; 

    #define jump() \
        do {\
            inst = instrucoes+(*destino);\
            goto *instrucao_tabela[inst->tipo];\
        } while (0)\
        
    #define load_dest_sec()\
        destino = get_valor_source(&inst->opA);\
        secundario = get_valor_source(&inst->opB)

    #define triop_ultval(sinal)\
        { \
            load_dest_sec();\
            dlong res = (*destino) sinal (*secundario);\
            (*destino) = res;\
            ultimo_valor = res;\
        }

    #define biop_ultaval(sinal)\
        { \
            destino = get_valor_source(&inst->opA);\
            dlong res = sinal (*destino);\
            (*destino) = res;\
            ultimo_valor = res;\
        }

    op_none:
        prox();

    op_add:
        triop_ultval(+)
        prox();

    op_sub:
        triop_ultval(-)
        prox();

    op_mul:
        triop_ultval(*)
        prox();

    op_div:
        triop_ultval(/)
        prox();

    op_mod:
        triop_ultval(%)
        prox();

    op_and:
        triop_ultval(&)
        prox();

    op_or:
        triop_ultval(|)
        prox();

    op_xor:
        triop_ultval(^)
        prox();

    op_mov:
        load_dest_sec();
       // printf("PORRA [%zu][%zu]\n", (*secundario), secundario);
        (*destino) = (*secundario);
        prox();

    op_get:
        load_dest_sec();
        void* bytes = calloc(*secundario, sizeof(dlong));
        (*destino) = (dlong)bytes / 8;
        prox();

    op_cmp:
        load_dest_sec();
        ultimo_valor = (*destino) - (*secundario);
        prox();

    op_not:
        biop_ultaval(~)
        prox();

    op_jzero:
        destino = get_valor_source(&inst->opA);
        if (ultimo_valor == 0) jump();
        prox();

    op_jnzero:
        destino = get_valor_source(&inst->opA);
        if (ultimo_valor != 0) jump();
        prox();

    op_jeven:
        destino = get_valor_source(&inst->opA);
        if ((ultimo_valor & 1) != 0) jump();
        prox();

    op_jodd:
        destino = get_valor_source(&inst->opA);
        if ((ultimo_valor & 1) == 0) jump();
        prox();

    op_jpos:
        destino = get_valor_source(&inst->opA);
        if (ultimo_valor > 0) jump();
        prox();

    op_jneg:
        destino = get_valor_source(&inst->opA);
        if (ultimo_valor < 0) jump();
        prox();

    op_jzneg:
        destino = get_valor_source(&inst->opA);
        if (ultimo_valor <= 0) jump();
        prox();

    op_jzpos:
        destino = get_valor_source(&inst->opA);
        if (ultimo_valor >= 0) jump();
        prox();

    op_jmp:
        destino = get_valor_source(&inst->opA);
        jump();

    op_call:
        destino = get_valor_source(&inst->opA);
        stack_add(stak, inst-instrucoes);
        jump();

    op_push:
        destino = get_valor_source(&inst->opA);
        stack_add(stak, (*destino));
        prox();

    op_free:
        destino = get_valor_source(&inst->opA);
        free((void*)(*destino));
        (*destino) = 0;
        prox();

    op_pop:
        destino = get_valor_source(&inst->opA);
        (*destino) = stack_pop(stak);
        prox();

    op_top:
        destino = get_valor_source(&inst->opA);
        (*destino) = (stak->topo)-1;
        prox();

    op_return:
        inst = instrucoes+stack_pop(stak);
        prox();

    op_debug:
        mostrar_memoria(-1);
        prox();

    op_fim:
        return;
}

int main(){
    stak = novo_stack();

    instrucoes = malloc(2000*sizeof(instrucao));
    instrucoes_size = 2000;

    FILE* f = fopen("../_/script.vm", "rb");
    readFile(f, &script_size, &script);

    
    parse_programa();
    executar_programa();
    return 0;
}