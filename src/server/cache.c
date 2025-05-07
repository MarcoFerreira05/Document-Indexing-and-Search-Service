#include <stdio.h>
#include <glib.h>
#include <index.h>
#include <stdlib.h>


/*DEBUG

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

--------------------------------*/


typedef struct cachepage{
    int key;
    int ref;
    int dirty;

}* CachePage;

int Cache_size = 10;
GHashTable* Cache = NULL;
CachePage* OnCache= NULL;
int AddOffset = 0;
int OnCacheUsado = 0;

// Poderá ser char ao inves de int para poupar memoria 
// Poderá ser usada uma GSList simpel ao invés da GList

/*
void print_glist_cachepages(GList *list) {
    for (GList *l = list; l != NULL; l = l->next) {
        CachePage page = (CachePage) l->data;
        printf("[key: %d, ref: %d, dirty: %d] -> ", page->key, page->ref, page->dirty);
    }
    printf("NULL\n");
}
//_____
*/
//remove o LRU element from cache
int cacheRemove(){
    
    for(int i = 0; i < Cache_size; i++){

        //print_hash_table(Cache);

        //printf("Visiting key %d\n", *(int*)emCheck->data);
        CachePage cachePage = OnCache[i];
        if(cachePage->ref >0){
            cachePage->ref--;
        }
        else
        {
            IndexPack pack = g_hash_table_lookup(Cache, &cachePage->key);
            //printf("Saving key %d on disk\n", cachePage->key);
            if(cachePage->dirty == 1){
                
                if(pack != NULL){
                    pack->deleted = 0;
                    if(IndexAddManager(pack,cachePage->key) == -1){
                        //os erros são impressos na função de add
                        return -1;
                    }
                }
                cachePage->dirty = 0;
            }
            //printf("Removing key %d from cache\n", cachePage->key);
            if(pack != NULL){
                g_hash_table_remove(Cache, &cachePage->key);
                OnCacheUsado--;
                //free(pack);
                return i;
            }
            //printf("Cache after removing key\n");
            //print_glist_cachepages(g_list_first(OnCache));
            break;
        }

        if(i == Cache_size-1){
           i = -1;
        }
    }

    return -1;
}
/*
int cacheRemove(){
    
    
    int i = rand() % Cache_size;

    CachePage cachePage = OnCache[i];
    IndexPack pack = g_hash_table_lookup(Cache, &cachePage->key);
    //printf("Saving key %d on disk\n", cachePage->key);
    if(cachePage->dirty == 1){
        
        if(pack != NULL){
            pack->deleted = 0;
            if(IndexAddManager(pack,cachePage->key) == -1){
                //os erros são impressos na função de add
                return -1;
            }
        }
        cachePage->dirty = 0;
    }
    //printf("Removing key %d from cache\n", cachePage->key);
    if(pack != NULL){
        g_hash_table_remove(Cache, &cachePage->key);
        OnCacheUsado--;
        //free(pack);
        return i;
    }
    //printf("Cache after removing key\n");
    //print_glist_cachepages(g_list_first(OnCache));

    return i;
}*/

int searchForDeletedSpace(){
    for(int i = 0; i < Cache_size; i++){
        CachePage cachePage = OnCache[i];
        if(cachePage == NULL){
            return i;
        }
    }
    return -1;
}

int cacheInit(int CacheSize) {

    if(CacheSize > 0){
        Cache_size = CacheSize;
    }
    if(Cache == NULL){

        Cache = g_hash_table_new_full(g_int_hash, g_int_equal, g_free, g_free);

        if(Cache == NULL){
            perror("Failed to create cache");
            return -1;
        }

        OnCache = calloc(Cache_size,sizeof(struct cachepage));

        if(Cache == NULL){
            perror("Failed to create cache");
            return -1;
        }
        AddOffset = IndexGetKey();
    }else{
        puts("Cache already initialized\n");
        return -1;
    } 

    return 0; 

}

