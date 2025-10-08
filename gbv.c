#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "util.h"
#include "gbv.h"

const char *gbv;

//Perguntar se pode colocar uma struct representando o documento na memória 
//Perguntar sobre a função fread e fwrite


//função auxiliar para criar um documento

int mover(Library *lib, FILE  *arquivo, size_t inicio, size_t fim, long deslocamento){
    if(!arquivo){
        printf("ERRO!\n");
        exit(1);
    }
    printf("%lu\n", inicio);
    printf("%lu\n", fim);
    char *buffer = (char *) malloc(BUFFER_SIZE); 
    if(!buffer){
        printf("Não foi possível alocar memória!");
        exit(1);
    }

    size_t tam = fim - inicio;
    size_t bytes = tam;
    size_t bloco = 0;
    size_t lidos = 0;

    if(bytes > BUFFER_SIZE){
        bloco = BUFFER_SIZE;
    } else {
        bloco = bytes;
    }

    while(bytes>0){

        if(deslocamento == 0){
            printf("Nada a ser feito!\n");
            return 0;
        } else if(deslocamento > 0){
            
            fseek(arquivo, inicio, SEEK_SET);
            lidos = fread(buffer, 1, bloco, arquivo);
            fseek(arquivo, inicio, SEEK_SET);
            
            fseek(arquivo, deslocamento, SEEK_CUR);
            fwrite(buffer, 1, bloco, arquivo);
            bytes -= lidos;

        }else{

            fseek(arquivo, inicio, SEEK_SET);
            lidos = fread(buffer, 1, bloco, arquivo);
            fseek(arquivo, inicio, SEEK_SET);
            
            fseek(arquivo, deslocamento, SEEK_CUR);
            fwrite(buffer, 1, bloco, arquivo);
            bytes -= lidos;

        }

    }
    free(buffer);
    return 0;
}


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

    FILE *biblioteca = fopen(filename, "w+b");
    if(!biblioteca){
        printf("Não foi possível abrir o arquivo\n");
        return 2;
    }

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

    gbv = filename;
        
    FILE *biblioteca = fopen(filename, "r+b");
    // Se a biblioteca não foi aberta, significa que não há biblioteca
    if(!biblioteca){
        // tenta criar biblioteca
        if(gbv_create(filename) == 0){
            //sucesso
            lib->count = 0;

            lib->docs = (Document *) malloc(sizeof(Document));
            if(!lib->docs){
                printf("Falha ao alocar memória\n");
                return 3;
            }
        }else{
            //erro
            printf("Não foi possível criar a biblioteca!\n");
            return 2;
        }

    } else {
        // printf("shdiapfhdsaihf\n");
        fseek(biblioteca, 0, SEEK_SET);

        fread(&quantidadeDocs, sizeof(int), 1, biblioteca);
        fread(&offset, sizeof(long), 1, biblioteca);

        lib->count = quantidadeDocs;
        
       
        lib->docs = (Document *) malloc(lib->count*sizeof(Document));
        if(!lib->docs){
            printf("Não foi possível alocar memória!\n");
            return 3;
        }

        if(lib->count > 0){
            fseek(biblioteca, -(lib->count * sizeof(Document)), SEEK_END);
            fread(lib->docs, sizeof(Document), quantidadeDocs, biblioteca);

        }
        
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

    rewind(documento);

    bool existeDocumento = false;
    int cont;
     
    // printf("%d\n", lib->count);
    // printf("%s\n", lib->docs[0].name);

    // Verifica se o documento já existe na biblioteca
    for(cont = 0; cont<lib->count; cont++){
        if(strcmp(lib->docs[cont].name, docname) == 0){
            existeDocumento = true;
            break;
        }
    }

    Document doc;
    long soma = 0;

    if(lib->count == 0)
        doc = doc_create(docname, sizeof(int) + sizeof(long));
    else{
        if(!existeDocumento){
            // e se o arquivo aberto pela primeira vez já tiver documentos?
            for(int i = 0; i<lib->count; i++){
                soma+= lib->docs[i].size;
            }
            // pula a quantidade, o offset e o total dos tamanhos dos arquivos
            doc = doc_create(docname, sizeof(int) + sizeof(long) + soma);
        } else {
            doc = doc_create(docname, lib->docs[cont].offset);
        }
    }


    if(lib->count>0 && !existeDocumento){
        lib->docs = realloc(lib->docs, (1+lib->count)*sizeof(Document));
        if(!lib->docs){
            printf("Não foi possível alocar memória!\n");
            return 3;
        }
    }

        
    char *buffer = (char *)malloc(BUFFER_SIZE);
    if(!buffer){
        printf("Não foi possível alocar memória!\n");
        return 3;
    }

    int bytes = 0;


    //se a biblioteca está vazia, cria a area de dados e o diretorio
    if(lib->count == 0){
        printf("Entrou aqui\n");

        lib->docs[lib->count] = doc;

        fseek(biblioteca, sizeof(int) + sizeof(long), SEEK_SET);

        while(!feof(documento)){
            bytes = fread(buffer, 1, BUFFER_SIZE, documento);
            fwrite(buffer, 1, bytes, biblioteca);
        }

        fwrite(&(lib->docs[0]), sizeof(Document), 1, biblioteca);

    } else{

        Document *aux = (Document *) malloc(lib->count * BUFFER_SIZE);
        if(!aux){
            printf("Não foi possível alocar memória!\n");
            return 3;
        }

        // Se existe o documento, este é substituido
        if(existeDocumento){
            printf("Entrou no if\n");
            fseek(biblioteca, -(lib->count * sizeof(Document)), SEEK_END);
            int offsetDocumento = 0;
            int offsetDiretorio = 0;
            //carregando para dentro de aux o vetor dos documentos
            fread(aux, sizeof(Document), lib->count, biblioteca);

            for(int j = 0; j<lib->count; j++){
                // substitui a struct antiga pela nova
                if(cont == j){
                    lib->docs[j] = doc;
                    offsetDocumento = doc.offset;
                } else{
                    lib->docs[j] = aux[j];
                }
            }

            fseek(biblioteca, offsetDocumento, SEEK_SET);
            while(!feof(documento)){
                bytes = fread(buffer, 1, BUFFER_SIZE, documento);
                fwrite(buffer, 1, bytes, biblioteca);
            }

            fseek(biblioteca, 4, SEEK_SET);
            fread(&offsetDiretorio, sizeof(long), 1, biblioteca);

            fseek(biblioteca, offsetDiretorio, SEEK_SET);
            fwrite(aux, sizeof(Document),lib->count, biblioteca);

            free(buffer);
            free(aux);

            return 0;

        }else{

            lib->docs[lib->count] = doc;

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

                    
            free(aux);
        }

    }

    free(buffer);

    rewind(biblioteca);

    lib->count = lib->count + 1;
    bytes = fwrite(&(lib->count), sizeof(int), 1, biblioteca);

    long offset = sizeof(int) + sizeof(long) + soma + lib->docs[lib->count-1].size;
    printf("Soma: %ld\n", soma);
    printf("Offset: %ld\n", offset);

    fwrite(&offset, sizeof(long), 1, biblioteca);

    printf("%d\n", lib->count);
    
    return 0;

}

