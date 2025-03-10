#ifndef CACHE_H
#define CACHE_H

#include "metadata.h"

// Initialize cache
void init_cache(int size);

// Add document to cache
void add_to_cache(Document doc);

// Find document in cache
Document *find_in_cache(int doc_id);

// Find least recently used index
int find_lru_index();

#endif
