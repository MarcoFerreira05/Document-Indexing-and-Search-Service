#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

int create_pipe(char *pipe_name) {
    int fifo = mkfifo(pipe, 0666);
    if (fifo < 0) {
        perror("Failed to create pipe\n");
        return -1;
    }
    return 0;
}

int close_pipe(char *pipe_name) {
    int result = unlink(pipe);
    if (result < 0) {
        perror("Failed to close pipe\n");
        return -1;
    }
    return 0;
}

Packet *create_packet(Code code, char *response_pipe, int document_id, char **metadata) {
    Packet *packet = (Packet *)malloc(sizeof(Packet));
    packet->code = code;
    packet->response_pipe = response_pipe;
    packet->document_id = document_id;
    packet->metadata = metadata;
    return 0;
}

int delete_packet(Packet *packet) {
    free(packet);
}

int send_packet(Packet *packet, char *pipe_name) {
    int fd = open(pipe_name, O_WRONLY);
    if (fd < 0) {
        perror("Failed to open pipe\n");
        return -1;
    }
    write(fd, packet, sizeof(Packet));
    close(fd);
    delete_packet(packet);
    return 0;
}

Packet *receive_packet(char *pipe_name) {
    int fd = open(pipe_name, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open request pipe\n");
        return NULL;
    }
    Packet *packet = (Packet *)malloc(sizeof(Packet));
    read(fd, packet, sizeof(Packet));
    close(fd);
    return packet;
}

 // debug
void debug_packet(char *title, Packet *packet) {
    printf("%s\n", title);
    printf("Code: %d\n", packet->code);
    printf("Response pipe: %d\n", packet->response_pipe);
    printf("Document ID: %d\n", packet->document_id);
    printf("Metadata: %s\n", packet->metadata);
}