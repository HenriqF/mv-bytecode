#include <windows.h>
#include <stdint.h>
#include <stdio.h>

typedef int (*aura)(int numero);

int main(){
    HMODULE hDll = LoadLibrary("create.dll");
    if (hDll == NULL);

    aura funcao = (aura)GetProcAddress(hDll, "aura");
    if (funcao == NULL);

    int res = funcao(67);
    printf("resposta: [%d]", res);

    FreeLibrary(hDll);
}