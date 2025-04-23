#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <unistd.h>
#include <sys/types.h>

// Maximum sizes for various fields
#define MAX_PIPE_SIZE 256
#define MAX_METADATA_FIELDS 4
#define MAX_FIELDS_SIZE 256

#define REQUEST_PIPE "request_pipe"
#define RESPONSE_PIPE_TEMPLATE "response_pipe_%d"

// Request types
typedef enum {
    ADD_DOCUMENT = 1,    // Add a new document
    QUERY_DOCUMENT,      // Query document metadata
    DELETE_DOCUMENT,     // Delete document metadata
    COUNT_LINES,         // Count lines containing keyword
    SEARCH_DOCUMENTS,    // Search documents containing keyword
    SEARCH_PARALLEL,     // Search documents in parallel
    SHUTDOWN_SERVER,     // Shutdown server

    SUCCESS,             // Success response
    FAILURE              // Failure response
} Code;

// Packet struct
typedef struct {
    Code code;
    char response_pipe[MAX_PIPE_SIZE];
    int document_id;
    char metadata[MAX_METADATA_FIELDS][MAX_FIELDS_SIZE];
} Packet;

// Helper functions for protocol operations
int create_pipe(char *pipe_name);
int close_pipe(char *pipe_name);
Packet *create_packet(Code code, char *response_pipe,
                      int document_id, char **metadata);
void delete_packet(Packet *packet);
int send_packet(Packet *packet, char *pipe_name);
Packet *receive_packet(char *pipe_name);
void debug_packet(char *title, Packet *packet); // debug

#endif
