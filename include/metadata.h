#ifndef METADATA_H
#define METADATA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TITLE_SIZE 200
#define AUTHORS_SIZE 200
#define YEAR_SIZE 5
#define PATH_SIZE 64

// Document structure definition
typedef struct {
    int id;                      // Unique document identifier
    char title[TITLE_SIZE];      // Document title
    char authors[AUTHORS_SIZE];  // Document authors
    char year[YEAR_SIZE];        // Document year
    char path[PATH_SIZE];        // Document path
} Document;

// Function declarations for metadata operations
int add_document(char *title, char *authors, char *year, char *path);
Document *get_document(int doc_id);
int delete_document(int doc_id);

#endif
