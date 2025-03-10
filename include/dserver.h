#ifndef DSERVER_H
#define DSERVER_H

#include "protocol.h"
#include "metadata.h"
#include "search.h"
#include "cache.h"
#include "persistence.h"

// Server initialization
void init_server(char *doc_folder, int cache_size);

// Create named pipes
void create_named_pipes();

// Main server loop
void server_loop();

// Process client request
Response process_request(Request req);

// Cleanup resources
void cleanup_resources();

#endif
