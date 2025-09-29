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
    
    if (strcmp(opcao, "-a") == 0) {
        for (int i = 3; i < argc; i++) {
            gbv_add(&lib, biblioteca, argv[i]);
        }
    }

    //gbv_list(&lib);

    //for (int i = 3; i < argc; i++) {
    //    gbv_remove(&lib, argv[i]);
    //}
    // int buffer;
    // rewind(lib.arquivo);
    // fread(&buffer, sizeof(int), 1, lib.arquivo);
    // rewind(lib.arquivo);
    // buffer = buffer+1;
    // fwrite(&buffer, sizeof(int), 1, lib.arquivo);
    // rewind(lib.arquivo);
    // int aux;
    // fread(&aux, sizeof(int), 1, lib.arquivo);
    // printf("A quantidade de arquivos é %d\n", aux);

    return 0;
}