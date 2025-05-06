#include "protocol.h"
#include "cache.h"
#include "command.h"
#include "search.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <glib.h>
#include <ctype.h>

int run = 1;

int validate_number(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

int validate_args(int argc, char **argv) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s <documents_folder> [cache_size]\n", argv[0]);
        return 0;
    }

    if (argc == 3 && (!validate_number(argv[2]) || atoi(argv[2]) < 0)) {
        fprintf(stderr, "Cache size must be a positive integer\n");
        return 0;
    }
    return 1;
}

void send_response(){
    
}

void terminate_child_request() {
    // Child process requests parent to be terminated
    int request_pipe_fd = open_pipe(REQUEST_PIPE, O_WRONLY);
    Packet *terminate_request = create_packet(TERMINATE_CHILD, getpid(), -1, -1, NULL,
                                              NULL, NULL, NULL, NULL, -1);
    send_packet(terminate_request, request_pipe_fd);
    delete_packet(terminate_request);
    close_pipe(request_pipe_fd);
    exit(EXIT_SUCCESS);
}

void handle_add_document(Packet *request) {

    int key = add_document(request->title, request->authors, request->year, request->path);

    Packet *response;
    if (key == -1) {
        response = create_packet(FAILURE, -1, -1, -1, NULL,
                                 NULL, NULL, NULL, NULL, -1);
    } else {
        response = create_packet(SUCCESS, -1, key, -1, NULL,
                                 NULL, NULL, NULL, NULL, -1);
    }

    char response_pipe[MAX_PIPE_SIZE];
    snprintf(response_pipe, MAX_PIPE_SIZE, RESPONSE_PIPE_TEMPLATE, request->src_pid);
    int response_pipe_fd = open_pipe(response_pipe, O_WRONLY);
    send_packet(response, response_pipe_fd);
    delete_packet(response);
    close_pipe(response_pipe_fd);
}

void handle_consult_document(Packet *request) {

    char **metadata = consult_document(request->key);

    Packet *response;
    if (metadata != NULL) {
        response = create_packet(SUCCESS, -1, -1, -1, NULL,
                                 metadata[0], metadata[1], metadata[2], metadata[3], -1);
    } else {
        response = create_packet(FAILURE, -1, -1, -1, NULL,
                                 NULL, NULL, NULL, NULL, -1);
    }

    if (metadata != NULL) {
        for (int i = 0; i < 4; i++) {
            free(metadata[i]);
        }
        free(metadata);
    }

    char response_pipe[MAX_PIPE_SIZE];
    snprintf(response_pipe, MAX_PIPE_SIZE, RESPONSE_PIPE_TEMPLATE, request->src_pid);
    int response_pipe_fd = open_pipe(response_pipe, O_WRONLY);
    send_packet(response, response_pipe_fd);
    delete_packet(response);
    close_pipe(response_pipe_fd);
}

void handle_delete_document(Packet *request) {

    Packet *response;
    if (!delete_document(request->key)) {
        response = create_packet(SUCCESS, -1, request->key, -1, NULL,
                                 NULL, NULL, NULL, NULL, -1);
    } else {
        response = create_packet(FAILURE, -1, -1, -1, NULL,
                                 NULL, NULL, NULL, NULL, -1);
    }
    
    char response_pipe[MAX_PIPE_SIZE];
    snprintf(response_pipe, MAX_PIPE_SIZE, RESPONSE_PIPE_TEMPLATE, request->src_pid);
    int response_pipe_fd = open_pipe(response_pipe, O_WRONLY);
    send_packet(response, response_pipe_fd);
    delete_packet(response);
    close_pipe(response_pipe_fd);
}

void handle_count_lines(Packet *request, char *folder_path) {
    
    char **metadata = consult_document(request->key);
    char path[MAX_PATH_SIZE];
    strcpy(path, metadata[3]);
    
    for (int i = 0; i < 4; i++) {
        free(metadata[i]);
    }
    free (metadata);

    pid_t pid = fork();

    if (pid == 0) {
        int lines = search_keyword_in_file(request->keyword, path, 0, folder_path);

        Packet *response;
        if (lines == -1) {
            response = create_packet(FAILURE, -1, -1, -1, NULL,
                                     NULL, NULL, NULL, NULL, -1);
        } else {
            response = create_packet(SUCCESS, -1, -1, lines, NULL,
                                     NULL, NULL, NULL, NULL, -1);
        }

        char response_pipe[MAX_PIPE_SIZE];
        snprintf(response_pipe, MAX_PIPE_SIZE, RESPONSE_PIPE_TEMPLATE, request->src_pid);
        int response_pipe_fd = open_pipe(response_pipe, O_WRONLY);
        send_packet(response, response_pipe_fd);
        delete_packet(response);
        close_pipe(response_pipe_fd);

        terminate_child_request();

    } else if (pid < 0) {
        // Fork failed
        perror("Fork failed\n");
        char response_pipe[MAX_PIPE_SIZE];
        snprintf(response_pipe, MAX_PIPE_SIZE, RESPONSE_PIPE_TEMPLATE, request->src_pid);
        int response_pipe_fd = open_pipe(response_pipe, O_WRONLY);
        Packet *response = create_packet(FAILURE, -1, -1, -1, NULL,
                                         NULL, NULL, NULL, NULL, -1);
        send_packet(response, response_pipe_fd);
        delete_packet(response);
        close_pipe(response_pipe_fd);
    }
}

