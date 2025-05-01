#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <glib.h>

int main(int argc, char **argv) {

    if (argc < 2) {
        fprintf(stderr, "Usage: %s -<option> [args...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char option = argv[1][1];
    pid_t pid = getpid();

    // Create request packet
    Packet *request;
    switch (option)
    {
    case 'a':
        if (argc != 6) {
            fprintf(stderr, "Usage: %s -a <title> <authors> <year> <path>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        request = create_packet(ADD_DOCUMENT, pid, -1, -1, NULL,
                                argv[2], argv[3], argv[4], argv[5], -1);
        break;
    case 'c':
        if (argc != 3) {
            fprintf(stderr, "Usage: %s -c <document_id>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        request = create_packet(QUERY_DOCUMENT, pid, atoi(argv[2]), -1,
                                NULL, NULL, NULL, NULL, NULL, -1);
        break;
    case 'd':
        if (argc != 3) {
            fprintf(stderr, "Usage: %s -d <document_id>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        request = create_packet(DELETE_DOCUMENT, pid, atoi(argv[2]), -1,
                                NULL, NULL, NULL, NULL, NULL, -1);
        break;
    case 'l':
        if (argc != 4) {
            fprintf(stderr, "Usage: %s -l <document_id> <keyword>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        request = create_packet(COUNT_LINES, pid, atoi(argv[2]), -1, argv[3],
                                NULL, NULL, NULL, NULL, -1);
        break;
    case 's':
        if (argc < 3 || argc > 4) {
            fprintf(stderr, "Usage: %s -s <keyword> <n_procs>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        int n_procs = (argc == 4) ? atoi(argv[3]) : 1;
        request = create_packet(SEARCH_DOCUMENTS, pid, -1, -1, argv[2],
                                NULL, NULL, NULL, NULL, n_procs);
        break;
    case 'f':
        if (argc != 2) {
            fprintf(stderr, "Usage: %s -f\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        request = create_packet(SHUTDOWN_SERVER, pid, -1, -1,
                                NULL, NULL, NULL, NULL, NULL, -1);
        break;
    default:
        fprintf(stderr, "\n   -a  |  add document\n");
        fprintf(stderr, "\n   -c  |  consult document\n");
        fprintf(stderr, "\n   -d  |  delete document\n");
        fprintf(stderr, "\n   -l  |  count lines\n");
        fprintf(stderr, "\n   -s  |  search documents\n");
        fprintf(stderr, "\n   -f  |  shutdown server\n");
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
            GArray *keys = g_array_new(FALSE, FALSE, sizeof(int));

            do {
                g_array_append_val(keys, response->key);
                delete_packet(response);
                response = receive_packet(response_pipe_fd);
            } while (response != NULL);

            for (int i = 0; i < keys->len; i++) {
                if (i == 0) {
                    printf("[%d, ", g_array_index(keys, int, i));
                }
                else if (i == keys->len - 1) {
                    printf("%d]\n", g_array_index(keys, int, i));
                }
                else printf("%d, ", g_array_index(keys, int, i));
            }

            g_array_free(keys, FALSE);
        } else {
            printf("No documents were found\n");
        }
        break;

    case 'f':
        if (response->code == SUCCESS) {
            printf("Server is shutting down\n");
        } else {
            printf("Failed to shutdown server\n");
        }
        break;

    default:
        fprintf(stderr, "Invalid option\n");
        exit(EXIT_FAILURE);
        break;
    }

    // Clean up
    delete_packet(response);
    close_pipe(response_pipe_fd);
    delete_pipe(response_pipe);

    return 0;
}
