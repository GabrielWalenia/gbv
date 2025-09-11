#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "util.h"
#include "gbv.h"

//função auxiliar para criar um documento
Document *doc_create(const char *docname){

    Document *doc = (Document *) malloc(sizeof(Document));
    if(!doc){
        printf("Não foi possível alocar memória!\n");
        return NULL;
    }


    struct stat buffer;
    stat(docname, &buffer);

    strcpy(doc->name, docname);
    doc->date = buffer.st_atime;
    doc->size = buffer.st_size;
    doc->offset = 0;

    return doc;
}
// funcao auxiliar para abrir documentos
FILE *doc_open(const char *docname){
    if(!docname)
        return NULL;

    FILE *doc = fopen(docname, "r+b");
    if(!doc){
        printf("Não foi possível abrir o arquivo!\n");
        return NULL;
    }

    return doc;
}

void gbv_complete(FILE *arquivo, Library *lib){
    int buffer;

    fread(&buffer, sizeof(int), 1, arquivo);
    lib->count = buffer;
}

// cria a biblioteca virtual em memoria
// ??
int gbv_create(const char *filename){
    if(!filename){
        return 1;
    }
    // Library lib;
    // gbv_open(&lib, filename);
    // lib.docs = (Document *) malloc(lib.count*sizeof(Document));
    
    // int buffer;
    // fread(&buffer, sizeof(int), 1, lib.arquivo);
    // lib.count = buffer;

    return 0;
}

// abre o arquivo da biblioteca
int gbv_open(Library *lib, const char *filename){
    if(!filename || !lib){
        printf("Parametros inválidos\n");
        return 1;
    }

    int quantidadeDocs = 0;
    int offset = 2*sizeof(int);

    FILE *conteiner = fopen(filename, "r+b");
    if(!conteiner){
        conteiner = fopen(filename, "w+b");
        
        if(!conteiner){
            printf("Não foi possível abrir o arquivo");
            return 2;
        }

        rewind(conteiner);
        //numero de documentos na biblioteca
        
        fwrite(&quantidadeDocs, sizeof(int), 1, conteiner);
        // Offset da área de diretorio (da posição atual ou do inicio do arquivo?)
        // Estou considerando do inicio do arquivo
        
        fwrite(&offset, sizeof(int), 1, conteiner);

    } else {
        rewind(conteiner);

        fread(&quantidadeDocs, sizeof(int), 1, conteiner);
        fread(&offset, sizeof(int), 1, conteiner);

        lib->count = quantidadeDocs;

    }

    return 0;

}

// adiciona o arquivo na biblioteca
int gbv_add(Library *lib, const char *archive, const char *docname){
    

    return 1;
}

// remove o arquivo da biblioteca
int gbv_remove(Library *lib, const char *docname){

    return 1;
}

// lista os documentos da biblioteca
int gbv_list(const Library *lib){
    if(!lib)
        return 1;
    for(int i = 0; i<lib->count; i++)
        printf("documento: %s\n", lib->docs[i].name);
    return 0;
}

//Visualiza os documentos segundo as especificações do enunciado
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