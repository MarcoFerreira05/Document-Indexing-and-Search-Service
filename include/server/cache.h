/**
 * @file cache.h
 * @brief Header file for the cache implementation using GLib's GHashTable.
 *
 * This file contains the function declarations for initializing, adding, 
 * retrieving, removing, and destroying a cache implemented with GLib's GHashTable.
 */

#ifndef CACHE_H
#define CACHE_H

#include <glib.h>

/**
 * @brief Initializes the cache.
 *
 * This function creates and initializes a global GHashTable to be used as a cache.
 * It ensures that the cache is only initialized once.
 *
 * @return 0 on success, -1 on failure (e.g., if the cache is already initialized).
 */
int cacheInit();

/**
 * @brief Adds a value to the cache.
 *
 * This function inserts a value into the cache. Both the key and value
 * are duplicated to ensure proper memory management.
 *
 * @param value The value associated with the key (void pointer).
 * @return 0 on success, -1 if the cache is not initialized.
 */
int cacheAdd(void *value);

/**
 * @brief Retrieves a value from the cache by its key.
 *
 * This function looks up a value in the cache using the provided key.
 *
 * @param key The key to look up in the cache (string).
 * @return A pointer to the value associated with the key, or NULL if the key is not found
 *         or the cache is not initialized.
 */
void *cacheGet(int *key);

/**
 * @brief Removes a key-value pair from the cache.
 *
 * This function removes a key-value pair from the cache using the provided key.
 *
 * @param key The key to remove from the cache (string).
 * @return 0 on success, -1 if the cache is not initialized.
 */
int cacheRemove(int *key);

/**
 * @brief Destroys the cache and frees all associated memory.
 *
 * This function destroys the cache, freeing all memory associated with it.
 * After calling this function, the cache must be reinitialized before use.
 *
 * @return 0 on success, -1 if the cache is not initialized.
 */
int cacheDestroy();

#endif 