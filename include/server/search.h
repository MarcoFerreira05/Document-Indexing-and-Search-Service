#ifndef SEARCH_H
#define SEARCH_H

#include <glib.h>

/**
 * @brief Procura uma palavra no ficheiro associado a uma chave.
 * 
 * Esta função é executada pelo servidor num processo que faz procuras, para que novos clientes não fiquem
 * bloqueados por causa da procura atual.
 * 
 * Cria um novo processo para executar o grep, utilizando pipe + dup2.
 * 
 * O argumento @c one_ocurrence deve ser falso (0) se estiver a ser feita uma procura -l.
 * Deve ser verdadeiro (!=0) quando esta função é invocada como auxiliar para as procuras -s.
 * 
 * @param key Chave da meta-informação do ficheiro.
 * @param keyword Palavra a procurar.
 * @param one_ocurrence Indica se só se pretende encontrar a 1ª ocorrência da palavra.
 * 
 * @return Número de ocorrências da @c keyword , ou -1 em caso de erro.
 */
int search_keyword_in_file(int key, char *keyword, int one_ocurrence);


/**
 * @brief Determina quais os ficheiros que contêm uma palavra-chave.
 * 
 * Para cada chave em @c keys, invoca a @c search_keyword_in_file com o campo @c one_ocurrence a 1.
 * Se o resultado da chamada for > 0 (ou seja, a palavra existe no ficheiro), a chave é acresentada ao array do resultado.
 * Esta é a versão sequencial.
 * O @c GArray devolvido deve ser libertado por quem invocou esta função.
 * 
 * @param keys Chaves de todos os documentos indexados pelo servidor.
 * @param keyword Palavra a procurar.
 * 
 * @return Apontador para um array que contém as chaves dos ficheiros onde a palavra ocorre.
 * 
 * @note O tipo de @c keys poderá não ser este, isto é apenas para poder testar.
 */
GArray* docs_with_keyword(GArray *keys, char *keyword);


/**
 * @brief Determina, concorrentemente, quais os ficheiros que contêm uma palavra-chave.
 * 
 * São utilizados 2 pipes anónimos.
 * Num deles, o pai escreve todas as chaves e os filhos lêm todos deste mesmo pipe.
 * Para cada chave que vão buscar ao pipe, os filhos procuram a palavra no ficheiro, invocando a
 * @c search_keyword_in_file com o campo @c one_ocurrence a 1., e se encontrarem escrevem a chave no outro pipe.
 * Enquanto isto decorre, o pai lê do segundo pipe as chaves onde ocorreu a palavra e vai acrescentando ao array.
 * O @c GArray devolvido deve ser libertado por quem invocou esta função.
 * 
 * @param keys Chaves de todos os documentos indexados pelo servidor.
 * @param keyword Palavra a procurar.
 * @param number_procs Número de processos a utilizar na procura.
 * 
 * @return Apontador para um array que contém as chaves dos ficheiros onde a palavra ocorre.
 * 
 * @note O tipo de @c keys poderá não ser este, isto é apenas para poder testar.
 */
GArray* docs_with_keyword_concurrent(GArray *keys, char *keyword, int number_procs);

#endif