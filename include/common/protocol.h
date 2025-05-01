#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <unistd.h>
#include <sys/types.h>

// Pipe names and max size
#define MAX_PIPE_SIZE 64
#define REQUEST_PIPE "request_pipe"
#define RESPONSE_PIPE_TEMPLATE "response_pipe_%d"

// Maximum sizes for various fields
#define MAX_KEYWORD_SIZE 32
#define MAX_TITLE_SIZE 190
#define MAX_AUTHORS_SIZE 190
#define MAX_YEAR_SIZE 5
#define MAX_PATH_SIZE 64

// Request types
typedef enum {
    ADD_DOCUMENT = 0,  // Add a new document
    QUERY_DOCUMENT,    // Query document metadata
    DELETE_DOCUMENT,   // Delete document metadata
    COUNT_LINES,       // Count lines containing keyword
    SEARCH_DOCUMENTS,  // Search documents containing keyword
    SHUTDOWN_SERVER,   // Shutdown server
    TERMINATE_CHILD,   // Terminates the child process

    SUCCESS = 100,     // Success response
    FAILURE            // Failure response
} Code;

// Packet structure
typedef struct {
    Code code;
    pid_t src_pid;
    int key;
    int lines;
    char keyword[MAX_KEYWORD_SIZE];
    char title[MAX_TITLE_SIZE];
    char authors[MAX_AUTHORS_SIZE];
    char year[MAX_YEAR_SIZE];
    char path[MAX_PATH_SIZE];
    int n_procs;
} Packet;

// Helper functions for protocol operations
int create_pipe(char *pipe_name);
int delete_pipe(char *pipe_name);
int open_pipe(char *pipe_name, int flags);
int close_pipe(int pipe_fd);
Packet *create_packet(Code code, pid_t src_pid, int key, int lines, char *keyword,
                      char *title, char *authors, char *year, char *path, int n_procs);
int delete_packet(Packet *packet);
int send_packet(Packet *packet, int pipe_fd);
Packet *receive_packet(int pipe_fd);
void debug_packet(char *header, Packet *packet); // debug

#endif
