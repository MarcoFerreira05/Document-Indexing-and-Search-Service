#include "include/common/protocol.h"
#include "index.h"

#include <stdio.h>
#include <stdlib.h>

// static Cache cache = NULL;
static Index *index = NULL;
static int run = 1;
static id = 1;

Packet *handle_add_document(Packet *request) {
    // ...
    return create_packet(ADD_DOCUMENT, SUCCESS, request->response_pipe,
                         id++, NULL, request->client_pid);
}

Packet *handle_query_document(Packet *request) {
    return create_packet(QUERY_DOCUMENT, SUCCESS, request->response_pipe,
                         id++, NULL, request->client_pid);

}

Packet *handle_delete_document(Packet *request) {
    return create_packet(DELETE_DOCUMENT, SUCCESS, request->response_pipe,
                         id++, NULL, request->client_pid);
}

Packet *handle_shutdown_server(Packet *request) {
    run = 0;
    return create_packet(SHUTDOWN_SERVER, SUCCESS, request->response_pipe,
                         id++, NULL, request->client_pid);
}

int handle_request(Packet *request) {
    
    Packet *response;

    switch (request->type) {
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
            return -1;
    }
    
    if (send_packet(response, request->response_pipe) != 0) {
        perror("Failed to send response\n");
        return -1;
    }
    
    return 0;
}

void server_run(char *documents_folder, int cache_size) {

    // Initialize index
    index = index_init();
    if (index_init() == NULL) {
        perror("Failed to initialize index\n");
        exit(EXIT_FAILURE);
    }

    // Create request pipe
    if (create_pipe(REQUEST_PIPE) != 0) {
        perror("Failed to create request pipe\n");
        exit(EXIT_FAILURE);
    }

    while (run) {
        Packet *request = receive_packet(REQUEST_PIPE);

        // Receive request
        if (request != NULL) {
            // Handle request
            handle_request(request);
        } else {
            // Error receiving request
            perror("Failed to receive request\n");
        }

        delete_packet(request);

        // Small delay to prevent CPU hogging (TBC)
        // usleep(10000);  // 10ms
    }

    // Close request pipe
    unlink(REQUEST_PIPE);
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

