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

    char response_pipe[MAX_PIPE_SIZE];
    snprintf(pipe, sizeof(pipe), RESPONSE_PIPE_TEMPLATE, pid);

    Packet *request;
    switch (option)
    {
    case 'a':
        request = create_packet(ADD_DOCUMENT, PENDING, response_pipe,
                                -1, argv[2], pid);
        break;
    case 'c':
        request = create_packet(QUERY_DOCUMENT, PENDING, response_pipe,
                                atoi(argv[2]), NULL, pid);
        break;
    case 'd':
        request = create_packet(DELETE_DOCUMENT, PENDING, response_pipe,
                                atoi(argv[2]), NULL, pid);
        break;
    case 'f':
        request = create_packet(SHUTDOWN_SERVER, PENDING, response_pipe,
                                -1, NULL, pid);
        break;
    default:
        perror("Invalid option\n");
        exit(EXIT_FAILURE);
        break;
    }
    send_packet(request, REQUEST_PIPE);

    Packet *response = receive_packet(response_pipe);

    unlink(response_pipe);

    if (response == NULL) {
        perror("Failed to receive response\n");
        exit(EXIT_FAILURE);
    }

    if (response->status == FAILURE) {
        perror("Request failed\n");
        exit(EXIT_FAILURE);
    }

    delete_packet(response);

    return 0;
}