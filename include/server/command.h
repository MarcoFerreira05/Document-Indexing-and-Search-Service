/**
 * @file command.h
 * @brief Header file for document indexing and management commands.
 *
 * This file contains the function declarations and data structures used for
 * managing document indexing, including adding, removing, and handling metadata.
 */

#ifndef COMMAND_H
#define COMMAND_H

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

 /**
 * @brief Estrutura que representa um pacote de indexação de documentos.
 *
 * Esta estrutura armazena os metadados de um documento, incluindo título, autores,
 * ano de publicação e o caminho do arquivo no sistema.
 */
typedef struct indexPackage{
    int deleted;
    char Title[200];
    char authors[200];
    char year[5];
    char path[64];

}*IndexPack;

/**
 * @brief Prepara um arquivo para ser guardado e indexado.
 *
 * Esta função recebe um ponteiro para uma lista dos atributos de um documento a ser
 * indexado e transforma-o numa struct para ser guardado e indexado no HD.
 *
 * @param ToIndex Ponteiro para um array de strings que representa o documento a indexar.
 * @return Retorna a key onde o mesmo foi indexado ou -1 em caso de erro.
 */
int add_document(char title[], char authors[], char year[], char path[]);

/**
 * @brief Remove um documento do índice.
 *
 * Esta função remove um documento do índice, utilizando a chave fornecida.
 * Caso a remoção seja bem-sucedida, retorna 0. Em caso de erro, retorna -1.
 *
 * @param key Chave do documento a ser removido.
 * @return Retorna 0 em caso de sucesso ou -1 em caso de erro.
 */
int delete_document(int key);

/**
 * @brief Recupera os metadados de um documento indexado.
 *
 * Esta função busca os metadados de um documento no índice, utilizando a chave fornecida.
 *
 * @param key Chave do documento a ser recuperado.
 * @return Ponteiro para a estrutura IndexPack contendo os metadados do documento,
 *         ou NULL em caso de erro.
 */
char** consult_document(int key);

/**
 * @brief Lista todos os documentos indexados.
 *
 * Esta função exibe uma lista de todos os documentos atualmente indexados no sistema.
 *
 * @return Retorna 0 em caso de sucesso ou -1 em caso de erro.
 */
GArray* all_valid_keys();

#endif // COMMAND_H