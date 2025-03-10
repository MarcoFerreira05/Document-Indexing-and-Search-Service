#ifndef SEARCH_H
#define SEARCH_H

#include "metadata.h"

// Count lines containing keyword
int count_lines_with_keyword(int doc_id, char *keyword);

// Search documents containing keyword
int *search_documents_with_keyword(char *keyword, int *num_results);

// Search documents in parallel
int *search_documents_parallel(char *keyword, int max_processes, int *num_results);

#endif
