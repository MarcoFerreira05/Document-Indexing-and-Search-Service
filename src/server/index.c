#include <stdlib.h>
#include <string.h>
#include "include/common/protocol.h"

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
 * @brief Gerencia o acesso ao índice de documentos.
 *
 * Esta função verifica a existência do arquivo de índice, evita concorrência no acesso,
 * e executa operações no índice conforme o modo especificado.
 *
 * @param argument Ponteiro para os dados necessários para a operação (char* Key ou IndexPack).
 * @param mode Modo de operação, ação a ser realizada no índice, adição, consulta, eliminação.
 * @return Retorna um ponteiro para o resultado da operação realizada.
 */
void* IndexAcessManager(void* argument, int mode){

    //Verificar se o arquivo Index existe 
    //Se sim, verificar se não há concorrencia para acessar o arquivo e abrir o arquivo
    //Se não, criar um novo arquivo index (Primeira execução do servidor)

    //Depois, performar a operação de acordo com o modo 
     
    //fechar o arquivo
    //retornar o resultado da operação
}


 /**
 * @brief Prepara um arquivo para ser guardado e indexado.
 *
 * Esta função recebe um ponteiro para uma lista dos atributos de um documento a ser
 * indexado e transforma-o numa struct para ser guradado e indexado no HD.
 *
 * @param ToIndex Ponteiro para um array de strings que representa o documento a indexar.
 * @return Retorna um ponteiro para a string com a key onde o mesmo foi indexado.
 */
char* AddDocument(char **ToIndex){

    //Criação do pacote a ser indexado
    IndexPack PackToIndex;
    PackToIndex.Title = strdup(ToIndex[1]);
    PackToIndex.authors = strdup(ToIndex[2]);
    PackToIndex.year = strdup(ToIndex[3]);
    PackToIndex.path = strdup(ToIndex[4]);

    //Enviar pacote para indexação
    IndexAcessManager(&PackToIndex, ADD_DOCUMENT);
}