#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <sys/types.h>

// Maximum sizes for various fields
#define MAX_TITLE_SIZE 200
#define MAX_AUTHORS_SIZE 200
#define MAX_YEAR_SIZE 4
#define MAX_PATH_SIZE 64
#define MAX_KEYWORD_SIZE 64
#define MAX_RESPONSE_SIZE 1024
#define MAX_FIFO_PATH 64

#define REQUEST_PIPE "/tmp/request_pipe"
#define RESPONSE_PIPE "/tmp/response_pipe"

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
    int doc_id;          // Document ID (for operations that need it)
    char data[512];      // Data payload (varies by request type)
    int num_processes;   // Number of processes for parallel search
    pid_t client_pid;    // Client PID for identifying the client
} Request;

// Response structure
typedef struct {
    int status;          // Status code (0 for success, error code otherwise)
    int doc_id;          // Document ID (for ADD_DOCUMENT response)
    char data[1024];     // Data payload (varies by response type)
    int count;           // Count (for COUNT_LINES response)
    int doc_ids[100];    // Document IDs (for SEARCH_DOCUMENTS response)
    int num_results;     // Number of results in doc_ids
    pid_t client_pid;    // Client PID to identify which client the response is for
} Response;

// Helper functions for protocol operations
int create_pipes();
int send_request(Request *req);
int receive_request(Request *req);
int send_response(Response *res);
int receive_response(Response *res, pid_t client_pid);

#endif // PROTOCOL_H

