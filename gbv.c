#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "util.h"
#include "gbv.h"

//Perguntar sobre o modo View
//Perguntar sobre o segmentation fault
//Perguntar sobre o offset

//função auxiliar para criar um documento
Document doc_create(const char *docname, long offset){

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

// cria a biblioteca se ela não  existir
int gbv_create(const char *filename){

    int quantidadeDocs = 0;
    long offset = 0;

    FILE *conteiner = fopen(filename, "w+b");
    if(!conteiner){
        printf("Não foi possível abrir o arquivo");
        return 2;
    }

    fwrite(&quantidadeDocs, sizeof(int), 1, conteiner);
        
    fwrite(&offset, sizeof(long), 1, conteiner);

    return 0;
}

// abre o arquivo da biblioteca
int gbv_open(Library *lib, const char *filename){
    if(!filename || !lib){
        printf("Parametros inválidos\n");
        return 1;
    }
 
    int quantidadeDocs = 0;
    long offset = 0;

    FILE *conteiner = fopen(filename, "r+b");
    if(!conteiner){
        if(gbv_create(filename) == 0){
            lib->docs = (Document *) malloc(sizeof(Document));
            lib->count = 0;
        }else{
            return 2;
        }
    } else {
        // printf("ja existe");
        rewind(conteiner);

        fread(&quantidadeDocs, sizeof(int), 1, conteiner);
        fread(&offset, sizeof(long), 1, conteiner);

        lib->count = quantidadeDocs;
    }

    return 0;
}

// adiciona o arquivo na biblioteca
int gbv_add(Library *lib, const char *archive, const char *docname){
    if(!lib || !archive || !docname){
        printf("Paramêtros inválidos");
        return 1;
    }

    FILE *documento = fopen(docname, "r+b");
    FILE *biblioteca = fopen(archive, "r+b");

    if(!documento || !biblioteca){
        printf("Não foi possível abrir o arquivo!\n");
        return 2;
    }

    Document doc;
    long soma = 0;

    if(lib->count == 0)
        doc = doc_create(docname, sizeof(int) + sizeof(long));
    else{
        // e se o arquivo aberto pela primeira vez já tiver documentos?
        for(int i = 0; i<lib->count; i++){
            soma+= lib->docs[i].size;
        }
        // pula a quantidade, o offset e o total dos tamanhos dos arquivos
        doc = doc_create(docname, sizeof(int) + sizeof(long)+soma);
    }

    lib->docs[lib->count] = doc;
    
    char *buffer = (char *)malloc(BUFFER_SIZE);
    char *aux = (char *) malloc(BUFFER_SIZE);
    //se a biblioteca está vazia, cria a area de dados e o diretorio
    if(lib->count == 0){

        fseek(biblioteca, sizeof(int) + sizeof(long), SEEK_SET);

        while(!feof(documento)){
            fread(buffer, 1, BUFFER_SIZE, documento);
            fwrite(buffer, 1, BUFFER_SIZE, biblioteca);
        }

        
        fseek(biblioteca, 0, SEEK_END);

        fwrite(&(lib->docs[0]), sizeof(Document), 1, biblioteca);

    } else{
        
        // Aqui tem um segmentation fault
        fseek(biblioteca, sizeof(int)+sizeof(long)+soma, SEEK_SET);
        
        printf("%ld", ftell(biblioteca));
        //carregando para dentro de aux o vetor dos documentos
        fread(aux, lib->count*sizeof(Document), 1, biblioteca);

        fseek(biblioteca, sizeof(int) + sizeof(long)+soma, SEEK_SET);
        while(!feof(documento)){
            fread(buffer, 1, BUFFER_SIZE, documento);
            fwrite(buffer, 1, BUFFER_SIZE, biblioteca);
        }

        fwrite(aux, lib->count*sizeof(Document),1, biblioteca);
        fwrite(&(lib->docs[lib->count]), sizeof(Document), 1, biblioteca);
    }

    free(buffer);
    free(aux);

    lib->count++;

    int quantidadeDocs = lib->count;
    int offset = sizeof(int) + sizeof(long) + lib->docs[lib->count-1].size;
    
    fseek(biblioteca, 0, SEEK_SET);
    fwrite(&quantidadeDocs, sizeof(int), 1, biblioteca);
    fwrite(&offset, sizeof(long), 1, biblioteca);

    fclose(documento);
    fclose(biblioteca);

    return 0;

}

// remove o arquivo da biblioteca
int gbv_remove(Library *lib, const char *docname){
    if(!lib || !docname){
        printf("Paramêtros inválidos");
        return 1;
    }
    if(lib->count <= 0){
        printf("Biblioteca vazia!");
        return 2;
    }

    //acha o documento que precisa ser retirado
    int i = 0;
    while(strcmp(lib->docs[i].name, docname)!=0){
        i++;
        if(i>lib->count){
            printf("Documento não existe na biblioteca");
            return 3;
        }
    }

    int j = i;
    while(j<lib->count){
        lib->docs[j] = lib->docs[j+1];
    }
    
    lib->count--;
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
    
    FILE *documento = fopen(docname, "r+b");
    if(!documento){
        printf("Não foi possível abrir o documento");
        return 2;
    }

    int i=0;
    while(strcmp(lib->docs[i].name, docname) != 0)
        i++;
    
    char *buffer = NULL;
    // char aux[BUFFER_SIZE];

    fread(buffer, 1, BUFFER_SIZE, documento);
    printf("%s", buffer);

    while(!feof(documento)){
        fread(buffer, 1, BUFFER_SIZE, documento);
        printf("%s", buffer);
    }

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