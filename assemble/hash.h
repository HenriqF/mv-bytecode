#ifndef HASH
#define HASH

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef struct HashItem{
    char* key;
    int value;

    struct HashItem* next;
} HashItem;

typedef struct HashMap{
    size_t buckets;
    size_t pairs;
    HashItem* HashItems;
} HashMap;



void initHashmap(HashMap* HashMap);

void setKey(HashMap* HashMap, char* key, int value);
void removeKey(HashMap* HashMap, char* key);
int* getValue(HashMap HashMap, char* key);
void search(HashMap HashMap, char* key);

#endif