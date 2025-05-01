#include <stdio.h>
#include <glib.h>
#include <index.h>


/*DEBUG*/

void print_entry(gpointer key, gpointer value, gpointer user_data){
    IndexPack data  = (IndexPack) value;
    int keyuser = *(int*)key;
    printf("Chave: %d | Valor: %s\n", keyuser, data->Title);
}

void print_hash_table(GHashTable *table) {
    if (!table) {
        printf("Tabela hash nula.\n");
        return;
    }

    printf("\n=== Conteúdo da Tabela Hash ===\n");


    g_hash_table_foreach(table, print_entry, NULL);
}

/*--------------------------------*/


int Cache_size = 10;
GHashTable* Cache = NULL;
GList* OnCache= NULL;
int AddOffset = 0;

typedef struct cachepage{
    int key;
    int ref;
    int dirty;

} * CachePage;
// Poderá ser char ao inves de int para poupar memoria 
// Poderá ser usada uma GSList simpel ao invés da GList

//DEBUG
void print_glist_cachepages(GList *list) {
    for (GList *l = list; l != NULL; l = l->next) {
        CachePage page = (CachePage) l->data;
        printf("[key: %d, ref: %d, dirty: %d] -> ", page->key, page->ref, page->dirty);
    }
    printf("NULL\n");
}
//_____

//remove o LRU element from cache
int cacheRemove(){

    
    //Remover o LRU element
    GList* emCheck = g_list_first(OnCache);
    
    print_glist_cachepages(emCheck);
    
    while(emCheck != NULL){

        print_hash_table(Cache);

        printf("Visiting key %d\n", *(int*)emCheck->data);
        CachePage cachePage = (CachePage)emCheck->data;
        if(cachePage->ref == 1){
            cachePage->ref = 0;
        }
        else
        {
            printf("Saving key %d on disk\n", cachePage->key);
            if(cachePage->dirty == 1){
                IndexPack pack = g_hash_table_lookup(Cache, &cachePage->key);
                if(pack != NULL){
                    if(IndexAddManager(pack,cachePage->key) == -1){
                        perror("Failed to write cache page to disk\n");
                        return -1;
                    }
                }
                cachePage->dirty = 0;
            }
            
            printf("Removing key %d from cache\n", cachePage->key);
            IndexPack pack = g_hash_table_lookup(Cache, &cachePage->key);
            if(pack != NULL){
                g_hash_table_remove(Cache, &cachePage->key);
                OnCache = g_list_delete_link(OnCache, emCheck);
            }
            printf("Cache after removing key\n");
            print_glist_cachepages(g_list_first(OnCache));
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

int cacheInit(int CacheSize) {

    if(CacheSize > 0){
        Cache_size = CacheSize;
    }
    if(Cache == NULL){

        Cache = g_hash_table_new_full(g_int_hash, g_int_equal, g_free, g_free);


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
        if(cacheInit(Cache_size) == -1){
            perror("Cache not initialized\n");
            return -1;
        }
    }

    //Obter a chave do pacote
    int key = AddOffset;    
    AddOffset ++; 

    printf("\n\n\n-Request to add value to cache with key %d\n",key);
    CachePage page = malloc(sizeof(struct cachepage));
    if(page == NULL){
        perror("Failed to allocate memory for cache page\n");
        return -1;
      }

    page->key = key;
    page->ref = 1;
    page->dirty = 1;

    if(g_list_length(OnCache) >= Cache_size){
        //Se a cache estiver cheia, remover o LRU element
        printf("Cache is full, removing LRU element\n");
        if(cacheRemove() == -1){
            perror("Failed to remove LRU element from cache\n");
            return -1;
        }
    }
    
    printf("\nAdded key %d to cache\n", key);
    int *key_ptr = malloc(sizeof(int));
    *key_ptr = key;
    g_hash_table_insert(Cache, key_ptr, value);
    OnCache = g_list_append(OnCache, page);
    print_glist_cachepages(g_list_first(OnCache));
    return key;

}


void* cacheGet(int key) {

    if(Cache == NULL){
        if(cacheInit(Cache_size) == -1){
            perror("Cache not initialized\n");
            return NULL;
        }
    }    

    IndexPack pack = g_hash_table_lookup(Cache, &key);

    if(pack == NULL){
        pack =  IndexConsultManager(key);
       
        if(pack == NULL || pack->deleted == 1){
            //perror("Document not found\n");
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
            
            if(g_list_length(OnCache) >= Cache_size){
                //Se a cache estiver cheia, remover o LRU element
                if(cacheRemove() == -1){
                    perror("Failed to remove LRU element from cache\n");
                    return NULL;
                }
            }

            g_hash_table_insert(Cache, &page->key, pack);
            OnCache = g_list_append(OnCache, page);
        }

    }

    return pack;

}

int cacheDelete(int key) {

    if(Cache == NULL){
        if(cacheInit(Cache_size) == -1){
            perror("Cache not initialized\n");
            return -1;
        }
    }

    //Verifica se o documento existe no cache e elimina da cache
    IndexPack pack = g_hash_table_lookup(Cache, &key);
    if(pack != NULL){
        g_hash_table_remove(Cache, &key);
        GList* page = g_list_find_custom(OnCache, &key, (GCompareFunc)g_int_equal);
        OnCache = g_list_remove_link(OnCache, page);
        g_list_free_1(page);
    }

    
    IndexPack BlankPack = g_malloc(sizeof(struct indexPackage));
    BlankPack->deleted = 1;

    if(IndexDeleteManager(key,BlankPack) == -1){
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
        return 0;
    }

    //Iterar sobre a lista de páginas escrever em disco os elementos "dirty" da cache e liberar a memória
    GList* page = OnCache;
    while(page != NULL){
        CachePage cachePage = (CachePage)page->data;
        if(cachePage->dirty == 1){
            IndexPack pack = g_hash_table_lookup(Cache, &cachePage->key);
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

GArray* listDocuments() {

    GArray* documents = g_array_new(FALSE, FALSE, sizeof(int));
    
    if(Cache == NULL){
        if(cacheInit(Cache_size) == -1){
            perror("Cache not initialized\n");
            return NULL;
        }
    }

    for(int i = 0; i < AddOffset; i++){
        IndexPack pack = cacheGet(i);
        if(pack != NULL){
            g_array_append_val(documents, i);    
        }
    }

    return documents;
}

