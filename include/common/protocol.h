#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <unistd.h>
#include <sys/types.h>

// Maximum sizes for various fields
#define MAX_PIPE_SIZE 256

#define REQUEST_PIPE "/tmp/request_pipe"
#define RESPONSE_PIPE_TEMPLATE "/tmp/response_pipe_%d"

// Request types
typedef enum {
    ADD_DOCUMENT = 1,    // Add a new document
    QUERY_DOCUMENT,      // Query document metadata
    DELETE_DOCUMENT,     // Delete document metadata
    COUNT_LINES,         // Count lines containing keyword
    SEARCH_DOCUMENTS,    // Search documents containing keyword
    SEARCH_PARALLEL,     // Search documents in parallel
    SHUTDOWN_SERVER      // Shutdown server
} RequestType;

// Response statuses
typedef enum {
    SUCCESS = 0,
    FAILURE,
    PENDING
} ResponseStatus;

// Packet struct
typedef struct {
    RequestType type;
    ResponseStatus status;
    char *response_pipe;
    int document_id;
    char **metadata;
    pid_t client_pid;
} Packet;

// Helper functions for protocol operations
int create_pipe(char *pipe_name);
int close_pipe(char *pipe_name);
Packet *create_packet(RequestType type, ResponseStatus status, char *response_pipe,
                      int document_id, char **metadata, pid_t client_pid);
int delete_packet(Packet *packet);
int send_packet(Packet *packet, char *pipe_name);
Packet *receive_packet(char *pipe_name);

#endif
