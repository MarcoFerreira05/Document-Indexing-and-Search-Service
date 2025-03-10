#ifndef DCLIENT_H
#define DCLIENT_H

#include "protocol.h"

// Send request to server and get response
Response send_request_and_get_response(Request req);

// Process server response
void process_response(Response resp, RequestType req_type);

#endif