// remove o arquivo da biblioteca
int gbv_remove(Library *lib, const char *docname){
    if(!lib || !docname){
        printf("Parametros inválidos!\n");
        return 1;
    }

    FILE *biblioteca = fopen(gbv, "r+b");
    if(!biblioteca){
        printf("Não foi possível abrir o arquivo!\n");
        return 2;
    }

    if(lib->count <= 0){
        printf("Biblioteca Vazia!\n");
        return 3;
    }

    int cont;
    for (cont = 0; cont < lib->count; cont++){
        if(strcmp(lib->docs[cont].name, docname) == 0){
            break;
        }
    }
    if(cont >= lib->count){
        printf("Documento não existe no vetor!\n");
        return 4;
    }

    
    long offset;
    int quantidadeDocs;

    if (lib->count == 1){

        quantidadeDocs = 0;
        offset = 12;

        // Se a biblioteca só tem um elemento, zera a quantidade de arquivos e coloca o offset em 12 
        rewind(biblioteca);
        fwrite(&quantidadeDocs, sizeof(int), 1, biblioteca);
        fwrite(&offset, sizeof(long), 1, biblioteca);

        // fwrite(&aux, 1, lib->docs[0].size, biblioteca);
        // fwrite(&aux, sizeof(Document), 1, biblioteca);
        
    } else {
        Document docExcluido = lib->docs[cont];
        
        // Traz todo mundo para trás
        for(int j = cont; j < lib->count-1; j++){
            lib->docs[j] = lib->docs[j+1];
        }
        // coloca o item que desejamos excluir na ultima posicao do vetor
        lib->docs[lib->count-1] = docExcluido;

        // Diminui a quantidade de documentos
        quantidadeDocs = lib->count-1;

        fseek(biblioteca, 4, SEEK_SET);
        fread(&offset, sizeof(long), 1, biblioteca);

        rewind(biblioteca);

        mover(lib, biblioteca, lib->docs[cont].offset, lib->docs[cont].offset+lib->docs[cont].size, offset-lib->docs[cont].offset);

        int i = cont+1;
        while(i < lib->count){
            rewind(biblioteca);
            mover(lib, biblioteca, lib->docs[i].offset, lib->docs[i].offset+lib->docs[i].size, -lib->docs[cont].size);
            i++;
        }
        
        offset -= lib->docs[cont].size;
            
        rewind(biblioteca);

        fwrite(&quantidadeDocs, sizeof(int), 1, biblioteca);
        fwrite(&offset, sizeof(long), 1, biblioteca);

        fseek(biblioteca, offset, SEEK_SET);
        fwrite(&lib->docs, sizeof(Document), quantidadeDocs, biblioteca);
    }

    lib->count--;




    offset -= lib->docs[cont].size;

    
    // int i = cont;
    // while(i < lib->count){
    //     mover(lib, biblioteca, lib->docs[cont].offset, lib->docs[cont].offset+lib->docs[cont].size, -1);
    //     i++;
    // }

    // int quantidadeDocs = 0;
    // long offset = 12;
    // char aux = 0;

    // if (lib->count == 1){
    //     // Se a biblioteca só tem um elemento, zera a quantidade de arquivos e coloca o offset em 12 
    //     rewind(biblioteca);
    //     fwrite(&quantidadeDocs, sizeof(int), 1, biblioteca);
    //     fwrite(&offset, sizeof(long), 1, biblioteca);

    //     // fwrite(&aux, 1, lib->docs[0].size, biblioteca);
    //     // fwrite(&aux, sizeof(Document), 1, biblioteca);
        
    // } else {

    //     Document docExcluido = lib->docs[cont];
        
    //     // Calcula o offset da area de diretorio
    //     for(int i = 0; i<lib->count; i++){
    //         offset+=lib->docs[i].size;
    //     }
    //     offset = offset - docExcluido.size;

    //     // Traz todo mundo para trás
    //     for(int j = cont; j < lib->count-1; j++){
    //         lib->docs[j] = lib->docs[j+1];
    //     }
    //     // coloca o item que desejamos excluir na ultima posicao do vetor
    //     lib->docs[lib->count-1] = docExcluido;

    //     // Diminui a quantidade de documentos
    //     quantidadeDocs = lib->count-1;
    
    //     rewind(biblioteca);
    //     fwrite(&quantidadeDocs, sizeof(int), 1, biblioteca);
    //     fwrite(&offset, sizeof(long), 1, biblioteca);

    //     mover(biblioteca, docExcluido.offset, offset, docExcluido.size);

    // }

    // lib->count--;
    return 0;
}

