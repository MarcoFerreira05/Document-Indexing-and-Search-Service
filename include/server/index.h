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
 * @file index.h
 * @brief Interface para operações de gestão de documentos no ficheiro de índice.
 * 
 * Este módulo fornece funções para adicionar, consultar, eliminar documentos
 * e obter a próxima chave disponível no ficheiro "IndexFile.txt".
 */


/**
 * @brief Fornce o proximo índice disponivel.
 * @return A chave utilizada disponivel.\\
 */
int IndexGetKey();


/**
 * @brief Adiciona ou atualiza um documento no ficheiro de índice.
 * 
 * Escreve um `IndexPack` no ficheiro no offset correspondente à chave fornecida.
 * 
 * @param argument Estrutura `IndexPack` a ser escrita.
 * @param key Chave onde o documento deve ser armazenado.
 * @return A chave utilizada (>=0) ou -1 em caso de erro.\\
 */
int IndexAddManager(IndexPack argument, int key);

/**
 * @brief Consulta um documento no ficheiro de índice.
 * 
 * Lê e devolve uma estrutura `IndexPack` associada à chave fornecida.
 * 
 * @param key Chave do documento a consultar.
 * @return Ponteiro para o `IndexPack` lido, ou NULL em caso de erro.
 */
void* IndexConsultManager(int key);

/**
 * @brief Elimina logicamente um documento do ficheiro de índice.
 * 
 * Marca o documento como eliminado escrevendo um pacote vazio (`BlankPackage`) na posição correspondente.
 * 
 * @param key Chave do documento a eliminar.
 * @param BlankPackage Pacote a ser escrito no lugar do documento (normalmente com `deleted = 1`).
 * @return 0 em caso de sucesso ou -1 em caso de erro.
 */
int IndexDeleteManager(int key, IndexPack *BlankPackage);

#endif // INDEX_H
