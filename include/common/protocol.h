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
#define MAX_AUTHORS_SIZE 200
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
    KILL_CHILD,        // Kill child process

    SUCCESS = 100,     // Success response
    FAILURE,           // Failure response
    ACKNOWLEDGE,       // Acknowledge response
    LAST_FRAG          // Marks as fragment
} Code;

typedef struct { // of size 508 bytes
    Code code;
    pid_t src_pid;
    int key;
    int lines;
    char keyword[MAX_KEYWORD_SIZE];
    char title[MAX_TITLE_SIZE];
    char authors[MAX_AUTHORS_SIZE];
    char year[MAX_YEAR_SIZE];
    char path[MAX_PATH_SIZE];
} Packet;


// Helper functions for protocol operations
int create_pipe(char *pipe_name);
int close_pipe(char *pipe_name);
Packet *create_packet(Code code, pid_t src_pid, int key, int lines, char *keyword,
                      char *title, char *authors, char *year, char *path);
void delete_packet(Packet *packet);
int send_packet(Packet *packet, char *pipe_name);
Packet *receive_packet(char *pipe_name);
void debug_packet(char *header, Packet *packet); // debug

#endif
