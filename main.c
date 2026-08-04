#include "main.h"

void mostrar_registros(){
    for (int i = 0 ; i < REG_COUNT; i++){
        if (i % 5 == 0 && i != 0){
            printf("\n");
        }
        printf("%2d[%16lld]  ", i, regs[i]);
    }
    printf("\n");
}

int get_operando_info(int pos, operando* op){
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
    int limit = pos+op->tamanho;

    if ((size_t)limit > script_size){
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

dlong* decode_valor(operando* op){
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

int processar_triop(int pos){
    instrucao inst = script[pos];

    pos = get_operando_info(pos+1, &opA);
    pos = get_operando_info(pos, &opB);

    if (opA.tipo == t_absoluto){
        eprintf("destino de operacação nao pode ser valor absoluto... \n");
    };

    dlong* destino = decode_valor(&opA);
    dlong* secundario = decode_valor(&opB);

    // printf("A:%lld, %d, %d    ", opA.valor, opA.tamanho, opA.tipo);
    // printf("B:%lld, %d, %d\n", opB.valor, opB.tamanho, opB.tipo);

    switch (inst){
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
            void* bytes = malloc(*secundario);
            (*secundario) = (dlong)bytes;
            (*destino) = (*secundario);
            break;

        case i_push:
            (*destino) = stack_add(stak, (*secundario));
            break;
        
        case i_pop:
            (*destino) = stack_pop(stak);
            (*secundario) = stak->topo-1;
            break;

        default:
            eprintf("operacao desconheciada... \n");
            break;
    }
    ultimo_valor = *destino;

    return pos;
}

int processar_biop(int pos){
    instrucao inst = script[pos];
    
    pos = get_operando_info(pos+1, &opA);
    if (opA.tipo == t_absoluto){
        eprintf("destino de operacação nao pode ser valor absoluto... \n");
    };

    dlong* destino = decode_valor(&opA);

    switch (inst){
        case i_not:
            break;
        case i_jeq:
            break;
        case i_jlt:
            break;
        case i_jle:
            break;
        case i_jgt:
            break;
        case i_jge:
            break;
        case i_jev:
            break;
        case i_jod:
            break;
        case i_jmp:
            break;
        case i_call:
            break;
        case i_return:
            break;
        case i_free:
            free((void*)(*destino));
            (*destino) = 0;
            break;
        default:
            eprintf("operacao desconheciada... \n");
            break;
    }

    ultimo_valor = *destino;
    return pos;
}

int main(){
    stak = novo_stack();

    FILE* f = fopen("_/script.vm", "rb");
    readFile(f, &script_size, &script);

    size_t i = 0;
    while (i < script_size){
        if (script[i] >= i_add && script[i] <= i_pop){
            i = processar_triop(i);
        }
        else if (script[i] >= i_not && script[i] <= i_free){
            i = processar_biop(i);
        }
        else {
            eprintf("instrução desconhecida... pos[%d] n[%d]\n", i, script[i]);
        }
        mostrar_registros();
        mostrar_stack(stak);

        printf("\n");
    }

    return 0;
}