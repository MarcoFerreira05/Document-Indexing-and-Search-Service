#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "cache.h"
#include "command.h"
#include <stdio.h>

/**
 * @brief Adiciona retorna a proxima Key disponivel.
 *
 * Retorna a proxima Key disponivel para adicionar um novo documento ao índice.
 *
 *
 * @return Retorna a Key do Ficheiro ou -1 em caso de erro.
 */
int IndexGetKey(){
    //Verificar se o arquivo Index existe
    int IndexFile = open("IndexFile.txt", O_RDONLY | O_CREAT| O_APPEND, 0600);
    if(IndexFile == -1){
        //Erro ao abrir o arquivo
        perror("Erro ao abrir o arquivo de índice");
        return -1;
    }

    // -- Calcular Offset do documento -- //
    off_t offset = lseek(IndexFile, 0, SEEK_END);
    if (offset == -1) {
        perror("Erro ao obter o offset do documento");
        close(IndexFile);
        return -1;
    }

    close(IndexFile);
    return offset / sizeof(IndexPack); // Retorna a chave do próximo documento
}


/**
 * @brief Adiciona informação ao índice de documentos.
 *
 * Esta função adiciona documentos ao índice.
 *
 * @param argument Ponteiro para os dados para adicionar ao index (IndexPack).
 * @return Retorna a Key do Ficheiro ou -1 em caso de erro.
 */
int IndexAddManager(IndexPack argument,int key){

    //Verificar se o arquivo Index existe 
    int IndexFile = open("IndexFile.txt", O_RDONLY | O_CREAT, 0600); 
    
    if(IndexFile == -1){
        //Erro ao abrir o arquivo
        perror("Erro ao abrir o arquivo de índice");
        return -1;
    }
    
    // -- Calcular Offset do documento -- //
    off_t offset = key * sizeof(IndexPack);
    off_t offsetSeek = lseek(IndexFile, offset, SEEK_SET);
    if (offsetSeek == -1) {
        perror("Erro ao obter o offset do documento");
        close(IndexFile);
        return -1;
    }


    size_t bytesWritten = write(IndexFile,argument, sizeof(IndexPack));

    if(bytesWritten == -1){
        //Erro ao escrever no arquivo
        perror("Erro ao escrever no arquivo de índice");
        return -1;
    }

    close(IndexFile);
    return key; // Escrita bem sucedida
}




/**
 * @brief Consulta um documento no índice.
 *
 * Esta função auxiliar consulta um documento no índice e retorna o caminho do arquivo
 * correspondente.
 *
 * @param key Chave do documento a ser consultado.
 * @return Retorna o conteudo guardado na correspondente Key ou NULL em caso de erro.
 */

void* IndexConsultManager(int key){
    
    //Verificar se o arquivo Index existe
    int IndexFile = open("IndexFile.txt", O_RDONLY , 0600);
    if(IndexFile == -1){
        //Erro ao abrir o arquivo
        perror("Erro ao abrir o arquivo de índice");
        return NULL;
    }

    // -- Calcular Offset do documento -- //
    off_t offset = key * sizeof(IndexPack);
    off_t offsetSeek = lseek(IndexFile, offset, SEEK_SET);
    if (offsetSeek == -1) {
        perror("Erro ao obter o offset do documento");
        close(IndexFile);
        return NULL;
    }

    void* pack = malloc(sizeof(IndexPack));
    ssize_t bytesRead = read(IndexFile, &pack, sizeof(IndexPack));
    if(bytesRead == -1){
        //Erro ao ler o arquivo
        perror("Erro ao ler o arquivo de índice");
        free(pack);
        close(IndexFile);
        return NULL;
    }

    return pack;
}

/**
 * @brief Remove um documento do índice.
 *
 * Esta função remove um documento do índice.
 *
 * @param key Chave do documento a ser removido.
 * @param DeletePackage Pacote de Indexação com os campos a NULL.
 * @return Retorna 0 em caso de sucesso ou -1 em caso de erro.
 */
int IndexDeleteManager(int key,IndexPack *BlankPackage){

    //Verificar se o arquivo Index existe
    int IndexFile = open("IndexFile.txt", O_WRONLY, 0600);
    if(IndexFile == -1){
        //Erro ao abrir o arquivo
        perror("Erro ao abrir o arquivo de índice");
        return -1;
    }

    // -- Calcular Offset do documento -- //
    off_t offset = key * sizeof(IndexPack);
    off_t offsetSeek = lseek(IndexFile, offset, SEEK_SET);
    if (offsetSeek == -1) {
        perror("Erro ao obter o offset do documento");
        close(IndexFile);
        return -1;
    }

    //Remover o documento
    write(IndexFile, *BlankPackage, sizeof(IndexPack));
    close(IndexFile);

    return 0;

}

