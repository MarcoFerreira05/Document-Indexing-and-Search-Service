#ifndef INDEX_H
#define INDEX_H

#define INDEX_FILE "index"

typedef struct {
    int fd;
    char *documents_folder;
} Index;

Index *index_init();

#endif