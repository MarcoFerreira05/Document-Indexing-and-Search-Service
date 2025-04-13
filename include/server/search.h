#ifndef SEARCH_H
#define SEARCH_H

/**
 * @brief Procura uma palavra no ficheiro associado a uma chave.
 * 
 * Esta função é executada pelo servidor num processo que faz procuras, para que novos clientes não fiquem
 * bloqueados por causa da procura atual.
 * 
 * Cria um novo processo para executar o grep, utilizando pipe + dup2.
 * 
 * @param key Chave da meta-informação do ficheiro.
 * @param keyword Palavra a procurar.
 * 
 * @return -1 em caso de erro, 0 se tudo correu bem
 */
int search_keyword_in_file(int key, char *keyword);

#endif