#include <stdio.h>
#include <glib.h>
#include <index.h>

GHashTable* Cache = NULL;

int cacheInit() {

    if(Cache == NULL){

        Cache = g_hash_table_new_full(g_str_hash, g_int_equal, g_free, g_free);

        if(Cache == NULL){
            perror("Failed to create cache\n");
            return -1;
        }
    }else{
        perror("Cache already initialized\n");
        return -1;
    } 

    return 0; 

}

int cacheAdd(void *value) {

    if(Cache == NULL){
        if(cacheInit() == -1){
            perror("Cache not initialized\n");
            return -1;
        }
    }

    //Obter a chave do pacote
    int key = IndexAddManager(value);    

    g_hash_table_insert(Cache, key, value);

    return key;

}

void* cacheGet(int *key) {

    if(Cache == NULL){
        if(cacheInit() == -1){
            perror("Cache not initialized\n");
            return -1;
        }
    }


    IndexPack pack = g_hash_table_lookup(Cache, key);

    if(pack == NULL){

        pack =  IndexConsultManager(key);
       
        if(pack == NULL || pack->deleted == 1){
            perror("Document not found\n");
            return NULL;
        }
    }

    return pack;

}

int cacheRemove(int *key) {

    if(Cache == NULL){
        if(cacheInit() == -1){
            perror("Cache not initialized\n");
            return -1;
        }
    }
    g_hash_table_remove(Cache, key);

    

    return 0;

}

int cacheDestroy() {

    if(Cache == NULL){
        if(cacheInit() == -1){
            perror("Cache not initialized\n");
            return -1;
        }
    }
    

    g_hash_table_destroy(Cache);
    Cache = NULL;
    return 0;

}

