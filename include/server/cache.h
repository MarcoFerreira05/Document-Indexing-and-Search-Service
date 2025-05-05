#ifndef CACHE_H
#define CACHE_H

/**
 * @file cache.h
 * @brief Header file for document chach management.
 *
 * This file contains the function declarations and data structures used for
 * managing the cache, including adding, consulting, and deleting documents.
 */
#include <stdio.h>
#include <glib.h>
#include <index.h>

/**
 * @brief Inicializa a cache.
 * 
 * Inicia a cache.
 * 
 * @param size Tamanho da cache (default 10).
 * @return 0 em caso de sucesso, -1 em caso de erro.
 */
int cacheInit(int size);

/**
 * @brief Adiciona um novo valor à cache.
 * 
 * @param value Ponteiro para o valor a ser adicionado.
 * @return Chave gerada para o novo valor, ou -1 em caso de erro.
 */
int cacheAdd(void *value);

/**
 * @brief Recupera um valor da a sua chave.
 * 
 * Caso não esteja na cache, tenta carregar do gestor de índice.
 * 
 * @param key Chave do valor a ser recuperado.
 * @return Ponteiro para o valor encontrado ou NULL se não existir.
 */
void* cacheGet(int key);

/**
 * @brief Elimina um valor e marca o documento como apagado.
 * 
 * @param key Chave do valor a ser eliminado.
 * @return 0 em caso de sucesso, -1 em caso de erro.
 */
int cacheDelete(int key);

/**
 * @brief Liberta todos os recursos associados à cache.
 * 
 * Grava as páginas dirty em disco antes de destruir a cache.
 * 
 * @return 0 em caso de sucesso, -1 em caso de erro.
 */
int cacheDestroy();

/**
 * @brief Lista todos os documentos presentes na cache.
 * 
 * @return Array com os caminhos de todos os documentos presentes. 
 * @note O array (e seus elementos) deve ser libertado após o uso.
 */
GArray* listDocumentsPaths();
#endif // CACHE_H