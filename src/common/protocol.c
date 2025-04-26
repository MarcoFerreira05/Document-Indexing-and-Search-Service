#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int create_pipe(char *pipe_name) {
    int fifo = mkfifo(pipe_name, 0666);
    if (fifo < 0) {
        perror("Failed to create pipe\n");
        return -1;
    }
    return 0;
}

int close_pipe(char *pipe_name) {
    int result = unlink(pipe_name);
    if (result < 0) {
        perror("Failed to close pipe\n");
        return -1;
    }
    return 0;
}

Packet *create_packet(Code code, char *response_pipe, int document_id, char **metadata) {
    Packet *packet = (Packet *)malloc(sizeof(Packet));
    packet->code = code;
    strcpy(packet->response_pipe, response_pipe);
    packet->document_id = document_id;
    if (metadata != NULL) {
        for (int i = 0; i < METADATA_FIELDS_COUNT; i++) {
            strcpy(packet->metadata[i], metadata[i]);
        }
    }
    return packet;
}

void delete_packet(Packet *packet) {
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
    free(packet);
    return 0;
}

Packet *receive_packet(char *pipe_name) {
    int fd = open(pipe_name, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open pipe\n");
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
    printf("Code: %i\n", packet->code);
    printf("Response pipe: %s\n", packet->response_pipe);
    printf("Document ID: %d\n", packet->document_id);
    for(int i = 0; i < METADATA_FIELDS_COUNT; i++) {
        printf("Metadata[%d]: %s\n", i, packet->metadata[i]);
    }
    printf("[ ------------------------ ]\n\n");
}