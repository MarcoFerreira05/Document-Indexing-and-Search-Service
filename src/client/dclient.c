#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
        request = create_packet(ADD_DOCUMENT, response_pipe, -1, &argv[2]);
        break;
    case 'c':
        request = create_packet(QUERY_DOCUMENT, response_pipe, atoi(argv[2]), NULL);
        break;
    case 'd':
        request = create_packet(DELETE_DOCUMENT, response_pipe, atoi(argv[2]), NULL);
        break;
    case 'f':
        request = create_packet(SHUTDOWN_SERVER, response_pipe, -1, NULL);
        break;
    default:
        perror("Invalid option\n");
        exit(EXIT_FAILURE);
        break;
    }

    // Create response pipe and send request
    create_pipe(response_pipe);
    debug_packet("[ Request sent by client ]", request); // debug
    send_packet(request, REQUEST_PIPE);
    printf("Request sent to server\n");

    // Receive response and close response pipe
    Packet *response = receive_packet(response_pipe);
    debug_packet("[ Response received by client ]", response); // debug
    close_pipe(response_pipe);

    // Process response
    // ...

    // Delete response packet
    delete_packet(response);

    return 0;
}