#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

/*
EXAMPLE USAGE:
dclient -a "title" "authors" "year" "path"      ||      243123
dclient -c 243123                               ||      biblia jesus 0034 src/...
dclient -d 243123                               ||      Document deleted
dclient -l 243123 "teste"                       ||      34
dclient -s "teste"                              ||      1 2 3 4 5
*/

int main(int argc, char **argv) {
    if (argc < 2 || argc > 6) {
        fprintf(stderr, "Usage: %s <option> <arguments>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char option = argv[1][1];
    pid_t pid = getpid();


    // Create request packet
    Packet *request;
    switch (option)
    {
    case 'a':
        request = create_packet(ADD_DOCUMENT, pid, -1, -1, NULL, argv[2], argv[3], argv[4], argv[5], -1);
        break;
    case 'c':
        request = create_packet(QUERY_DOCUMENT, pid, atoi(argv[2]), -1, NULL, NULL, NULL, NULL, NULL, -1);
        break;
    case 'd':
        request = create_packet(DELETE_DOCUMENT, pid, atoi(argv[2]), -1,  NULL, NULL, NULL, NULL, NULL, -1);
        break;
    case 'l':
        request = create_packet(COUNT_LINES, pid, atoi(argv[2]), -1, argv[3], NULL, NULL, NULL, NULL, -1);
        break;
    case 's':
        request = create_packet(SEARCH_DOCUMENTS, pid, -1, -1, argv[2], NULL, NULL, NULL, NULL, atoi(argv[3]));
        break;
    case 'f':
        request = create_packet(SHUTDOWN_SERVER, pid, -1, -1, NULL, NULL, NULL, NULL, NULL, -1);
        break;
    default:
        perror("Invalid option\n");
        exit(EXIT_FAILURE);
        break;
    }

    // Create response pipe
    char response_pipe[MAX_PIPE_SIZE];
    snprintf(response_pipe, MAX_PIPE_SIZE, RESPONSE_PIPE_TEMPLATE, pid);
    create_pipe(response_pipe);

    // Send request
    int request_pipe_fd = open_pipe(REQUEST_PIPE, O_WRONLY);
    send_packet(request, request_pipe_fd);
    delete_packet(request);
    close_pipe(request_pipe_fd);

    // Receive response
    int response_pipe_fd = open_pipe(response_pipe, O_RDONLY);
    Packet *response = receive_packet(response_pipe_fd);

    // Process response
    switch (option)
    {
    case 'a':
        if (response->code == SUCCESS) {
            printf("Document %d indexed\n", response->key);
        } else {
            printf("Failed to add document\n");
        }
        break;

    case 'c':
        if (response->code == SUCCESS) {
            printf("Title: %s\nAuthors: %s\nYear: %s\nPath %s\n",
                   response->title, response->authors, response->year, response->path);
        } else {
            printf("Failed to find document\n");
        }
        break;

    case 'd':
        if (response->code == SUCCESS) {
            printf("Index entry %d deleted\n", response->key);
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
            do {
                printf("%d\n", response->key);
                delete_packet(response);
                response = receive_packet(response_pipe_fd);
            } while (response != NULL);
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

    // Clean up
    delete_packet(response);
    close_pipe(response_pipe_fd);
    delete_pipe(response_pipe);

    return 0;
}