// lista os documentos da biblioteca
int gbv_list(const Library *lib){
    if(!lib)
        return 1;

    if(lib->count <= 0){
        printf("Biblioteca vazia!\n");
        return 2;
    }

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
    if(!lib || !docname){
        printf("Parâmetros inválidos!\n");
        return 1;
    }
    
    FILE *biblioteca = fopen(gbv, "r+b");

    if(!biblioteca){
        printf("Não foi possível abrir o documento\n");
        return 2;
    }

    if(lib->count <= 0){
        printf("Biblioteca vazia!\n");
        return 3;
    }

    // Encontra o documento procurado dentro da biblioteca
    int i;
    for(i = 0; i<lib->count; i++){
        if(strcmp(lib->docs[i].name, docname) == 0){
            break;
        }
    }



    printf("n -> Próximo bloco\n");
    printf("p -> Bloco anterior\n");
    printf("q -> sair da vizualização\n");

    char opcao;
    char *buffer = (char *) malloc(BUFFER_SIZE);
    if(!buffer){
        printf("Não foi possível alocar memória!\n");
        return 4;
    }

    long tam = lib->docs[i].size;
    long bytes = tam;
    long bloco = 0;
    long lidos = 0;
    long offset = lib->docs[i].offset;

    fseek(biblioteca, offset, SEEK_SET);

    if(bytes > BUFFER_SIZE){
        bloco = BUFFER_SIZE-1;
    } else{
        bloco = bytes;
    }

    printf("Exibindo conteudo do documento:\n");
    lidos = fread(buffer, 1, bloco, biblioteca);
    buffer[lidos] = '\0';
    printf("%s\n", buffer);
    
    bytes -= lidos;

    while(bytes > 0){

        if(bytes > BUFFER_SIZE){
            bloco = BUFFER_SIZE;
        } else{
            bloco = bytes;
        }

        opcao = getchar();

        if (opcao == 'n'){
            lidos = fread(buffer, 1, bloco, biblioteca);
            buffer[lidos] = '\0';
            printf("%s\n", buffer);

            bytes -= lidos;

        } else if(opcao == 'p'){
            fseek(biblioteca, -2*BUFFER_SIZE, SEEK_CUR);
            lidos = fread(buffer, 1, bloco, biblioteca);
            buffer[lidos] = '\0';
            bytes += lidos;

            printf("%s\n", buffer);
        }
        else if(opcao == 'q'){
            break;
        } else if (opcao != 'q' && opcao != 'p' && opcao != 'n' ){
            printf("Opção inválida!\n");
        }
        
        getchar() ;  
    }

    free(buffer);
    
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