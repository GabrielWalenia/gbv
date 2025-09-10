#include <stdio.h>
#include <stdlib.h>
#include "stdlib.h"

#include "gbv.h"

void gbv_complete(FILE *arquivo, Library *lib){
    int bytes_lidos;
    int buffer;

    bytes_lidos = fread(&buffer, sizeof(int), 1, arquivo);
    if(bytes_lidos == 0){
        // variavel auxiliar para indicar o numero que se deve escrever no inicio de buffer
        lib->count = 0;
        buffer = 0;
        rewind(arquivo);
        fwrite(&buffer, sizeof(int), 1, arquivo);
    
    } else{
        lib->count = buffer;

    }
}

// cria a biblioteca virtual em memoria
int gbv_create(const char *filename){
    return 0;
}

// abre o arquivo da biblioteca
int gbv_open(Library *lib, const char *filename){
    if(!filename){
        return 1;
    }

    FILE *conteiner = fopen(filename, "w+b");
    lib->arquivo = conteiner;
    if(!conteiner){
        printf("Não foi possível abrir o arquivo");
        return 1;
    }

    gbv_complete(conteiner, lib);

    //lib->arquivo = conteiner;

    return 0;

}

// adiciona o arquivo na biblioteca
int gbv_add(Library *lib, const char *archive, const char *docname);

// remove o arquivo da biblioteca
int gbv_remove(Library *lib, const char *docname);

// lista os documentos da biblioteca
int gbv_list(const Library *lib){
    if(!lib)
        return 1;
    for(int i = 0; i<lib->count; i++)
        printf("documento: %s\n", lib->docs[i].name);
    return 0;
}

//??
int gbv_view(const Library *lib, const char *docname);
//ordena os arquivos da biblioteca
int gbv_order(Library *lib, const char *archive, const char *criteria){
    if(strcmp("nome", criteria)){
        return 1;
    }
    if(strcmp("data", criteria)){
        return 2;
    }
    if(strcmp("tamanho", criteria)){
        return 3;
    }
    return -1;
}