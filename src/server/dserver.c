#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int run = 1;

Packet *handle_add_document(Packet *request) {
    // ...
    Packet *response = create_packet(SUCCESS, REQUEST_PIPE, 123456, NULL);
    return response;
}

Packet *handle_query_document(Packet *request) {
    // ...
    return create_packet(SUCCESS, REQUEST_PIPE, 123456, NULL);
}

Packet *handle_delete_document(Packet *request) {
    // ...
    return create_packet(SUCCESS, REQUEST_PIPE, 123456, NULL);
}

Packet *handle_shutdown_server(Packet *request) {
    run = 0;
    return create_packet(SUCCESS, REQUEST_PIPE, 123456, NULL);
}

Packet *handle_request(Packet *request) {
    Packet *response;

    switch (request->code) {
        case ADD_DOCUMENT:
            response = handle_add_document(request);
            break;
        case QUERY_DOCUMENT:
            response = handle_query_document(request);
            break;
        case DELETE_DOCUMENT:
            response =handle_delete_document(request);
            break;
        case SHUTDOWN_SERVER:
            response = handle_shutdown_server(request);
            break;
        default:
            perror("Invalid request type\n");
            return NULL;
    }
    return response;
}

void server_run(char *documents_folder, int cache_size) {

    // Create request pipeç
    create_pipe(REQUEST_PIPE);

    while (run) {
        // Receive request
        Packet *request = receive_packet(REQUEST_PIPE);
        debug_packet("[ Request received by server ]", request); // debug

        if (request != NULL) {
            // Handle request
            Packet *response = handle_request(request);
            if (response != NULL) {
                debug_packet("[ Response sent by server ]", response); // debug
                send_packet(response, request->response_pipe);
            } else {
                // Error handling
                perror("Failed to handle request\n");
            }
        } else {
            // Error receiving request
            perror("Failed to receive request\n");
        }
    }

    // Close request pipe
    close_pipe(REQUEST_PIPE);
}

int main(int argc, char **argv) {
    // Check for correct number of arguments
    if (argc < 2 && argc > 3) {
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

