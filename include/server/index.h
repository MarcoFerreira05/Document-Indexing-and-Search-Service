#ifndef INDEX_H
#define INDEX_H

#define INDEX_FILE "index"

typedef struct {
    int fd;
    char *documents_folder;
} Index;

Index *index_init();


 /**
 * @brief Prepara um arquivo para ser guardado e indexado.
 *
 * Esta função recebe um ponteiro para uma lista dos atributos de um documento a ser
 * indexado e transforma-o numa struct para ser guradado e indexado no HD.
 *
 * @param ToIndex Ponteiro para um array de strings que representa o documento a indexar.
 * @return Retorna um ponteiro para a string com a key onde o mesmo foi indexado.
 */
char* AddDocument(char **ToIndex);


#endif