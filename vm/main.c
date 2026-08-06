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

size_t processar_siop(size_t pos){
    instrucao* nova_instrucao = malloc(sizeof(instrucao));

    tipo_instrucao tipo = script[pos];
    nova_instrucao->tipo = tipo;
    nova_instrucao->opq = 0;

    append_instrucao(nova_instrucao);
    return pos+1;
}



dlong* get_valor_stackheap(operando* op){
    if (op->tipo == t_stack){
        dlong pos = regs[op->valor]; 

        if (pos < 0 || pos >= (dlong)stak->topo) {
            eprintf("stack [%lld] inexistente...\n", pos);
        }

        return &stak->itens[pos];
    }
    if (op->tipo == t_heap){
        return NULL;
    }

    eprintf("tipo de valor inexistente");
    return NULL;
}

static inline dlong* get_valor_source(operando* op){
    if (op->tipo == t_absoluto) return &op->valor;

    if (op->valor < 0 || op->valor >= REG_COUNT){
        eprintf("registro [%lld] inexistente...\n", op->valor);
    }

    if (op->tipo == t_registro) return &regs[op->valor];

    return get_valor_stackheap(op);
}

size_t executar_instrucao(size_t pos){
    instrucao* inst = instrucoes[pos];

    if (inst->opq == 0) {
        if (inst->tipo == i_return){
            return stack_pop(stak)+1;
        }
        if (inst->tipo == i_debug){
            mostrar_memoria(0);
        }

        return pos+1;
    }

    dlong* destino = get_valor_source(&inst->opA);

    if (inst->opq == 1){
        int jump = 0;

        switch(inst->tipo){
            case i_not:
                (*destino) = ~(*destino);
                ultimo_valor = (*destino);
                break;

            case i_jzero: jump = (ultimo_valor == 0); break;

            case i_jnzero: jump = (ultimo_valor != 0); break;

            case i_jeven: jump = ((ultimo_valor & 1) != 0); break;

            case i_jodd: jump = ((ultimo_valor & 1) == 0); break;

            case i_jpos: jump = (ultimo_valor > 0); break;

            case i_jneg: jump = (ultimo_valor < 0); break;

            case i_jzpos: jump = (ultimo_valor >= 0); break;

            case i_jzneg: jump = (ultimo_valor <= 0); break;

            case i_jmp: jump = 1; break;

            case i_call:
                stack_add(stak, pos);
                return (size_t)(*destino);

            case i_push:
                stack_add(stak, (*destino));
                break;
            case i_free:
                free((void*)(*destino));
                (*destino) = 0;
                break;
            case i_pop:
                (*destino) = stack_pop(stak);
                break;
            case i_top:
                (*destino) = (stak->topo)-1;
                break;
            default:
                eprintf("erro com operacao biop");
                break;
        }
   
        if (jump){
            return (size_t)(*destino);
        }

        return pos+1;
    }

    if (inst->opq == 2){
        dlong* secundario = get_valor_source(&inst->opB);
        switch (inst->tipo){
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

void executar_programa(){
    while (pos_instrucao < qtd_instrucao){
        //size_t current_pos = pos_instrucao;
        pos_instrucao = executar_instrucao(pos_instrucao);

        // printf("\n[t:%d av:%lld bv:%lld qtd:%d]", 
        //     instrucoes[current_pos]->tipo,
        //     instrucoes[current_pos]->opA.valor,
        //     instrucoes[current_pos]->opB.valor,
        //     instrucoes[current_pos]->opq
        // );
        //mostrar_memoria(current_pos);
    }   
}

int main(){
    stak = novo_stack();

    instrucoes = malloc(2000*sizeof(instrucao*));
    instrucoes_size = 2000;

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
        else if (script[i] >= i_return && script[i] <= i_debug){
            i = processar_siop(i);
        } 
        else if (script[i] == i_none){
            append_instrucao(NULL);
            i++;
        }
        else {
            eprintf("instrução desconhecida... pos[%d] n[%d]\n", i, script[i]);
        }
    }

    executar_programa();

    return 0;
}