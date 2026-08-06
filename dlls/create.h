#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64)
    #define externo __declspec(dllexport)
#else
    #define externo __attribute__((visibility("default")))
#endif