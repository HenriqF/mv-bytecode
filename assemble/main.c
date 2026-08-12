#include "main.h"

typedef struct {
    size_t start;
    size_t end;
} line_def;

size_t script_size;
char* script;

size_t result_size;
size_t result_pos = 0;
char* result;


void result_append(unsigned char bytes[], size_t qtd){
    if (result_pos+qtd >= result_size){
        char* temp = realloc(result, result_size*2);
        if (temp == NULL){
            eprintf("DEU MERDA\n");
        }
        result = temp;
    }

    for (size_t i = 0; i < qtd; i++){
        result[result_pos++] = bytes[i];
    }
}

size_t script_lines_size;
size_t script_lines_qtd;
line_def** script_lines;

HashMap verbos = {50, 0, NULL};
HashMap labels = {50, 0, NULL};

void append_line(line_def* ld){
    if (script_lines_qtd >= script_lines_size){
        line_def** temp = realloc(script_lines, script_lines_size*2);
        if (temp == NULL){
            eprintf("DEU MERDA\n");
        }
        script_lines = temp;
    }

    while (script[ld->start] == ' '){
        ld->start++;
    }

    script_lines[script_lines_qtd++] = ld;
}

void get_lines(){
    size_t i = 0;
    size_t line_start = 0;

    int ret = 0;
    while (i < script_size){
        if (script[i] == '\r'){
            ret = 2;
        }
        if (script[i] == '\n'){
            if (ret < 0) ret = 0;

            line_def* nl = malloc(sizeof(line_def));
            nl->start = line_start;
            nl->end = i-ret;
            append_line(nl);

            line_start = i+1;
        }
        ret--;
        i++;
    }   
    line_def* nl = malloc(sizeof(line_def));
    nl->start = line_start;
    nl->end = i;
    append_line(nl);
}

size_t get_arg(size_t pos, size_t line_end, unsigned char bytes[]){
    size_t esp = pos;
    while (esp < line_end){
        if (script[esp] == ' '){
            break;
        }
        esp++;
    }

    short prefixo = 8;

    if (script[pos] == 'r'){ 
        prefixo += 20;
        pos++;
    }
    else if (script[pos] == 's'){
        prefixo += 40;
        pos++;
    }
    else if (script[pos] == 'h'){
        prefixo += 60;
        pos++;
    }

    size_t size = esp-pos;
    char* num = malloc((size+1)*sizeof(char));
    num[size] = 0;
    snprintf(num, size+1, "%s", script+pos);


    long long numero;
    if (script[pos] == '@'){
        int* result = getValue(labels, num+1);
        if (result == NULL) eprintf("LABEL NAO EXISTE, [%s]\n", num+1);

        numero = (long long) *result;
    }
    else{
        char* endptr;
        numero = strtoll(num, &endptr ,10);
        if (num == endptr) eprintf("deu merda convertendo numero [%s]", num);
    }

    memcpy(bytes, &prefixo, 1);
    memcpy(bytes+1, &numero, sizeof(long long));
        
    // printf("[ ");
    // for (size_t i = 0; i< sizeof(long long)+1; i++){
    //     printf("%d ", bytes[i]);
    // }
    // printf("]\n");

    return esp+1;
}

void process_lines(){    
    for (size_t i = 0; i < script_lines_qtd; i++){
        line_def* line = script_lines[i];

        size_t esp = line->start;
        while (esp < line->end){
            if (script[esp] == ' '){
                break;
            }
            esp++;
        }
        size_t size = esp-line->start;

        if (size <= 0) continue;
        if (script[line->start] == '#') continue;
        if (script[line->end-1] == ':') {
            unsigned char byte[1] = {0};
            result_append(byte, 1);
            continue;
        }

        char* verb = malloc((size+1)*sizeof(char));
        verb[size] = 0;
        snprintf(verb, size+1, "%s", script+line->start);

        int* index = getValue(verbos, verb);
        if (index == NULL) eprintf("PALAVRA CHAVE NAO EXISTE");

        unsigned char instruct[] = {(char)(*index)};
        unsigned char bytesA[sizeof(long long)+1];
        unsigned char bytesB[sizeof(long long)+1];

        // printf("%d\n", (*index));
        if ((*index) == i_none || ((*index) >= i_return && (*index) <= i_debug) ) {
            result_append(instruct, 1);
        }
        else if ((*index) >= i_add && (*index) <= i_cmp){
            size_t argp = line->start + size+1;
            argp = get_arg(argp, line->end, bytesA);
            get_arg(argp, line->end, bytesB);

            result_append(instruct, 1);
            result_append(bytesA, sizeof(long long)+1);
            result_append(bytesB, sizeof(long long)+1);
        }
        else if ((*index) >= i_not && (*index) <= i_top){
            size_t argp = line->start + size+1;
            get_arg(argp, line->end, bytesA);

            result_append(instruct, 1);
            result_append(bytesA, sizeof(long long)+1);
        }

        free(verb);
    }
}   

void process_labels(){
    initHashmap(&labels);

    size_t linha_verdade = 0;
    for (size_t i = 0; i < script_lines_qtd; i++){
        line_def* line = script_lines[i];

        size_t esp = line->start;
        while (esp < line->end){
            if (script[esp] == ' '){
                break;
            }
            esp++;
        }

        size_t size = esp-line->start;
        if (size == 0){
            continue;
        }
        if (script[line->end-1] != ':') {
            linha_verdade++;
            continue;
        }

        char* label = malloc((size+1)*sizeof(char));
        label[size-1] = 0;
        snprintf(label, size, "%s", script+line->start);
        
        if (size < 2) eprintf("NOME DE LABEL MUITO CURTO [%s]\n", label);

        setKey(&labels, label, (int)(linha_verdade++));
    }
}

void init_verbos_hash(){
    initHashmap(&verbos);

    struct {
        char* key;
        int value;
    } items[] = {
        {"none", 0},
        {"add", 1},
        {"sub", 2},
        {"mul", 3},
        {"div", 4},
        {"mod", 5},
        {"and", 6},
        {"or", 7},
        {"xor", 8},
        {"mov", 9},
        {"get", 10},
        {"cmp", 11},
        {"not", 12},
        {"jzero", 13}, {"jequal", 13},
        {"jnzero", 14}, {"jdiff", 14},
        {"jeven", 15}, 
        {"jodd", 16},
        {"jpos", 17}, {"jbigger", 17},
        {"jneg", 18}, {"jsmaller", 18},
        {"jzneg", 19}, {"jeqsmaller", 19},
        {"jzpos", 20}, {"jeqbigger", 20},
        {"jmp", 21}, {"goto", 21},
        {"call", 22},
        {"push", 23},
        {"free", 24},
        {"pop", 25},
        {"top", 26},
        {"return", 27},
        {"debug", 28},
        {"fim", 29}
    };

    for (size_t i = 0; i < sizeof(items) / sizeof(items[0]); i++) {
        setKey(&verbos, items[i].key, items[i].value);
    }
    return;
}

int main(){
    init_verbos_hash();

    script_lines = malloc(50*sizeof(line_def*));
    script_lines_size = 50;

    FILE* f = fopen("../_/script.asm", "rb");
    readFile(f, &script_size, &script);
    fclose(f);
    
    result = malloc(2*script_size);
    result_size = 2*script_size;

    get_lines();
    process_labels();
    process_lines();

    printf("\n(%4zu)[ ", result_pos);
    for (size_t i = 0; i < result_pos; i++){
        printf("%d ", result[i]);
    }
    printf("]\n");

    f = fopen("../_/script.vm", "wb");
    writeFileN(f, result, result_pos);
    fclose(f);
    return 0;
}