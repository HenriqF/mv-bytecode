#include "main.h"

void mostrar_memoria(int i){
    printf("\n");
    for (int i = 0 ; i < REG_COUNT; i++){
        if (i % 3 == 0 && i != 0){
            printf("\n");
        }
        printf("%2d[%16lld]  ", i, regs[i]);
    }
    printf("\n[%d]   ", i);
    mostrar_stack(stak);
    printf("\n");
}

void grow_instrucoes(){
    instrucao** temp = realloc(instrucoes, 2*instrucoes_size);
    if (temp == NULL){
        eprintf("erro ao criar espaco para novas instrucoes\n");
    }
    instrucoes = temp;
}
void append_instrucao(instrucao* inst){
    if (inst == NULL){
        instrucao* nova_instrucao = malloc(sizeof(instrucao));
        nova_instrucao->tipo = i_none;
        nova_instrucao->opq = 0;
        inst = nova_instrucao;
    }

    if (qtd_instrucao >= instrucoes_size){
        grow_instrucoes();
    }

    instrucoes[qtd_instrucao++] = inst;
}

size_t get_operando_info(size_t pos, operando* op){
    int op_info = script[pos];

    if (op_info >= 60){
        op->tamanho = op_info-60;
        op->tipo = t_heap;
    }
    else if (op_info >= 40){
        op->tamanho = op_info-40;
        op->tipo = t_stack;
    }
    else if (op_info >= 20){
        op->tamanho = op_info-20;
        op->tipo = t_registro;
    }
    else {
        op->tamanho = op_info;
        op->tipo = t_absoluto;
    }
    
    if (op->tamanho > 8) op->tamanho = 8;
    
    char bytes[8] = {0};
    int offset = 0;
    
    pos++;
    size_t limit = pos + ((size_t)op->tamanho);

    if (limit > script_size){
        eprintf("instrucao malformada \n");
    }

    for (; pos < limit; pos++){
        bytes[offset++] = script[pos];
    }

    // for (int i = 0 ; i < 8; i++)printf("[%d]", bytes[i]); 
    // printf("\n");
    
    memcpy(&op->valor, bytes, sizeof(op->valor));
    return pos;
}

dlong* get_valor_source(operando* op){
    switch (op->tipo){
        case t_absoluto:
            return &op->valor;

        case t_registro:
            if (op->valor < 0 || op->valor > REG_COUNT){
                eprintf("registro [%lld] inexistente...\n", op->valor);
            }
            return &regs[op->valor];
            break;

        case t_stack:
            break;

        case t_heap:
            break;

        default:
            return NULL;
    }
    return NULL;
}

size_t processar_triop(size_t pos){
    instrucao* nova_instrucao = malloc(sizeof(instrucao));

    tipo_instrucao tipo = script[pos];
    nova_instrucao->tipo = tipo;
    nova_instrucao->opq = 2;

    pos = get_operando_info(pos+1, &nova_instrucao->opA);
    pos = get_operando_info(pos, &nova_instrucao->opB);

    if (nova_instrucao->opA.tipo == t_absoluto){
        printf("%d %d, %lld, %d\n", tipo, nova_instrucao->opA.tamanho, nova_instrucao->opA.valor, nova_instrucao->opA.tipo);
        eprintf("destino de operacação nao pode ser valor absoluto... \n");
    };

    append_instrucao(nova_instrucao);
    return pos;
}

size_t processar_biop(size_t pos){
    instrucao* nova_instrucao = malloc(sizeof(instrucao));

    tipo_instrucao tipo = script[pos];
    nova_instrucao->tipo = tipo;
    nova_instrucao->opq = 1;

    pos = get_operando_info(pos+1, &nova_instrucao->opA);

    if (tipo >= i_free && tipo <= i_top){
        if (nova_instrucao->opA.tipo == t_absoluto){
            eprintf("destino de operacação nao pode ser valor absoluto... \n");
        };
    }

    append_instrucao(nova_instrucao);
    return pos;
}

size_t executar_instrucao(size_t pos){
    if (instrucoes[pos]->opq == 0){
        return pos+1;
    }

    dlong* destino = get_valor_source(&instrucoes[pos]->opA);

    int era_biop = 1;
    switch(instrucoes[pos]->tipo){
        case i_not:
            (*destino) = ~(*destino);
            ultimo_valor = (*destino);
            break;
        case i_jze:
            if (ultimo_valor == 0) return (*destino);
            return pos+1;
        case i_jnz:
            if (ultimo_valor != 0) return (*destino);
            return pos+1;
        case i_jev:
            break;
        case i_jod:
            break;
        case i_jeq:
            break;
        case i_jlt:
            break;
        case i_jleq:
            break;
        case i_jgt:
            break;
        case i_jgeq:
            break;
        case i_jmp:
            return (*destino);
        case i_call:
            break;
        case i_push:
            stack_add(stak, (*destino));
            break;
        case i_free:
            free((void*)(*destino));
            (*destino) = 0;
            break;
        case i_pop:
            (*destino) = stack_pop(stak);
            ultimo_valor = (*destino);
            break;
        case i_top:
            (*destino) = (stak->topo)-1;
            ultimo_valor = (*destino);
            break;
        default:
            era_biop = 0;
            break;
    }

    if (era_biop) return pos+1;

    dlong* secundario = get_valor_source(&instrucoes[pos]->opB);

    int era_triop = 1;
    switch (instrucoes[pos]->tipo){
        case i_add:
            (*destino) = (*destino) + (*secundario);
            break;
        case i_sub:
            (*destino) = (*destino) - (*secundario);

            break;
        case i_mul:
            (*destino) = (*destino) * (*secundario);
            break;
        case i_div:
            (*destino) = (*destino) / (*secundario);
            break;
        case i_mod:
            (*destino) = (*destino) % (*secundario);
            break;
        case i_and:
            (*destino) = (*destino) & (*secundario);
            break;
        case i_or:
            (*destino) = (*destino) | (*secundario);
            break;
        case i_xor:
            (*destino) = (*destino) ^ (*secundario);
            break;
        case i_mov:
            (*destino) = (*secundario);
            break;
        case i_get:
            void* bytes = calloc(*secundario, sizeof(char));
            (*secundario) = (dlong)bytes;
            (*destino) = (*secundario);
            break;
        default:
            era_triop = 0;
            break;
    }

    if (era_triop) {
        ultimo_valor = (*destino);
        return pos+1;
    }

    eprintf("erro com quantidade de operadores em instrucao\n");
    return 67;
}

int main(){
    stak = novo_stack();
    instrucoes = malloc(50*sizeof(instrucao*));
    instrucoes_size = 50;

    FILE* f = fopen("../_/script.vm", "rb");
    readFile(f, &script_size, &script);


    size_t i = 0;
    while (i < script_size){
        if (script[i] >= i_add && script[i] <= i_get){
            i = processar_triop(i);
        }
        else if (script[i] >= i_not && script[i] <= i_top){
            i = processar_biop(i);
        }
        else if (script[i] == i_none){
            append_instrucao(NULL);
            i++;
        }
        else {
            eprintf("instrução desconhecida... pos[%d] n[%d]\n", i, script[i]);
        }
    }

    while (pos_instrucao < qtd_instrucao){
        pos_instrucao = executar_instrucao(pos_instrucao);

        // printf("\n[t:%d av:%lld bv:%lld]", 
        //     instrucoes[current_pos]->tipo,
        //     instrucoes[current_pos]->opA.valor,
        //     instrucoes[current_pos]->opB.valor
        // );
        // mostrar_memoria(0);
    }   
    mostrar_memoria(0);


    return 0;
}