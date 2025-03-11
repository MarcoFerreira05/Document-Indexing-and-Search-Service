#include <include/common/protocol.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

int create_request_pipe() {
    int fifo = mkfifo(REQUEST_PIPE, 0666);
    if (fifo < 0) {
        perror("Failed to create request pipe\n");
        return -1;
    }
    return 0;
}

int create_response_pipe(pid_t client_pid) {
    char pipe_name[MAX_PIPE_SIZE];
    snprintf(pipe_name, sizeof(pipe_name), RESPONSE_PIPE, client_pid);
    int fifo = mkfifo(pipe_name, 0666);
    if (fifo < 0) {
        perror("Failed to create response pipe\n");
        return -1;
    }
    return 0;
}

int send_request(Request *request) {
    int fd = open(REQUEST_PIPE, O_WRONLY);
    if (fd < 0) {
        perror("Failed to open request pipe\n");
        return -1;
    }
    write(fd, request, sizeof(Request));
    close(fd);
    return 0;
}

int receive_request(Request *request) {
    int fd = open(REQUEST_PIPE, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open request pipe\n");
        return -1;
    }
    read(fd, request, sizeof(Request));
    close(fd);
    return 0;
}

int send_response(Response *response, pid_t client_pid) {
    char pipe_name[MAX_PIPE_SIZE];
    snprintf(pipe_name, sizeof(pipe_name), RESPONSE_PIPE, client_pid);
    int fd = open(pipe_name, O_WRONLY);
    if (fd < 0) {
        perror("Failed to open response pipe\n");
        return -1;
    }
    write(fd, response, sizeof(Response));
    close(fd);
    return 0;
}

int receive_response(Response *response, pid_t client_pid) {
    char pipe_name[MAX_PIPE_SIZE];
    snprintf(pipe_name, sizeof(pipe_name), RESPONSE_PIPE, client_pid);
    int fd = open(pipe_name, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open response pipe\n");
        return -1;
    }
    read(fd, response, sizeof(Response));
    close(fd);
    unlink(pipe_name);
    return 0;
}