void handle_search_documents(Packet *request, char *folder_path) {

    pid_t pid = fork();

    if (pid == 0) {
        GArray *keys = NULL;

        if (request->n_procs <= 1) {
            keys = docs_with_keyword(request->keyword, folder_path);
        } else {
            keys = docs_with_keyword_concurrent(request->keyword, request->n_procs, folder_path);
        }

        char response_pipe[MAX_PIPE_SIZE];
        snprintf(response_pipe, MAX_PIPE_SIZE, RESPONSE_PIPE_TEMPLATE, request->src_pid);
        int response_pipe_fd = open_pipe(response_pipe, O_WRONLY);

        if (keys == NULL) {
            Packet *response = create_packet(FAILURE, -1, -1, -1, NULL,
                                             NULL, NULL, NULL, NULL, -1);
            send_packet(response, response_pipe_fd);
            delete_packet(response);
        } else if (keys->len == 0) {
            Packet *response = create_packet(SUCCESS, -1, -1, -1, NULL,
                                             NULL, NULL, NULL, NULL, -1);
            send_packet(response, response_pipe_fd);
            delete_packet(response);
        } else {
            for (int i = 0; i < keys->len; i++) {
                Packet *response = create_packet(SUCCESS, -1, g_array_index(keys, int, i), -1, NULL,
                                                 NULL, NULL, NULL, NULL, -1);
                send_packet(response, response_pipe_fd);
                delete_packet(response);
            }
        }

        g_array_free(keys, FALSE);
        close_pipe(response_pipe_fd);

        terminate_child_request();

    } else if (pid < 0) {
        // Fork failed
        char response_pipe[MAX_PIPE_SIZE];
        snprintf(response_pipe, MAX_PIPE_SIZE, RESPONSE_PIPE_TEMPLATE, request->src_pid);
        int response_pipe_fd = open_pipe(response_pipe, O_WRONLY);
        perror("Fork failed\n");
        Packet *response = create_packet(FAILURE, -1, -1, -1, NULL,
                                         NULL, NULL, NULL, NULL, -1);
        send_packet(response, response_pipe_fd);
        delete_packet(response);
        close_pipe(response_pipe_fd);
    }
}

void handle_shutdown_server(Packet *request) {
    
    run = 0;

    Packet *response = create_packet(SUCCESS, -1, -1, -1, NULL,
                                     NULL, NULL, NULL, NULL, -1);
    
    char response_pipe[MAX_PIPE_SIZE];
    snprintf(response_pipe, MAX_PIPE_SIZE, RESPONSE_PIPE_TEMPLATE, request->src_pid);
    int response_pipe_fd = open_pipe(response_pipe, O_WRONLY);
    send_packet(response, response_pipe_fd);
    delete_packet(response);
    close_pipe(response_pipe_fd);
}

void handle_request(Packet *request, char *documents_folder) {

    switch (request->code) {
        case ADD_DOCUMENT:
            handle_add_document(request);
            break;
        case CONSULT_DOCUMENT:
            handle_consult_document(request);
            break;
        case DELETE_DOCUMENT:
            handle_delete_document(request);
            break;
        case COUNT_LINES:
            handle_count_lines(request, documents_folder);
            break;
        case SEARCH_DOCUMENTS:
            handle_search_documents(request, documents_folder);
            break;
        case SHUTDOWN_SERVER:
            handle_shutdown_server(request);
            break;
        case TERMINATE_CHILD:
            waitpid(request->src_pid, NULL, 0);
            break;
        default:
            puts("Invalid request type\n");
    }
}


void server_run(char *documents_folder, int cache_size) {

    // Initialize cache
    if (cacheInit(cache_size) == -1) {
        puts("Failed to initialize cache\n");
        exit(EXIT_FAILURE);
    }

    // Create request pipe
    create_pipe(REQUEST_PIPE);
    int request_pipe_fd = open_pipe(REQUEST_PIPE, O_RDWR);

    while (run) {
        // Receive request
        Packet *request = receive_packet(request_pipe_fd);
        if (request != NULL) {
            handle_request(request, documents_folder);
            delete_packet(request);
        } else {
            // Error receiving request
            puts("Failed to receive request\n");
        }
    }

    while (wait(NULL) > 0);

    if(cacheDestroy() == -1) {
        printf("Failed to destroy cache. Some data was not written to disk\n");
    }

    // Close request pipe
    close_pipe(request_pipe_fd);
    delete_pipe(REQUEST_PIPE);
}

int main(int argc, char **argv) {
    // Validate arguments
    if (!validate_args(argc, argv)) {
        exit(EXIT_FAILURE);
    }

    // Parse arguments
    char *documents_folder = argv[1];
    int cache_size = argc == 3 ? atoi(argv[2]) : 0;

    // Run server
    server_run(documents_folder, cache_size);

    return 0;
}

