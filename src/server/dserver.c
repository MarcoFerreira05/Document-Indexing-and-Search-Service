#include "include/common/protocol.h"
#include "index.h"

#include <stdio.h>
#include <stdlib.h>

// static Cache cache = NULL;
static Index *index = NULL;
static int run = 1;

int handle_add_document(Request *request, Response *response) {
    char **metadata = request->metadata;
    int document_id = index_add(index, metadata);
    if (document_id < 0) {
        perror("Failed to add document metadata to index\n");
        return -1;
    }
    response->document_id = document_id;
    return 0;
}

int handle_query_document(Request *request, Response *response) {
    int document_id = request->document_id;

}

int handle_delete_document(Request *request, Response *response) {

}

int handle_shutdown_server(Request *request, Response *response) {
    run = 0;
    response->status = 0;
    return 0;
}

int handle_request(Request *request) {
    Response *response = (Response*)malloc(sizeof(Response));
    response->client_pid = request->client_pid;
    
    switch (request->type) {
        case ADD_DOCUMENT:
            response->status = handle_add_document(request, response);
            break;
        case QUERY_DOCUMENT:
            response->status = handle_query_document(request, response);
            break;
        case DELETE_DOCUMENT:
            response->status = handle_delete_document(request, response);
            break;
        case COUNT_LINES:
            response->status = handle_shutdown_server(request, response); // debug
            break;
        case SEARCH_DOCUMENTS:
            response->status = handle_shutdown_server(request, response); // debug
            break;
        case SEARCH_PARALLEL:
            response->status = handle_shutdown_server(request, response); // debug
            break;
        case SHUTDOWN_SERVER:
            response->status = handle_shutdown_server(request, response);
            break;
        default:
            perror("Invalid request type\n");
            return -1;
    }
    
    // Send response (defined in protocol.h)
    if (send_response(response, request->client_pid) != 0) {
        perror("Failed to send response\n");
        return -1;
    }
    
    return 0;
}

void server_run(char *documents_folder) {

    // Create request pipe
    if (create_request_pipe() != 0) {
        perror("Failed to create request pipe\n");
        exit(EXIT_FAILURE);
    }

    // Initialize index
    index = index_init();
    if (index_init() == NULL) {
        perror("Failed to initialize index\n");
        exit(EXIT_FAILURE);
    }

    while (run) {
        Request *request;

        // Receive request
        int result = receive_request(request);
        if (result == 0) {
            // Handle request
            handle_request(request);
        } else if (result < 0) {
            // Error receiving request
            perror("Failed to receive request\n");
        }
        
        // Small delay to prevent CPU hogging ??
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
    server_run(documents_folder);

    return 0;
}

