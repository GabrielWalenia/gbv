#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gbv.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <opção> <biblioteca> [documentos...]\n", argv[0]);
        return 1;
    }

    const char *opcao = argv[1];
    const char *biblioteca = argv[2];

    Library lib;
    if (gbv_open(&lib, biblioteca) != 0) {
        printf("Erro ao abrir biblioteca %s\n", biblioteca);
        return 1;
    }
    
    if (strcmp(opcao, "-a") == 0) {
        for (int i = 3; i < argc; i++) {
            gbv_add(&lib, biblioteca, argv[i]);
        }
    }

    gbv_list(&lib);

    gbv_remove(&lib, argv[3]);
    
    gbv_list(&lib);

    printf("ahjdsfhihfohfa\n");

    return 0;
}