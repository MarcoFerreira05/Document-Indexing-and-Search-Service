#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <unistd.h>
#include <sys/types.h>

// Maximum sizes for various fields
#define MAX_PIPE_SIZE 256

#define REQUEST_PIPE "/tmp/request_pipe"
#define RESPONSE_PIPE "/tmp/response_pipe_%d"

// Command types
typedef enum {
    ADD_DOCUMENT = 1,    // Add a new document
    QUERY_DOCUMENT,      // Query document metadata
    DELETE_DOCUMENT,     // Delete document metadata
    COUNT_LINES,         // Count lines containing keyword
    SEARCH_DOCUMENTS,    // Search documents containing keyword
    SEARCH_PARALLEL,     // Search documents in parallel
    SHUTDOWN_SERVER      // Shutdown server
} RequestType;

// Request structure
typedef struct {
    RequestType type;    // Type of request
    char **metadata;   // Metadata associated with the request
    int document_id; // Number of documents
    pid_t client_pid;    // Client PID for identifying the client
} Request;

// Response structure
typedef struct {
    int status;          // Status code (0 for success, error code otherwise)
    char **metadata;   // Metadata associated with the request
    int document_id; // Number of documents
    pid_t client_pid;    // Client PID for identifying the client
} Response;

// Helper functions for protocol operations
int create_request_pipe();
int create_response_pipe(pid_t client_pid);
int send_request(Request *req);
int receive_request(Request *req);
int send_response(Response *res, pid_t client_pid);
int receive_response(Response *res, pid_t client_pid);
int close_request_pipe();
int close_response_pipe(pid_t client_pid);

#endif // PROTOCOL_H

