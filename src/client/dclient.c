#include "include/common/protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <option> <arguments>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char option = argv[1][1];
    
    Request *request = (Request*) malloc(sizeof(Request));
    switch (option)
    {
    case 'a':
        request->type = ADD_DOCUMENT;
        request->metadata = &argv[2];
        request->client_pid = getpid();
        send_request(request);
        break;
    case 'c':
        request->type = QUERY_DOCUMENT;
        request->document_id = atoi(argv[2]);
        request->client_pid = getpid();
        send_request(request);
        break;
    case 'd':
        request->type = DELETE_DOCUMENT;
        request->document_id = atoi(argv[2]);
        request->client_pid = getpid();
        send_request(request);
        break;
    default:
        perror("Invalid option\n");
        exit(EXIT_FAILURE);
        break;
    }
    Response *response = (Response*) malloc(sizeof(Response));
    receive_response(response, getpid());
    free(request);
    return 0;
}