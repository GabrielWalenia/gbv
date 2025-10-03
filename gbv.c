#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "util.h"
#include "gbv.h"


//Perguntar se pode colocar uma struct representando o documento na memória 
//Perguntar sobre a função fread e fwrite


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

int gbv_close(FILE *arquivo){
    fclose(arquivo);
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
            if(!lib->docs){
                printf("Falha ao alocar memória\n");
                return 3;
            }
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

    // fclose(conteiner);

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
        doc = doc_create(docname, sizeof(int) + sizeof(long) + soma);
    }


    if(lib->count>0){
        lib->docs = realloc(lib->docs, (1+lib->count)*sizeof(Document));
    }
    lib->docs[lib->count] = doc;
    
    char *buffer = (char *)malloc(BUFFER_SIZE);
    char *aux = (char *) malloc(BUFFER_SIZE);
    int bytes = 0;


    //se a biblioteca está vazia, cria a area de dados e o diretorio
    if(lib->count == 0){

        fseek(biblioteca, sizeof(int) + sizeof(long), SEEK_SET);

        while(!feof(documento)){
            bytes = fread(buffer, 1, BUFFER_SIZE, documento);
            fwrite(buffer, 1, bytes, biblioteca);
        }

        fwrite(&(lib->docs[0]), sizeof(Document), 1, biblioteca);

    } else{

        fseek(biblioteca, -(lib->count * sizeof(Document)), SEEK_END);

        //carregando para dentro de aux o vetor dos documentos
        fread(aux, sizeof(Document), lib->count, biblioteca);

        fseek(biblioteca, -(lib->count * sizeof(Document)), SEEK_END);
        while(!feof(documento)){
            bytes = fread(buffer, 1, BUFFER_SIZE, documento);
            fwrite(buffer, 1, bytes, biblioteca);
        }

        fwrite(aux, sizeof(Document),lib->count, biblioteca);

        fwrite(&(lib->docs[lib->count]), sizeof(Document), 1, biblioteca);
    }

    free(buffer);
    free(aux);

    lib->count++;

    int quantidadeDocs = lib->count;
    int offset = sizeof(int) + sizeof(long) + soma + lib->docs[lib->count].size;

    rewind(biblioteca);

    fwrite(&quantidadeDocs, sizeof(int), 1, biblioteca);
    fwrite(&offset, sizeof(long), 1, biblioteca);

    return 0;

}

// remove o arquivo da biblioteca
int gbv_remove(Library *lib, const char *docname){
    if(!lib || !docname){
        printf("Parâmetros inválidos!\n");
        return 1;
    }

    if(lib->count <= 0){
        printf("Biblioteca vazia!\n");
        return 2;
    }

    // itera pelo vetor de documentos para achar um que tenha o mesmo nome
    int i;
    for(i = 0; i < lib->count ; i++){
        //se o nome é igual, sai do loop
        if(strcmp(lib->docs[i].name, docname) == 0){
            break;
        }
    }

    if(i>=lib->count){
        printf("Documento não existe no vetor!\n");
        return 3;
    }

    if(lib->count > 1){
        
        Document doc = lib->docs[i];
        lib->docs[i] = lib->docs[lib->count-1];
        lib->docs[lib->count-1] = doc;
        
    }

    lib->count--;
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