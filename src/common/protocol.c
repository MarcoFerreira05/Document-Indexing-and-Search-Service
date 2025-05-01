#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdarg.h>

int create_pipe(char *pipe_name) {
    int fifo = mkfifo(pipe_name, 0666);
    if (fifo < 0) {
        perror("Failed to create pipe\n");
        return -1;
    }
    return 0;
}

int delete_pipe(char *pipe_name) {
    int result = unlink(pipe_name);
    if (result < 0) {
        perror("Failed to delete pipe\n");
        return -1;
    }
    return 0;
}

int open_pipe(char *pipe_name, int flags) {
    int fd = open(pipe_name, flags);
    if (fd < 0) {
        perror("Failed to open pipe fd\n");
        return -1;
    }
    return fd;
}

int close_pipe(int pipe_fd) {
    int result = close(pipe_fd);
    if (result < 0) {
        perror("Failed to close pipe fd\n");
        return -1;
    }
    return 0;
}
 
Packet *create_packet(Code code, pid_t src_pid, int key, int lines, char *keyword,
                      char *title, char *authors, char *year, char *path) {
    Packet *packet = (Packet *)malloc(sizeof(Packet));
    packet->code = code;
    packet->src_pid = src_pid;
    packet->key = key;
    packet->lines = lines;
    if (keyword != NULL) {
        strcpy(packet->keyword, keyword);
    }
    if (title != NULL) {
        strcpy(packet->title, title);
        strcpy(packet->authors, authors);
        strcpy(packet->year, year);
        strcpy(packet->path, path);
    }
    return packet;
}

int delete_packet(Packet *packet) {
    if (packet != NULL) {
        free(packet);
        return 0;
    } else {
        return -1;
    }
}

int send_packet(Packet *packet, int pipe_fd) {
    // debug_packet("Sending packet", packet);
    write(pipe_fd, packet, sizeof(Packet));
    return 0;
}

Packet *receive_packet(int pipe_fd) {
    Packet *packet = (Packet *)malloc(sizeof(Packet));
    ssize_t s = read(pipe_fd, packet, sizeof(Packet));
    if (s > 0) {
        // debug_packet("Received packet", packet);
    } else if (s == 0) {
        printf("Read 0 bytes\n");
        free(packet);
        packet = NULL;
    } else if (s < 0) {
        perror("Read less than 0 bytes\n");
        free(packet);
        packet = NULL;
    }

    return packet;
}

// debug
void debug_packet(char *header, Packet *packet) {
    printf("/-------------\\\n");
    printf("%s\n", header);
    printf("Code: %i\n", packet->code);
    printf("Pid: %i\n", packet->src_pid);
    printf("Key: %i\n", packet->key);
    printf("Lines: %i\n", packet->lines);
    printf("Keyword: %s\n", packet->keyword);
    printf("Title: %s\n", packet->title);
    printf("Authors: %s\n", packet->authors);
    printf("Year: %s\n", packet->year);
    printf("Path: %s\n", packet->path);
    printf("\\-------------/\n");
}

/* Packet *create_packet(Code code, pid_t src_pid, ...) {

    va_list args;
    va_start(args, src_pid);

    Packet *packet = (Packet *)malloc(sizeof(Packet));
    packet->code = code;
    packet->src_pid = src_pid;

    switch (code) {
    case ADD_DOCUMENT:
        strcpy(packet->title, va_arg(args, char *));
        strcpy(packet->authors, va_arg(args, char *));
        strcpy(packet->year, va_arg(args, char *));
        strcpy(packet->path, va_arg(args, char *));
        break;

    case QUERY_DOCUMENT:
        packet->key = va_arg(args, int);
        break;

    case DELETE_DOCUMENT:
        packet->key = va_arg(args, int);
        break;

    case COUNT_LINES:
        packet->key = va_arg(args, int);
        strcpy(packet->keyword, va_arg(args, char *));
        break;

    case SEARCH_DOCUMENTS:
        strcpy(packet->keyword, va_arg(args, char *));
        break;

    case SHUTDOWN_SERVER:
    case KILL_CHILD:
    case FAILURE:
    case FRAG:
        // No additional arguments needed
        break;
    
    default:
        perror("Unkown code\n");
        free(packet);
        va_end(args);
        return NULL;
    }

    va_end(args);
    return packet;
} */
