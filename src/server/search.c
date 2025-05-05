#include "search.h"
#include "command.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <ctype.h>


// apenas para teste
/*char* get_path(int key) {
    char *path = malloc(sizeof(char) * 12);

    snprintf(path, 22, "dataset/%d.txt", key);

    return path;
}*/


int search_keyword_in_file(char *keyword, char *relative_file_path, int one_ocurrence, char *folder_path) {

    char file_path[64];
    snprintf(file_path, 64, "%s/%s", folder_path, relative_file_path);

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
        if(!one_ocurrence && execlp("grep", "grep", "-c", keyword, file_path, NULL) == -1) {
            perror("execlp");
            close(fildes[1]);
            _exit(-1);
        }
        else if(one_ocurrence && execlp("grep", "grep", "-c", "-m", "1", keyword, file_path, NULL) == -1) {
            perror("execlp");
            close(fildes[1]);
            _exit(-1);
        }
    }

    close(fildes[1]);

    int child_status;

    wait(&child_status);
    if(WIFEXITED(child_status) && WEXITSTATUS(child_status) == 2) {
        close(fildes[0]);
        return -1;
    }
    
    char line_count[11] = {'\0'};
    ssize_t bytes_read = read(fildes[0], line_count, 11);
    if(bytes_read == -1) {
        perror("read");
        close(fildes[0]);
        return -1;
    }
    line_count[bytes_read-1] = '\0';

    close(fildes[0]);

    return atoi(line_count);
}



GArray* docs_with_keyword(char *keyword, char *folder_path) {
    GArray *keys = all_valid_keys();
    GArray *docs_with_keyword = g_array_new(FALSE, TRUE, sizeof(int));

    int i, key;
    for(i = 0; i < keys->len; i++) {
        key = g_array_index(keys, int, i);
        if(search_keyword_in_file(key, keyword, 1, folder_path) > 0) g_array_append_val(docs_with_keyword, key);
    }

    g_array_free(keys, FALSE);

    return docs_with_keyword;
}



GArray* docs_with_keyword_concurrent(char *keyword, int number_procs, char *folder_path) {
    GArray *keys = all_valid_keys();

    // pipe para enviar as chaves aos filhos
    int i, keys_pipe[2];

    if(pipe(keys_pipe) == -1) {
        perror("pipe");
        return NULL;
    }

    // pipe para os filhos devolverem as chaves dos ficheiros onde a keyword ocorre
    int found_pipe[2];

    if(pipe(found_pipe) == -1) {
        perror("pipe");
        return NULL;
    }

    // lançar todos os processos
    pid_t child_id;
    for(i = 0; i < number_procs; i++) {
        child_id = fork();

        if(child_id == -1) {
            perror("fork");
            return NULL;
        }

        else if(child_id == 0) { // filhos vão lendo do pipe à medida que estão prontos e escrevem para o outro pipe as keys onde encontram
            close(keys_pipe[1]);
            close(found_pipe[0]);

            ssize_t br;
            int key;
            while((br = read(keys_pipe[0], &key, sizeof(int))) != 0) {
                if(search_keyword_in_file(key, keyword, 1, folder_path) > 0) {
                    if(write(found_pipe[1], &key, sizeof(int)) == -1) {
                        perror("write");
                        close(keys_pipe[0]);
                        close(found_pipe[1]);
                        _exit(-1);
                    }
                }
            }
            
            close(keys_pipe[0]);
            close(found_pipe[1]);
            _exit(0);
        }
    }

    close(keys_pipe[0]);
    close(found_pipe[1]);
    int key;

    // enviar chaves para os filhos
    for(i = 0; i < keys->len; i++) {
        key = g_array_index(keys, int, i);
        if(write(keys_pipe[1], &key, sizeof(int)) == -1) {
            perror("write");
            return NULL;
        }
    }

    close(keys_pipe[1]);

    // recolher chaves onde foi encontrado
    GArray *docs_with_keyword = g_array_new(FALSE, TRUE, sizeof(int));
    ssize_t br;
    while((br = read(found_pipe[0], &key, sizeof(int))) != 0) {
        if(br == -1) {
            perror("read");
            return NULL;
        }

        g_array_append_val(docs_with_keyword, key);
    }

    close(found_pipe[0]);

    // recolher filhos
    int child_status;
    for(i = 0; i < number_procs; i++) {
        wait(&child_status);
        if(WIFEXITED(child_status) && WEXITSTATUS(child_status) != 0) {
            printf("exit status %d\n", WEXITSTATUS(child_status));
        }
    }

    g_array_free(keys, FALSE);

    return docs_with_keyword;
}


// apenas para teste VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
/*
int get_keys_from_script(GArray *keys) {
    int fildes[2];
    if(pipe(fildes) == -1) {
        perror("pipe");
        return -1;
    }

    pid_t child_id = fork();
    if(child_id == -1) {
        perror("fork");
        return -1;
    }
    else if(child_id == 0) {
        close(fildes[0]);
        dup2(fildes[1], 1);
        execlp("python3", "python3", "getkeys.py", NULL);
        close(fildes[1]);
        _exit(0);
    }

    close(fildes[1]);
    
    ssize_t br = 0;
    char buffer;
    char key_builder[10] = {'\0'};
    int key, kb_index;
    while((br = read(fildes[0], &buffer, sizeof(char))) > 0) {
        if(buffer == '\n') {
            key = atoi(key_builder);
            g_array_append_val(keys, key);
            for(int i = 0; i < 10; i++) key_builder[i] = '\0';
            kb_index = 0;
        }
        else if(isdigit(buffer)) {
            key_builder[kb_index] = buffer;
            kb_index++;
        }
    }
    if(br == -1) {
        perror("read");
        return -1;
    }

    close(fildes[0]);

    return 0;
}

int main(int argc, char **argv) {
    if(argc == 3) printf("%d\n", search_keyword_in_file(atoi(argv[1]), argv[2], 0));
    else {
        GArray *keys = g_array_new(FALSE, TRUE, sizeof(int));
        
        get_keys_from_script(keys);

        GArray *docs_with_kw = docs_with_keyword_concurrent(keys, argv[1], 10);

        for(int i = 0; i < docs_with_kw->len; i++) {
            printf("%d\n", g_array_index(docs_with_kw, int, i));
        }

        g_array_free(keys, FALSE);
        g_array_free(docs_with_kw, FALSE);
    }
    
    return 0;
}*/