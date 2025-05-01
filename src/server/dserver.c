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

int run = 1;

void handle_add_document(Packet *request) {

    int key = AddDocument(request->title, request->authors, request->year, request->path);

    char response_pipe[MAX_PIPE_SIZE];
    snprintf(response_pipe, MAX_PIPE_SIZE, RESPONSE_PIPE_TEMPLATE, request->src_pid);
    int response_pipe_fd = open_pipe(response_pipe, O_WRONLY);

    Packet *response = create_packet(SUCCESS, -1, key, -1, NULL,
                                     NULL, NULL, NULL, NULL, -1);
    send_packet(response, response_pipe_fd);
    delete_packet(response);
    close_pipe(response_pipe_fd);
}

void handle_query_document(Packet *request) {

    char **metadata = consultDocument(request->key);

    char response_pipe[MAX_PIPE_SIZE];
    snprintf(response_pipe, MAX_PIPE_SIZE, RESPONSE_PIPE_TEMPLATE, request->src_pid);
    int response_pipe_fd = open_pipe(response_pipe, O_WRONLY);

    Packet *response;
    if (metadata != NULL) {
        response = create_packet(SUCCESS, -1, -1, -1, NULL,
                                 metadata[0], metadata[1], metadata[2], metadata[3], -1);
    } else {
        response = create_packet(FAILURE, -1, -1, -1, NULL,
                                 NULL, NULL, NULL, NULL, -1);
    }
    send_packet(response, response_pipe_fd);
    delete_packet(response);
    close_pipe(response_pipe_fd);
}

void handle_delete_document(Packet *request) {

    Packet *response;
    if (!deleteDocument(request->key)) {
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

    int lines = search_keyword_in_file(request->key, request->keyword, 0, folder_path);

    char response_pipe[MAX_PIPE_SIZE];
    snprintf(response_pipe, MAX_PIPE_SIZE, RESPONSE_PIPE_TEMPLATE, request->src_pid);
    int response_pipe_fd = open_pipe(response_pipe, O_WRONLY);

    Packet *response = create_packet(SUCCESS, -1, -1, lines, NULL,
                                     NULL, NULL, NULL, NULL, -1);
    
    send_packet(response, response_pipe_fd);
    delete_packet(response);
    close_pipe(response_pipe_fd);
}

void handle_search_documents(Packet *request, char *folder_path) {

    GArray *docs_with_kw = NULL;

    if(request->n_procs <= 1) {
        docs_with_kw = docs_with_keyword(request->keyword, folder_path);
    }
    else {
        docs_with_kw = docs_with_keyword_concurrent(request->keyword, request->n_procs, folder_path);
    }

    char response_pipe[MAX_PIPE_SIZE];
    snprintf(response_pipe, MAX_PIPE_SIZE, RESPONSE_PIPE_TEMPLATE, request->src_pid);
    
    int response_pipe_fd = open(response_pipe, O_WRONLY);
    
    for (int i = 1; i <= docs_with_kw->len; i++) {
        Packet *response = create_packet(SUCCESS, -1, g_array_index(docs_with_kw, int, i), -1, NULL,
                                         NULL, NULL, NULL, NULL, -1);
        send_packet(response, response_pipe_fd);
        delete_packet(response);
    }
    close_pipe(response_pipe_fd);

    g_array_free(docs_with_kw, FALSE);
}

void handle_request(Packet *request, char *documents_folder) {

    switch (request->code) {
        case ADD_DOCUMENT:
            handle_add_document(request);
            break;
        case QUERY_DOCUMENT:
            handle_query_document(request);
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
            run = 0;
            break;
        default:
            perror("Invalid request type\n");
    }
}


void server_run(char *documents_folder, int cache_size) {

    // Initialize cache
    if (cacheInit(cache_size) == -1) {
        perror("Failed to initialize cache\n");
        exit(EXIT_FAILURE);
    }

    // Create request pipe
    create_pipe(REQUEST_PIPE);
    int request_pipe_fd = open_pipe(REQUEST_PIPE, O_RDWR);

    while (run) {
        // Receive request
        Packet *request = receive_packet(request_pipe_fd);
        if (request != NULL) {
            if (request->code == TERMINATE_CHILD) {
                waitpid(request->src_pid, NULL, 0);
            } else {
                char response_pipe[MAX_PIPE_SIZE];
                snprintf(response_pipe, MAX_PIPE_SIZE, RESPONSE_PIPE_TEMPLATE, request->src_pid);
                int response_pipe_fd = open_pipe(response_pipe, O_WRONLY);
                
                if (request->code == QUERY_DOCUMENT || request->code == COUNT_LINES || request->code == SEARCH_DOCUMENTS) {
                    pid_t pid = fork();
                    if (pid == 0) {
                        // Child process
                        handle_request(request, documents_folder);
                        int request_pipe_fd = open_pipe(REQUEST_PIPE, O_WRONLY);
                        Packet *kill_request = create_packet(TERMINATE_CHILD, getpid(), -1, -1, NULL,
                                                             NULL, NULL, NULL, NULL, -1);
                        send_packet(kill_request, request_pipe_fd);
                        delete_packet(kill_request);
                        close_pipe(request_pipe_fd);
                        exit(0);
                    } else if (pid < 0) {
                        // Fork failed
                        perror("Fork failed\n");
                        Packet *response = create_packet(FAILURE, -1, -1, -1, NULL,
                                                         NULL, NULL, NULL, NULL, -1);
                        send_packet(response, response_pipe_fd);
                        delete_packet(response);
                    }
                } else {
                    handle_request(request, documents_folder);
                }
                close_pipe(response_pipe_fd);
            }
            delete_packet(request);
        } else {
            // Error receiving request
            perror("Failed to receive request\n");
        }
    }

    while (wait(NULL) > 0);

    if(cacheDestroy() == -1) {
        printf("A destruição da cache correu mal");
    }

    // Close request pipe
    close_pipe(request_pipe_fd);
    delete_pipe(REQUEST_PIPE);
}

int main(int argc, char **argv) {
    // Validate arguments
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s <documents_folder> [cache_size]\n"
                        "Note: <required> | [optional>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Parse arguments
    char *documents_folder = argv[1];
    int cache_size = argc == 3 ? atoi(argv[2]) : 0;

    // Run server
    server_run(documents_folder, cache_size);

    return 0;
}

