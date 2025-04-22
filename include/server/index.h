/**
 * @file index.h
 * @brief Header file for document indexing management.
 *
 * This file contains the function declarations and data structures used for
 * managing the document index, including adding, consulting, and deleting documents.
 */

#ifndef INDEX_H
#define INDEX_H

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "cache.h"
#include "command.h"

/**
 * @brief Adiciona retorna a proxima Key disponivel.
 *
 * Retorna a proxima Key disponivel para adicionar um novo documento ao índice.
 *
 *
 * @return Retorna a Key do Ficheiro ou -1 em caso de erro.
 */
int IndexGetKey();

/**
 * @brief Adiciona um documento ao índice.
 *
 * Esta função adiciona um novo documento ao arquivo de índice, calculando uma chave única
 * para o documento com base no offset atual do arquivo. O documento também é adicionado ao cache.
 *
 * @param argument Ponteiro para a estrutura do documento a ser indexado.
 * @return Retorna a chave única do documento em caso de sucesso ou -1 em caso de erro.
 */
int IndexAddManager(IndexPack argument, int key);

/**
 * @brief Consulta um documento no índice.
 *
 * Esta função consulta um documento no índice e retorna o caminho do arquivo
 * correspondente à chave fornecida.
 *
 * @param key Chave do documento a ser consultado.
 * @return Retorna o conteúdo guardado na correspondente chave ou NULL em caso de erro.
 */
void* IndexConsultManager(int key);

/**
 * @brief Remove um documento do índice.
 *
 * Esta função remove um documento do índice, sobrescrevendo os dados do documento
 * com um pacote em branco (campos nulos).
 *
 * @param key Chave do documento a ser removido.
 * @param BlankPackage Pacote de Indexação com os campos a NULL.
 * @return Retorna 0 em caso de sucesso ou -1 em caso de erro.
 */
int IndexDeleteManager(int key, IndexPack* BlankPackage);

#endif // INDEX_H