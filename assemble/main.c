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

    char* endptr;
    long long numero = strtoll(num, &endptr ,10);
    if (num == endptr) eprintf("deu merda convertendo numero [%s]", num);

    memcpy(bytes+1, &numero, sizeof(long long));
    memcpy(bytes, &prefixo, 1);
        
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

        if (line->end - line->start == 0) continue;
        if (script[line->start] == '#') continue;

        size_t esp = line->start;
        while (esp < line->end){
            if (script[esp] == ' '){
                break;
            }
            esp++;
        }
        size_t size = esp-line->start;

        char* verb = malloc((size+1)*sizeof(char));
        verb[size] = 0;
        snprintf(verb, size+1, "%s", script+line->start);

        int* index = getValue(verbos, verb);
        if (index == NULL) eprintf("PALAVRA CHAVE NAO EXISTE");


        unsigned char instruct[] = {(char)(*index)};
        unsigned char bytesA[sizeof(long long)+1];
        unsigned char bytesB[sizeof(long long)+1];

        if (*index >= i_add && *index <= i_get){
            size_t argp = line->start + size+1;
            argp = get_arg(argp, line->end, bytesA);
            get_arg(argp, line->end, bytesB);

            result_append(instruct, 1);
            result_append(bytesA, sizeof(long long)+1);
            result_append(bytesB, sizeof(long long)+1);
        }
        else if (*index >= i_not && *index <= i_top){
            size_t argp = line->start + size+1;
            get_arg(argp, line->end, bytesA);

            result_append(instruct, 1);
            result_append(bytesA, sizeof(long long)+1);
        }
        else if (*index == i_none) {
            result_append(instruct, 1);
        }

        free(verb);
    }
}   

void init_verbos_hash(){
    initHashmap(&verbos);

    struct {
        char* key;
        int value;
    } items[] = 
    {
        {"none", 0}, {"add", 1}, {"sub", 2},  {"mul", 3},
        {"div", 4},  {"mod", 5}, {"and", 6},  {"or", 7},
        {"xor", 8},  {"mov", 9}, {"get", 10}, {"not", 11},

        {"jzero", 12}, {"jnzero", 13},{"jeven", 14}, {"jodd", 15},
        {"jpos", 16}, {"jneg", 17},{"jzneg", 18},{"jzpos", 19},
        {"jmp", 20},
        
        {"call", 21},{"push", 22},
        {"free", 23},{"pop", 24},{"top", 25}, {"return", 26}
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