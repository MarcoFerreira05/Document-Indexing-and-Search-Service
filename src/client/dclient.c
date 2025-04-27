#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
dclient -a "title" "authors" "year" "path"      ||      243123
dclient -c 243123                               ||      "title" "authors" "year" "path"
dclient -d 243123                               ||      Document deleted
dclient -l 243123 "teste"                       ||      34
dclient -s "teste"                              ||      [2321893, 1237321, 3423556]
*/

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <option> <arguments>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char option = argv[1][1];
    pid_t pid = getpid();

    // Create response pipe
    char response_pipe[MAX_PIPE_SIZE];
    snprintf(response_pipe, MAX_PIPE_SIZE, RESPONSE_PIPE_TEMPLATE, pid);

    // Create request packet
    Packet *request;
    switch (option)
    {
    case 'a':
        request = create_packet(ADD_DOCUMENT, response_pipe, -1, -1, NULL, &argv[2]);
        break;
    case 'c':
        request = create_packet(QUERY_DOCUMENT, response_pipe, atoi(argv[2]), -1, NULL, NULL);
        break;
    case 'd':
        request = create_packet(DELETE_DOCUMENT, response_pipe, atoi(argv[2]), -1, NULL, NULL);
        break;
    case 'l':
        request = create_packet(COUNT_LINES, response_pipe, atoi(argv[2]), -1, argv[3], NULL);
        break;
    case 's':
        request = create_packet(SEARCH_DOCUMENTS, response_pipe, -1, -1, argv[2], NULL);
        break;
    case 'f':
        request = create_packet(SHUTDOWN_SERVER, response_pipe, -1, -1, NULL, NULL);
        break;
    default:
        perror("Invalid option\n");
        exit(EXIT_FAILURE);
        break;
    }

    // Create response pipe and send request
    create_pipe(response_pipe);
    send_packet(request, REQUEST_PIPE);

    // Receive response and close response pipe
    Packet *response = receive_packet(response_pipe);

    // Process response
    switch (option)
    {
    case 'a':
        if (response->code == SUCCESS) {
            printf("%d\n", response->document_id);
        } else {
            printf("Failed to add document\n");
        }
        break;

    case 'c':
        if (response->code == SUCCESS) {
            for(int i = 0; i < METADATA_FIELDS_COUNT; i++) {
                printf("%s\n", response->metadata[i]);
            }
        } else {
            printf("Failed to find document\n");
        }
        break;

    case 'd':
        if (response->code == SUCCESS) {
            printf("Document deleted\n");
        } else {
            printf("Failed to delete document\n");
        }
        break;

    case 'l':
        if (response->code == SUCCESS) {
            printf("%d\n", response->lines);
        } else {
            printf("Failed to count lines\n");
        }
        break;

    case 's':
        if (response->code == SUCCESS) {
            debug_packet("->", response);
            while(response->code != LAST_FRAG) {
                printf("%d\n", response->document_id);
                response = receive_packet(response_pipe);
                debug_packet("->", response);
            }
        } else {
            printf("Failed to search documents\n");
        }
        break;

    case 'f':
        if (response->code == SUCCESS) {
            printf("Server shutting down\n");
        } else {
            printf("Failed to shutdown server\n");
        }
        break;

    default:
        perror("Invalid option\n");
        exit(EXIT_FAILURE);
        break;
    }

    // Delete response packet
    close_pipe(response_pipe);

    return 0;
}