#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "cache.h"
#include "index.h"
#include "command.h"
 
 /**
 * @brief Prepara um arquivo para ser guardado e indexado.
 *
 * Esta função recebe um ponteiro para uma lista dos atributos de um documento a ser
 * indexado e transforma-o numa struct para ser guradado e indexado no HD.
 *
 * @param ToIndex Ponteiro para um array de strings que representa o documento a indexar.
 * @return Retorna a key onde o mesmo foi indexado ou -1 em caso de erro.
 */
int add_document(char title[], char authors[], char year[], char path[]){

    //Criação do pacote a ser indexado
    IndexPack PackToIndex = g_malloc(sizeof(struct indexPackage));
    PackToIndex->deleted = 0;
    strcpy(PackToIndex->Title, title);
    strcpy(PackToIndex->authors, authors);
    strcpy(PackToIndex->year, year);
    strcpy(PackToIndex->path,path);
    
    //Enviar pacote para indexação e retorna a key (ou -1 em caso de erro)
    int Key = cacheAdd(PackToIndex);

    return Key;
}


/**
 * @brief Consulta um documento no índice.
 *
 * Esta função consulta um documento no índice e retorna o caminho do arquivo
 * correspondente.
 *
 * @param key Chave do documento a ser consultado.
 * @return Retorna o conteudo guardado na correspondente Key ou NULL em caso de erro PRECISA DE FREE.
 */
char** consult_document(int key){
    
    //Enviar key para consulta e retorna o conteudo (ou NULL em caso de erro)
    IndexPack PackToConsult = (IndexPack) cacheGet(key);    

    if(PackToConsult == NULL){
        //Erro ao consultar o documento
        //perror("Documento não encontrado");
        return NULL;
    }

    //Tradução do pacote para string
    char**content = malloc(sizeof(char*)*4);
    content[0] = strdup(PackToConsult->Title);
    content[1] = strdup(PackToConsult->authors);
    content[2] = strdup(PackToConsult->year);
    content[3] = strdup(PackToConsult->path);    
    
    
    return content;//retorno da string com o conteudo do documento
}


/**
 * @brief Remove um documento do índice.
 *
 * Esta função remove um documento do índice.
 *
 * @param key Chave do documento a ser removido.
 * @return Retorna 0 em caso de sucesso ou -1 em caso de erro.
 */
int delete_document(int key){

    if(cacheDelete(key) == -1){
        //Erro ao remover o documento
        return -1;
    }

    return 0;
}


/**
 * @brief Lista todos os documentos indexados.
 *
 * Esta função exibe uma lista de todos os documentos atualmente indexados no sistema.
 *
 * @return Retorna 0 em caso de sucesso ou -1 em caso de erro.
 */
GArray* all_valid_keys(){

    return listDocuments();
}
