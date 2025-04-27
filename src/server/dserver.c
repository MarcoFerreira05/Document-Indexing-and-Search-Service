#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define LOCK_FILE = "server.lock"

int run = 1;

void handle_add_document(Packet *request) {
    int document_id = 3223423;
    Packet *response = create_packet(SUCCESS, REQUEST_PIPE, document_id, -1, NULL, NULL);
    send_packet(response, request->response_pipe);
}

void handle_query_document(Packet *request) {
    char *metadata[METADATA_FIELDS_COUNT] = {"Biblia",
                                             "Apostolos",
                                             "0034",
                                             "src/books/biblia.txt"};
    Packet *response = create_packet(SUCCESS, REQUEST_PIPE, -1, -1, NULL, metadata);
    send_packet(response, request->response_pipe);
}

void handle_delete_document(Packet *request) {
    Packet *response = create_packet(SUCCESS, REQUEST_PIPE, -1, -1, NULL, NULL);
    send_packet(response, request->response_pipe);
}

void handle_count_lines(Packet *request) {
    int lines = 34;
    Packet *response = create_packet(SUCCESS, REQUEST_PIPE, -1, lines, NULL, NULL);
    send_packet(response, request->response_pipe);
}

void handle_search_documents(Packet *request) {
    int document_ids[4] = {00001, 00002, 00003, 00004};
    Packet *response;
    for (int i = 0; i < 4; i++) {
        response = create_packet(SUCCESS, REQUEST_PIPE, document_ids[i], -1, NULL, NULL);
        debug_packet("[ Response sent by server ]", response);
        send_packet(response, request->response_pipe);
    }
    response = create_packet(SUCCESS, NULL, -1, -1, NULL, NULL);
    debug_packet("[ Response sent by server ]", response);
    send_packet(response, request->response_pipe);
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
}


void server_run(char *documents_folder, int cache_size) {

    // Create request pipe
    create_pipe(REQUEST_PIPE);

    while (run) {
        // Receive request
        Packet *request = receive_packet(REQUEST_PIPE);
        // debug_packet("[ Request received by server ]", request); // debug

        if (request != NULL) {
            if (request->code == SHUTDOWN_SERVER) {
                run = 0;
            }
            else {
                pid_t pid = fork();
                if (pid == 0) {
                    // Child process
                    handle_request(request);
                    exit(0);
                } else if (pid < 0) {
                    // Fork failed
                    perror("Fork failed\n");
                    Packet *response = create_packet(FAILURE, REQUEST_PIPE, -1, -1, NULL, NULL);
                    send_packet(response, request->response_pipe);
                    // debug_packet("[ Response sent by server ]", response); // debug
                }
            }
        } else {
            // Error receiving request
            perror("Failed to receive request\n");
        }
    }

    while (wait(NULL) > 0);

    // Close request pipe
    close_pipe(REQUEST_PIPE);
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

