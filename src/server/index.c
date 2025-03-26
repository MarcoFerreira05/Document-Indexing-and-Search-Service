#include <stdlib.h>
#include <string.h>
#include "include/common/protocol.h"
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

/**
 * @brief Estrutura que representa um pacote de indexação de documentos.
 *
 * Esta estrutura armazena os metadados de um documento, incluindo título, autores,
 * ano de publicação e o caminho do arquivo no sistema.
 */
typedef struct indexPackage{

    char*   Title;
    char*   authors;
    char*   year;
    char*   path;

}IndexPack;


/**
 * @brief Adiciona informação ao índice de documentos.
 *
 * Esta função adiciona documentos ao índice.
 *
 * @param argument Ponteiro para os dados para adicionar ao index (IndexPack).
 * @return Retorna a Key do Ficheiro ou -1 em caso de erro.
 */
int IndexAddManager(IndexPack argument){

    //Verificar se o arquivo Index existe 
    int IndexFile = open("IndexFile.txt", O_RDONLY | O_CREAT | O_APPEND, 0600); 
    
    if(IndexFile == -1){
        //Erro ao abrir o arquivo
        perror("Erro ao abrir o arquivo de índice");
        return -1;
    }
    
    // -- Calcular Key do documento -- //
    // Obter o offset atual antes da escrita para saber a Key do documento
    off_t currentOffset = lseek(IndexFile, 0, SEEK_CUR);
    if (currentOffset == -1) {
        perror("Erro ao obter o offset atual");
        close(IndexFile);
        return -1;
    }
    int key = currentOffset / sizeof(IndexPack);


    size_t bytesWritten = write(IndexFile,&argument, sizeof(IndexPack));

    if(bytesWritten == -1){
        //Erro ao escrever no arquivo
        perror("Erro ao escrever no arquivo de índice");
        return -1;
    }
    // Concorrência em falta aqui

    close(IndexFile);
    return key; // Escrita bem sucedida
}


 /**
 * @brief Prepara um arquivo para ser guardado e indexado.
 *
 * Esta função recebe um ponteiro para uma lista dos atributos de um documento a ser
 * indexado e transforma-o numa struct para ser guradado e indexado no HD.
 *
 * @param ToIndex Ponteiro para um array de strings que representa o documento a indexar.
 * @return Retorna a key onde o mesmo foi indexado ou -1 em caso de erro.
 */
int AddDocument(char **ToIndex){

    //Criação do pacote a ser indexado
    IndexPack PackToIndex;
    PackToIndex.Title = strdup(ToIndex[1]);
    PackToIndex.authors = strdup(ToIndex[2]);
    PackToIndex.year = strdup(ToIndex[3]);
    PackToIndex.path = strdup(ToIndex[4]);

    //Enviar pacote para indexação e retorna a key (ou -1 em caso de erro)
    int Key = IndexAcessManager(PackToIndex);
    
    free(PackToIndex.Title);
    free(PackToIndex.authors);
    free(PackToIndex.year);
    free(PackToIndex.path);

    return Key;
}