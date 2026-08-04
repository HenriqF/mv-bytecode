
#ifndef UTIL_H
#define UTIL_H

    #include <stddef.h>
    #include <stdio.h>

    #ifdef UTIL_STRING
        int endsWith(char* a, char* b);
        int startsWith(char* a, char* b);
    #endif

    #ifdef UTIL_FILE
        int readFile(FILE* f, size_t* size, char** content);
        int writeFile(FILE* f, char* content);
    #endif

    #ifdef UTIL_DEBUG
        void eprintf(const char *fmt, ...);
    #endif



    #ifdef UTIL_IMP
        #include <stdio.h>

        #ifdef UTIL_STRING
            #include <string.h>
            int endsWith(char* a, char* b){
                if (!a || !b) return 0;

                size_t la = strlen(a);
                size_t lb = strlen(b);

                if (lb > la) return 0;

                return memcmp(a+(la-lb), b, lb) == 0;
            }
            int startsWith(char* a, char* b){
                if (!a || !b) return 0;

                size_t la = strlen(a);
                size_t lb = strlen(b);

                if (lb > la) return 0;

                return memcmp(a, b, lb) == 0;
            }
        #endif

        #ifdef UTIL_FILE
            #include <stdlib.h>
            int readFile(FILE* f, size_t* size, char** content){
                if (!f || !size || !content) return -1;

                if (fseek(f, 0, SEEK_END) != 0) return -1;
                long file_size = ftell(f);
                if (file_size < 0) return -1;

                *size = (size_t)file_size;
                if (fseek(f, 0, SEEK_SET) != 0) return -1;

                *content = malloc(*size + 1);
                if (!*content) return -1;

                size_t nread = fread(*content, 1, *size, f);
                if (nread != *size){ free(*content); return -1; }

                (*content)[*size] = '\0';
                return 0;
            }
            int writeFile(FILE* f, char* content){
                if (!f) return -1;

                if(fprintf(f, "%s", content) < 0){
                    return -1;
                }

                return 0;
            }
        #endif

        #ifdef UTIL_DEBUG
            #include <stdarg.h>
            #include <stdlib.h>
            void eprintf(const char *fmt, ...){
                va_list args;
                va_start(args, fmt);
                vprintf(fmt, args);
                va_end(args);
                exit(0);
            }
        #endif
    #endif


#endif