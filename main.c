#include <stdio.h>
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
    int buffer = 0;
    fwrite(&buffer, sizeof(int), 1, lib.arquivo);
    rewind(lib.arquivo);
    
    fread(&buffer, sizeof(int), 1, lib.arquivo);
    rewind(lib.arquivo);
    printf("elementos: %d\n", buffer);
    buffer++;
    fwrite(&buffer, sizeof(int), 1, lib.arquivo);
    rewind(lib.arquivo);
    return 0;
}