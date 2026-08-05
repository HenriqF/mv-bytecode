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
    size_t nt = 2*instrucoes_size;
    instrucao** temp = realloc(instrucoes, nt*sizeof(instrucao*));
    if (temp == NULL){
        eprintf("erro ao criar espaco para novas instrucoes\n");
    }

    instrucoes = temp;
    instrucoes_size = nt;
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
    if (instrucoes[pos]->opq == 0) return pos+1;

    dlong* destino = get_valor_source(&instrucoes[pos]->opA);

    if (instrucoes[pos]->opq == 1){
        switch(instrucoes[pos]->tipo){
            case i_not:
                (*destino) = ~(*destino);
                ultimo_valor = (*destino);
                break;
            case i_jzero:
                if (ultimo_valor == 0) return (size_t)(*destino);
                return pos+1;
            case i_jnzero:
                if (ultimo_valor != 0) return (size_t)(*destino);
                return pos+1;
            case i_jeven:
                if ((ultimo_valor & 1) != 0) return (size_t)(*destino);
                return pos+1;
            case i_jodd:
                if ((ultimo_valor & 1) == 0) return (size_t)(*destino);
                return pos+1;
            case i_jpos:
                if (ultimo_valor > 0) return (size_t)(*destino);
                return pos+1;
            case i_jneg:
                if (ultimo_valor < 0) return (size_t)(*destino);
                return pos+1;
            case i_jzpos:
                if (ultimo_valor >= 0) return (size_t)(*destino);
                return pos+1;
            case i_jzneg:
                if (ultimo_valor <= 0) return (size_t)(*destino);
                return pos+1;
            case i_jmp:
                return (size_t)(*destino);
            case i_call:
                stack_add(stak, pos);
                return (size_t)(*destino);

                //return siop
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
                eprintf("erro com operacao biop");
                break;
        }
   
        return pos+1;
    }

    if (instrucoes[pos]->opq == 2){
        dlong* secundario = get_valor_source(&instrucoes[pos]->opB);
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
                eprintf("erro com operacao triop");
                break;
        }
        
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
        // size_t current_pos = pos_instrucao;
        pos_instrucao = executar_instrucao(pos_instrucao);


        // printf("\n[t:%d av:%lld bv:%lld qtd:%d]", 
        //     instrucoes[current_pos]->tipo,
        //     instrucoes[current_pos]->opA.valor,
        //     instrucoes[current_pos]->opB.valor,
        //     instrucoes[current_pos]->opq
        // );
        // mostrar_memoria(0);
    }   
    mostrar_memoria(0);

    return 0;
}