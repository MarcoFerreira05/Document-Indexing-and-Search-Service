#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <glib.h>
#include <ctype.h>

int validate_number(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

int validate_args(int argc, char **argv) {

    if (argc < 2) {
        fprintf(stderr, "Usage: %s -<option> [args...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char option = argv[1][1];

    switch (option)
    {
    case 'a':
        if (argc != 6) {
            fprintf(stderr, "Usage: %s -a <title> <authors> <year> <path>\n", argv[0]);
            return 0;
        }
        break;
    case 'c':
        if (argc != 3 || !validate_number(argv[2])) {
            fprintf(stderr, "Usage: %s -c <document_id>\n", argv[0]);
            return 0;
        }
        break;
    case 'd':
        if (argc != 3 || !validate_number(argv[2])) {
            fprintf(stderr, "Usage: %s -d <document_id>\n", argv[0]);
            return 0;
        }
        break;
    case 'l':
        if (argc != 4 || !validate_number(argv[2])) {
            fprintf(stderr, "Usage: %s -l <document_id> <keyword>\n", argv[0]);
            return 0;
        }
        break;
    case 's':
        if (argc < 3 || argc > 4 || (argc == 4 && !validate_number(argv[3])) ) {
            fprintf(stderr, "Usage: %s -s <keyword> [nr_procs]\n", argv[0]);
            return 0;
        }
        break;
    case 'f':
        if (argc != 2) {
            fprintf(stderr, "Usage: %s -f\n", argv[0]);
            return 0;
        }
        break;
    default:
        fprintf(stderr, "\nOPTIONS:\n");
        fprintf(stderr, "   -a <title> <authors> <year> <path>\nadd document\n");
        fprintf(stderr, "   -c <document_id>\nconsult document\n");
        fprintf(stderr, "   -d <document_id>\ndelete document\n");
        fprintf(stderr, "   -l <document_id> <keyword>\ncount lines\n");
        fprintf(stderr, "   -s <keyword> [nr_procs]\nsearch documents\n");
        fprintf(stderr, "   -f\nshutdown server\n");
        return 0;
    }

    return 1;
}

Packet *create_request(int argc, char **argv, char option, pid_t pid) {

    switch (option)
    {
        case 'a':
            return create_packet(ADD_DOCUMENT, pid, -1, -1, NULL,
                                argv[2], argv[3], argv[4], argv[5], -1);
        case 'c':
            return create_packet(CONSULT_DOCUMENT, pid, atoi(argv[2]), -1, NULL,
                                NULL, NULL, NULL, NULL, -1);
        case 'd':
            return create_packet(DELETE_DOCUMENT, pid, atoi(argv[2]), -1, NULL,
                                NULL, NULL, NULL, NULL, -1);
        case 'l':
            return create_packet(COUNT_LINES, pid, atoi(argv[2]), -1, argv[3],
                                NULL, NULL, NULL, NULL, -1);
        case 's':
            int nr_procs = (argc == 4) ? atoi(argv[3]) : 1;
            return create_packet(SEARCH_DOCUMENTS, pid, -1, -1, argv[2],
                                NULL, NULL, NULL, NULL, nr_procs);
        case 'f':
            return create_packet(SHUTDOWN_SERVER, pid, -1, -1, NULL,
                                NULL, NULL, NULL, NULL, -1);
        default:
            return NULL;
    }

}

void process_response(char *response_pipe, char option) {

    int response_pipe_fd = open_pipe(response_pipe, O_RDONLY);
    Packet *response = receive_packet(response_pipe_fd);

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
            printf("Title: %s\nAuthors: %s\nYear: %s\nPath: %s\n",
                   response->title, response->authors, response->year, response->path);
        } else {
            printf("No document was found\n");
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

            if (response->key == -1) {
                printf("No documents were found\n");  
            } else {
                GArray *keys = g_array_new(FALSE, FALSE, sizeof(int));

                do {
                    g_array_append_val(keys, response->key);
                    delete_packet(response);
                    response = receive_packet(response_pipe_fd);
                } while (response != NULL);
                    for (int i = 0; i < keys->len; i++) {
                        if (i == 0) {
                            printf("[%d, ", g_array_index(keys, int, i));
                        } else if (i == keys->len - 1) {
                            printf("%d]\n", g_array_index(keys, int, i));
                        } else printf("%d, ", g_array_index(keys, int, i));
                }
                g_array_free(keys, FALSE);
            }
        } else {
            printf("Failed to search documents\n");
        }
        break;
    case 'f':
        if (response->code == SUCCESS) {
            printf("Server is shutting down\n");
        } else {
            printf("Failed to shutdown server\n");
        }
        break;
    }

    delete_packet(response);
    close_pipe(response_pipe_fd);
}

int main(int argc, char **argv) {

    if (argc < 2) {
        fprintf(stderr, "Usage: %s -<option> [args...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (!validate_args(argc, argv)) {
        exit(EXIT_FAILURE);
    }

    char option = argv[1][1]; 
    pid_t pid = getpid();

    Packet *request = create_request(argc, argv, option, pid);

    // Create response pipe
    char response_pipe[MAX_PIPE_SIZE];
    snprintf(response_pipe, MAX_PIPE_SIZE, RESPONSE_PIPE_TEMPLATE, pid);
    create_pipe(response_pipe);

    // Send request
    int request_pipe_fd = open_pipe(REQUEST_PIPE, O_WRONLY);
    if (request_pipe_fd == -1) {
        fprintf(stderr, "Server is not running\n");
        delete_pipe(response_pipe);
        exit(EXIT_FAILURE);
    }
    send_packet(request, request_pipe_fd);
    delete_packet(request);
    close_pipe(request_pipe_fd);

    // Process response
    process_response(response_pipe, option);

    // Clean up
    delete_pipe(response_pipe);

    return 0;
}
