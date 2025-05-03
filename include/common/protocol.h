#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

/**
 * @file protocol.h
 * @brief Header file defining the protocol for inter-process communication using named pipes.
 */

// Pipe Configuration
#define MAX_PIPE_SIZE 64               ///< Maximum size for pipe names.
#define REQUEST_PIPE "tmp/request_pipe"    ///< Name of the request pipe.
#define RESPONSE_PIPE_TEMPLATE "tmp/response_pipe_%d" ///< Template for response pipe names, where %d is replaced by the process ID.

// Field Size Limits
#define MAX_KEYWORD_SIZE 32   ///< Maximum size for keywords.
#define MAX_TITLE_SIZE 190    ///< Maximum size for document titles.
#define MAX_AUTHORS_SIZE 190  ///< Maximum size for authors' names.
#define MAX_YEAR_SIZE 5       ///< Maximum size for year (e.g., "2023").
#define MAX_PATH_SIZE 64      ///< Maximum size for file paths.

/**
 * @brief Enumeration of request and response codes.
 */
typedef enum code {
    ADD_DOCUMENT = 0,       ///< Request to add a new document.
    QUERY_DOCUMENT,         ///< Request to query document metadata.
    DELETE_DOCUMENT,        ///< Request to delete document metadata.
    COUNT_LINES,            ///< Request to count lines containing a keyword.
    SEARCH_DOCUMENTS,       ///< Request to search documents containing a keyword.
    SHUTDOWN_SERVER,        ///< Request to shutdown the server.
    TERMINATE_CHILD,        ///< Request to terminate a child process.

    SUCCESS = 100,          ///< Response indicating success.
    FAILURE                 ///< Response indicating failure.
} Code;

/**
 * @brief Structure representing a packet sent or received through named pipes.
 */
typedef struct packet {
    Code code;                      ///< Request or response code.
    int src_pid;                    ///< Process ID of the sender.
    int key;                        ///< Key for identifying the document or request.
    int lines;                      ///< Number of lines.
    char keyword[MAX_KEYWORD_SIZE]; ///< Keyword for search or count operations.
    char title[MAX_TITLE_SIZE];     ///< Title of the document.
    char authors[MAX_AUTHORS_SIZE]; ///< Authors of the document.
    char year[MAX_YEAR_SIZE];       ///< Publication year of the document.
    char path[MAX_PATH_SIZE];       ///< File path of the document.
    int n_procs;                    ///< Number of processes.
} Packet;

/**
 * @brief Create a named pipe.
 * @param pipe_name The name of the pipe to create.
 * @return 0 on success, -1 on failure.
 */
int create_pipe(char *pipe_name);

/**
 * @brief Delete a named pipe.
 * @param pipe_name The name of the pipe to delete.
 * @return 0 on success, -1 on failure.
 */
int delete_pipe(char *pipe_name);

/**
 * @brief Open a named pipe.
 * @param pipe_name The name of the pipe to open.
 * @param flags The flags to use when opening the pipe (e.g., O_RDONLY, O_WRONLY).
 * @return The file descriptor of the opened pipe on success, -1 on failure.
 */
int open_pipe(char *pipe_name, int flags);

/**
 * @brief Close a named pipe.
 * @param pipe_fd The file descriptor of the pipe to close.
 * @return 0 on success, -1 on failure.
 */
int close_pipe(int pipe_fd);

/**
 * @brief Create a new packet.
 * @param code The request or response code.
 * @param src_pid The process ID of the sender.
 * @param key The key for identifying the document or request.
 * @param lines The number of lines where the keyword is found.
 * @param keyword The keyword for search or count operations.
 * @param title The title of the document.
 * @param authors The authors of the document.
 * @param year The publication year of the document.
 * @param path The relative path of the document.
 * @param n_procs The number of concurrent processes for search operations.
 * @return A pointer to the created packet on success, NULL on failure.
 */
Packet *create_packet(Code code, pid_t src_pid, int key, int lines, char *keyword,
                      char *title, char *authors, char *year, char *path, int n_procs);

/**
 * @brief Delete a packet.
 * @param packet The packet to delete.
 * @return 0 on success, -1 on failure.
 */
int delete_packet(Packet *packet);

/**
 * @brief Send a packet through a named pipe.
 * @param packet The packet to send.
 * @param pipe_fd The file descriptor of the pipe to send the packet through.
 * @return 0 on success, -1 on failure.
 */
int send_packet(Packet *packet, int pipe_fd);

/**
 * @brief Receive a packet from a named pipe.
 * @param pipe_fd The file descriptor of the pipe to receive the packet from.
 * @return A pointer to the received packet on success, NULL on failure.
 */
Packet *receive_packet(int pipe_fd);

#endif // PROTOCOL_H
