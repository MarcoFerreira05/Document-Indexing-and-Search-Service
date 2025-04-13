#include "search.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>



int search_keyword_in_file(int key, char *keyword) {
    /* interação com a estrutura central
    get_packet(key)???
    ainda falta definir como é que vão funcionar os múltiplos processos de procura do servidor,
    para enviar o resultado da consulta
    */

    char *file_path = "teste.txt"; // apenas para teste

    int fildes[2];
    if(pipe(fildes) == -1) {
        perror("pipe");
        return -1;
    }

    pid_t child_id = fork();
    if(child_id == -1) {
        close(fildes[0]);
        close(fildes[1]);
        perror("fork");
        return -1;
    }
    else if(child_id == 0) {
        close(fildes[0]);
        if(dup2(fildes[1], 1) == -1) {
            perror("dup2");
            close(fildes[1]);
            return -1;
        }
        if(execlp("grep", "grep", "-c", keyword, file_path, NULL) == -1) {
            perror("execlp");
            close(fildes[1]);
            return -1;
        }
    }

    close(fildes[1]);

    int child_status;

    wait(&child_status);
    if(WIFEXITED(child_status) && WEXITSTATUS(child_status) == 2) {
        close(fildes[0]);
        return -1;
    }
    
    char line_count[10] = {'\0'};
    ssize_t bytes_read = read(fildes[0], line_count, sizeof(int));
    if(bytes_read == -1) {
        perror("read");
        close(fildes[0]);
        return -1;
    }
    line_count[bytes_read-1] = '\0';

    /*este print é apenas para verificar se funciona, falta enviar o resultado por algum pipe para o processo
    que está a fazer esta consulta*/
    printf("%s aparece em %d linhas\n", keyword, atoi(line_count));

    close(fildes[0]);

    return 0;
}

int main() {
    return search_keyword_in_file(0, "ficheiro");
}