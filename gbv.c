#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "util.h"
#include "gbv.h"

//Perguntar sobre o modo View
//função auxiliar para criar um documento


Document doc_create(const char *docname, long offset){

    // Document *doc = (Document *) malloc(sizeof(Document));
    // if(!doc){
    //     printf("Não foi possível alocar memória!\n");
    //     return NULL;
    // }


    // struct stat buffer;
    // stat(docname, &buffer);

    // strcpy(doc->name, docname);
    // doc->date = buffer.st_atime;
    // doc->size = buffer.st_size;
    // doc->offset = 0;

    // return doc;

    Document doc;

    struct stat buffer;
    stat(docname, &buffer);

    strcpy(doc.name, docname);
    doc.date = buffer.st_atime;
    doc.size = buffer.st_size;
    doc.date = buffer.st_atime;
    doc.offset = offset;

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

// cria a biblioteca se ela não  existir
int gbv_create(const char *filename){

    int quantidadeDocs = 0;
    int offset = 0;

    FILE *conteiner = fopen(filename, "w+b");
    if(!conteiner){
        printf("Não foi possível abrir o arquivo");
        return 2;
    }

    fwrite(&quantidadeDocs, sizeof(int), 1, conteiner);
        
    fwrite(&offset, sizeof(int), 1, conteiner);
}

// abre o arquivo da biblioteca
int gbv_open(Library *lib, const char *filename){
    if(!filename || !lib){
        printf("Parametros inválidos\n");
        return 1;
    }
 
    int quantidadeDocs = 0;
    int offset = 0;

    FILE *conteiner = fopen(filename, "r+b");
    if(!conteiner){
        if(gbv_create(filename) == 0){
            lib->docs = (Document *) malloc(sizeof(Document));
            lib->count = 0;
        }
    } else {
        printf("ja existe");
        rewind(conteiner);

        fread(&quantidadeDocs, sizeof(int), 1, conteiner);
        fread(&offset, sizeof(int), 1, conteiner);

        lib->count = quantidadeDocs;
    }

    return 0;
}

// adiciona o arquivo na biblioteca
int gbv_add(Library *lib, const char *archive, const char *docname){
    if(!lib || !archive || !docname)
        return 1;
    FILE *documento = fopen(docname, "r+b");
    if(!documento)
        return 2;

    FILE *conteiner;

    int quantidade, offset;
    fread(&quantidade, sizeof(int), 1, archive);
    fread(&offset, sizeof(int), 1, archive);

    offset += quantidade*sizeof(int);

    // ???
    Document doc = doc_create(docname, offset);
    


    lib->count++;
    

    return 0;

}

// remove o arquivo da biblioteca
int gbv_remove(Library *lib, const char *docname){
    if(!lib || !docname)
        return 1;

    return 0;
}

// lista os documentos da biblioteca
int gbv_list(const Library *lib){
    if(!lib)
        return 1;
    char buffer[BUFFER_SIZE];
    for(int i = 0; i<lib->count; i++){
        printf("documento: %s\n", lib->docs[i].name);
        format_date(lib->docs[i].date, buffer, BUFFER_SIZE);
        printf("data: %s\n", buffer);
        printf("tamanho: %ld\n", lib->docs[i].size);
        printf("offset: %ld\n", lib->docs[i].offset);
    }
    return 0;
}

//Visualiza os documentos segundo as especificações do enunciado
int gbv_view(const Library *lib, const char *docname){
    if(!lib || !docname)
        return 1;
    char buffer[BUFFER_SIZE];
    char aux[BUFFER_SIZE];

    return 0;
}
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