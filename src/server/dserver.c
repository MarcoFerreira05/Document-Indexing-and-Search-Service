#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

int run = 1;

void handle_add_document(Packet *request) {
    char response_pipe[MAX_PIPE_SIZE];
    snprintf(response_pipe, MAX_PIPE_SIZE, RESPONSE_PIPE_TEMPLATE, request->src_pid);
    int response_pipe_fd = open_pipe(response_pipe, O_WRONLY);
    int key = 3223423;
    Packet *response = create_packet(SUCCESS, -1, key, -1, NULL,
                                     NULL, NULL, NULL, NULL);
    send_packet(response, response_pipe_fd);
    delete_packet(response);
    close_pipe(response_pipe_fd);
}

void handle_query_document(Packet *request) {
    char response_pipe[MAX_PIPE_SIZE];
    snprintf(response_pipe, MAX_PIPE_SIZE, RESPONSE_PIPE_TEMPLATE, request->src_pid);
    int response_pipe_fd = open_pipe(response_pipe, O_WRONLY);
    Packet *response = create_packet(SUCCESS, -1, -1, -1, NULL,
                                     "Biblia", "Apostolos", "0034", "src/biblia.txt");
    send_packet(response, response_pipe_fd);
    delete_packet(response);
    close_pipe(response_pipe_fd);
}

void handle_delete_document(Packet *request) {
    char response_pipe[MAX_PIPE_SIZE];
    snprintf(response_pipe, MAX_PIPE_SIZE, RESPONSE_PIPE_TEMPLATE, request->src_pid);
    int response_pipe_fd = open_pipe(response_pipe, O_WRONLY);
    Packet *response = create_packet(SUCCESS, -1, -1, -1, NULL,
                                     NULL, NULL, NULL, NULL);
    send_packet(response, response_pipe_fd);
    delete_packet(response);
    close_pipe(response_pipe_fd);
}

void handle_count_lines(Packet *request) {
    char response_pipe[MAX_PIPE_SIZE];
    snprintf(response_pipe, MAX_PIPE_SIZE, RESPONSE_PIPE_TEMPLATE, request->src_pid);
    int response_pipe_fd = open_pipe(response_pipe, O_WRONLY);
    int lines = 34;
    Packet *response = create_packet(SUCCESS, -1, -1, lines, NULL,
                                     NULL, NULL, NULL, NULL);
    send_packet(response, response_pipe_fd);
    delete_packet(response);
    close_pipe(response_pipe_fd);
}

void handle_search_documents(Packet *request) {
    char response_pipe[MAX_PIPE_SIZE];
    snprintf(response_pipe, MAX_PIPE_SIZE, RESPONSE_PIPE_TEMPLATE, request->src_pid);
    
    int response_pipe_fd = open(response_pipe, O_WRONLY); // Open ONCE
    
    for (int i = 1; i <= 5; i++) {
        Packet *response = create_packet(SUCCESS, -1, i, -1, NULL,
                                         NULL, NULL, NULL, NULL);
        send_packet(response, response_pipe_fd);
        delete_packet(response);
    }
    close_pipe(response_pipe_fd);
}

void handle_request(Packet *request) {

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
            handle_count_lines(request);
            break;
        case SEARCH_DOCUMENTS:
            handle_search_documents(request);
            break;
        default:
            perror("Invalid request type\n");
    }

    int request_pipe_fd = open_pipe(REQUEST_PIPE, O_WRONLY);
    Packet *kill_request = create_packet(TERMINATE_CHILD, getpid(), -1, -1, NULL,
                                         NULL, NULL, NULL, NULL);
    send_packet(kill_request, request_pipe_fd);
    delete_packet(kill_request);
    close_pipe(request_pipe_fd);
}


void server_run(char *documents_folder, int cache_size) {

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
                
                if (request->code == SHUTDOWN_SERVER) {
                    run = 0;
                    Packet *response = create_packet(SUCCESS, -1, -1, -1, NULL,
                                                     NULL, NULL, NULL, NULL);
                    send_packet(response, response_pipe_fd);
                } else {
                    pid_t pid = fork();
                    if (pid == 0) {
                        // Child process
                        handle_request(request);
                        exit(0);
                    } else if (pid < 0) {
                        // Fork failed
                        perror("Fork failed\n");
                        Packet *response = create_packet(FAILURE, -1, -1, -1, NULL,
                                                         NULL, NULL, NULL, NULL);
                        send_packet(response, response_pipe_fd);
                        delete_packet(response);
                    }
                }
                close_pipe(response_pipe_fd);
            }
        } else {
            // Error receiving request
            perror("Failed to receive request\n");
        }
        delete_packet(request);
    }

    while (wait(NULL) > 0);

    // Close request pipe
    close_pipe(request_pipe_fd);
    delete_pipe(REQUEST_PIPE);
}

int main(int argc, char **argv) {
    // Check for correct number of arguments
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

