#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <unistd.h>
#include <sys/types.h>

// Maximum sizes for various fields
#define MAX_PIPE_SIZE 256
#define METADATA_FIELDS_COUNT 4
#define MAX_FIELDS_SIZE 256
#define MAX_KEYWORD_SIZE 64
#define OPTIONS_COUNT 6

// Pipe names
#define REQUEST_PIPE "request_pipe"
#define REQUEST_PIPE_TEMPLATE "request_pipe%d"
#define RESPONSE_PIPE_TEMPLATE "response_pipe%d"

// Request types
typedef enum {
    ADD_DOCUMENT = 0,   // Add a new document
    QUERY_DOCUMENT,      // Query document metadata
    DELETE_DOCUMENT,     // Delete document metadata
    COUNT_LINES,         // Count lines containing keyword
    SEARCH_DOCUMENTS,    // Search documents containing keyword
    SHUTDOWN_SERVER,     // Shutdown server

    SUCCESS = 100,       // Success response
    ACKNOWLEDGMENT,       // Acknowledge a packet was recieved
    LAST_FRAG,           // Marks the last fragment
    FAILURE              // Failure response
} Code;

// Packet struct
typedef struct {
    Code code;
    char response_pipe[MAX_PIPE_SIZE];
    int document_id;
    int lines;
    char keyword[MAX_KEYWORD_SIZE];
    char metadata[METADATA_FIELDS_COUNT][MAX_FIELDS_SIZE];
} Packet;

// Helper functions for protocol operations
int create_pipe(char *pipe_name);
int close_pipe(char *pipe_name);
Packet *create_packet(Code code, char *response_pipe, int document_id,
                      int lines, char *keyword, char **metadata);
void delete_packet(Packet *packet);
int send_packet(Packet *packet, char *pipe_name);
Packet *receive_packet(char *pipe_name);
void debug_packet(char *title, Packet *packet); // debug

#endif
