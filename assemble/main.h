#include <stdio.h>
#include <math.h>
#include "hash.h"

#define UTIL_FILE
#define UTIL_STRING
#define UTIL_DEBUG
#define UTIL_IMP
#include "../util.h"
#include "../tipo.h"


typedef enum {
    t_absoluto,
    t_registro,
    t_stack,
    t_heap,
} valor_tipo;