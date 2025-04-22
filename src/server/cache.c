#include <stdio.h>
#include <glib.h>
#include <index.h>
#define CACHE_SIZE 10

GHashTable* Cache = NULL;
GList* OnCache= NULL;
int AddOffset = 0;

typedef struct cachepage{
    int key;
    int ref;
    int dirty;

} * CachePage;

//remove o LRU element from cache
int cacheRemove(){

    //Remover o LRU element
    GList* emCheck = g_list_first(OnCache);
    while(emCheck != NULL){
        CachePage cachePage = (CachePage)emCheck->data;
        if(cachePage->ref == 1){
            cachePage->ref = 0;
        }
        else
        {
            if(cachePage->dirty == 1){
                IndexPack pack = g_hash_table_lookup(Cache, cachePage->key);
                if(pack != NULL){
                    if(IndexAddManager(pack,cachePage->key) == -1){
                        perror("Failed to write cache page to disk\n");
                        return -1;
                    }
                }
                cachePage->dirty = 0;
            }
            
            IndexPack pack = g_hash_table_lookup(Cache, cachePage->key);
            if(pack != NULL){
                g_hash_table_remove(Cache, cachePage->key);
                OnCache = g_list_remove_link(OnCache, cachePage);
                g_list_free_1(cachePage);
                free(pack);
            }
            break;
        }
        
            
        if(emCheck->next == NULL){
            emCheck = g_list_first(OnCache);
        }else{
            emCheck = emCheck->next;
        }
    }

    return 0;
}

int cacheInit() {

    if(Cache == NULL){

        Cache = g_hash_table_new_full(g_str_hash, g_int_equal, g_free, g_free);

        if(Cache == NULL){
            perror("Failed to create cache\n");
            return -1;
        }
        AddOffset = IndexGetKey();
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
    int key = AddOffset;    
    AddOffset ++; 

    CachePage page = malloc(sizeof(struct cachepage));
    if(page == NULL){
        perror("Failed to allocate memory for cache page\n");
        return -1;
      }

    page->key = key;
    page->ref = 1;
    page->dirty = 1;

    if(g_list_length(OnCache) >= CACHE_SIZE){
        //Se a cache estiver cheia, remover o LRU element
        if(cacheRemove() == -1){
            perror("Failed to remove LRU element from cache\n");
            return -1;
        }
    }
    
    g_hash_table_insert(Cache, key, value);
    OnCache = g_list_append(OnCache, page);
    return key;

}

void* cacheGet(int key) {

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
        else{
            //Adicionar à cache
            CachePage page = malloc(sizeof(struct cachepage));
            if(page == NULL){
                perror("Failed to allocate memory for cache page\n");
                return NULL;
              }
            page->key = key;
            page->ref = 1;
            page->dirty = 0;
            
            if(g_list_length(OnCache) >= CACHE_SIZE){
                //Se a cache estiver cheia, remover o LRU element
                if(cacheRemove() == -1){
                    perror("Failed to remove LRU element from cache\n");
                    return -1;
                }
            }

            g_hash_table_insert(Cache, key, pack);
            OnCache = g_list_append(OnCache, page);
        }

    }

    return pack;

}

int cacheDelete(int key) {

    if(Cache == NULL){
        if(cacheInit() == -1){
            perror("Cache not initialized\n");
            return -1;
        }
    }

    //Verifica se o documento existe no cache e elimina da cache
    IndexPack pack = g_hash_table_lookup(Cache, key);
    if(pack != NULL){
        g_hash_table_remove(Cache, key);
        GList* page = g_list_find_custom(OnCache, &key, (GCompareFunc)g_int_equal);
        OnCache = g_list_remove_link(OnCache, page);
        g_list_free_1(page);
        free(pack);
    }

    
    IndexPack BlankPack = g_malloc(sizeof(struct indexPackage));
    BlankPack->deleted = 1;

    if(IndexDeleteManager(key,&BlankPack) == -1){
        //Erro ao remover o documento
        perror("Erro ao remover o documento");
        return -1;
    }else{
        return 0;
    }

    return 0;

}

int cacheDestroy() {

    if(Cache == NULL){
        if(cacheInit() == -1){
            perror("Cache not initialized\n");
            return -1;
        }
    }

    //Iterar sobre a lista de páginas escrever em disco os elementos "dirty" da cache e liberar a memória
    GList* page = OnCache;
    while(page != NULL){
        CachePage cachePage = (CachePage)page->data;
        if(cachePage->dirty == 1){
            IndexPack pack = g_hash_table_lookup(Cache, cachePage->key);
            if(pack != NULL){
                if(IndexAddManager(pack,cachePage->key) == -1){
                    perror("Failed to write cache page to disk\n");
                    return -1;
                }
            }
            cachePage->dirty = 0;
        }
        
        page = page->next;
    }
    
    g_list_free(OnCache);
    g_hash_table_destroy(Cache);
    Cache = NULL;
    return 0;

}

