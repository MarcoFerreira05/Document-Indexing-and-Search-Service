#ifndef PROTOCOL_H
#define PROTOCOL_H

#define REQUEST_PIPE "/tmp/request_pipe"
#define RESPONSE_PIPE "/tmp/response_pipe"

typedef enum {
    ADD_DOCUMENT,       // Add a new document
    QUERY_DOCUMENT,     // Query document metadata
    DELETE_DOCUMENT,    // Delete document metadata
    COUNT_LINES,        // Count lines containing keyword
    SEARCH_DOCUMENTS,   // Search documents containing keyword
    SEARCH_PARALLEL,    // Search documents in parallel
    SHUTDOWN_SERVER     // Shutdown server
} RequestType;

typedef struct {
    RequestType type;    // Type of request
    int doc_id;          // Document ID (for operations that need it)
    char data[512];      // Data payload (varies by request type)
    int num_processes;   // Number of processes for parallel search
} Request;

typedef struct {
    int status;          // Status code (0 for success, error code otherwise)
    int doc_id;          // Document ID (for ADD_DOCUMENT response)
    char data[1024];     // Data payload (varies by response type)
    int count;           // Count (for COUNT_LINES response)
    int doc_ids[100];    // Document IDs (for SEARCH_DOCUMENTS response)
    int num_results;     // Number of results in doc_ids
} Response;

// Function to initialize a request structure
Request init_request(RequestType type, int doc_id, const char *data, int num_processes);

// Function to initialize a response structure
Response init_response(int status, int doc_id, const char *data,
                       int count, const int *doc_ids, int num_results);



#endif