int insertOntoCache(int key, IndexPack value,int isDirty) {
    //printf("\n\n\n-Request to add value to cache with key %d\n",key);
    CachePage page = malloc(sizeof(struct cachepage));
    
    if(page == NULL){
        perror("Failed to allocate memory for cache page");
        return -1;
    }

    page->key = key;
    page->ref = 1;
    page->dirty = isDirty;
    
    
    int writePageIndex = OnCacheUsado;
    if(OnCacheUsado >= Cache_size){
        //Se a cache estiver cheia, remover o LRU element
        //printf("Cache is full, removing LRU element\n");
        if((writePageIndex = searchForDeletedSpace()) != -1){
            
        }else if((writePageIndex = cacheRemove()) == -1){
            perror("Failed to remove LRU element from cache");
            return -1;
        }
    }
    
    //printf("\nAdded key %d to cache\n", key);
    int *key_ptr = malloc(sizeof(int));
    *key_ptr = key;

    value->deleted = writePageIndex;
    g_hash_table_insert(Cache, key_ptr, value);
    OnCache[writePageIndex] = page;
    OnCacheUsado++;
    return 0;
}

int cacheAdd(IndexPack value) {

    if(Cache == NULL){
        puts("Cache not initialized\n");
        if(cacheInit(Cache_size) == -1){
            return -1;
        }
    }

    //Obter a chave do pacote
    int key = AddOffset;    
    AddOffset ++; 

    if (insertOntoCache(key, value,1) == 1){
        perror("Failed to insert value into cache");
        return -1;
    }

    return key;
}


void* cacheGet(int key) {

    if(Cache == NULL){
        puts("Cache not initialized\n");
        if(cacheInit(Cache_size) == -1){
            return NULL;
        }
    }

    if(key >= AddOffset) {
        return NULL;
    }

    IndexPack pack = g_hash_table_lookup(Cache, &key);


    if(pack == NULL){
        
        pack =  IndexConsultManager(key);
       
        if(pack == NULL || pack->deleted == 1){
            //perror("Document not found\n");
            return NULL;
        }
        else{
            if(insertOntoCache(key, pack,0)){
                perror("Failed to insert value into cache");
                return NULL;
            }
        }

    }else{
        
        CachePage cachePage = OnCache[pack->deleted];
        if(cachePage->ref < 2){
            cachePage->ref++;
        }
    }

    return pack;

}

/*
gint CompareCachePages(  gconstpointer a,gconstpointer b){

    CachePage EmCache = (CachePage) a; 
    CachePage EmProcura = (CachePage) b;

    if(EmCache->key == EmProcura->key){
        return 0;
    }
    else if(EmCache->key > EmProcura->key){
        return 1;
    }else{
        return -1;
    }
}
*/

int cacheDelete(int key) {

    if(Cache == NULL){
        puts("Cache not initialized\n");
        if(cacheInit(Cache_size) == -1){
            return -1;
        }
    }

        if(cacheGet(key) == NULL || key >= AddOffset){
        //Chave ja eliminada ou não existe 
        return -1;
    }


    //Verifica se o documento existe no cache e elimina da cache
    IndexPack pack = g_hash_table_lookup(Cache, &key);
    if(pack != NULL){
        int indice = pack->deleted;
        free(OnCache[indice]);
        OnCache[indice] = NULL;
        g_hash_table_remove(Cache, &key);
    
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
    for(int i = 0;i < OnCacheUsado;i++){
        CachePage cachePage = OnCache[i];
        if(cachePage == NULL){
        }
        else if( cachePage->dirty == 1){
            IndexPack pack = g_hash_table_lookup(Cache, &cachePage->key);
            if(pack != NULL){
                pack->deleted = 0;
                if(IndexAddManager(pack,cachePage->key) == -1){
                    perror("Failed to write cache page to disk");
                    return -1;
                }
            }
            //free(pack);
        }
    }
    
    g_hash_table_destroy(Cache);
    Cache = NULL;
    return 0;

}

GArray* listDocuments() {

    GArray* documents = g_array_new(FALSE, FALSE, sizeof(int));
    
    if(Cache == NULL){
        puts("Cache not initialized\n");
        if(cacheInit(Cache_size) == -1){
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

