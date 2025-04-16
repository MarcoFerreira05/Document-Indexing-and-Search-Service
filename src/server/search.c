#include "search.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>


// apenas para teste
char* get_path(int key) {
    char *path = malloc(sizeof(char) * 12);

    snprintf(path, 12, "teste%d.txt", key);

    return path;
}


int search_keyword_in_file(int key, char *keyword) {
    /* interação com a estrutura central
    get_packet(key)???
    */

    char *file_path = get_path(key);

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

    close(fildes[0]);
    free(file_path);

    return atoi(line_count);
}



GArray* docs_with_keyword(GArray *keys, char *keyword) {
    GArray *docs_with_keyword = g_array_new(FALSE, TRUE, sizeof(int));

    int i, key;
    for(i = 0; i < keys->len; i++) {
        key = g_array_index(keys, int, i);
        if(search_keyword_in_file(key, keyword) > 0) g_array_append_val(docs_with_keyword, key);
    }

    return docs_with_keyword;
}


// apenas para teste
int main(int argc, char **argv) {
    if(argc == 3) printf("%d\n", search_keyword_in_file(atoi(argv[1]), argv[2]));
    else {
        GArray *keys = g_array_new(FALSE, TRUE, sizeof(int));
        for(int i = 1; i <= 5; i++) {
            g_array_append_val(keys, i);
        }

        GArray *docs_with_kw = docs_with_keyword(keys, argv[1]);

        for(int i = 0; i < docs_with_kw->len; i++) {
            printf("%d\n", g_array_index(docs_with_kw, int, i));
        }

        g_array_free(keys, FALSE);
        g_array_free(docs_with_kw, FALSE);
    }
    
    return 0;
}