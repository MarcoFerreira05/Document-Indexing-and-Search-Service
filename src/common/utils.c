#include <stdlib.h>

void strcopy(char *destination, const char *source, size_t num) {
    size_t i;
    for (i = 0; i < num && source[i] != '\0'; i++) {
        destination[i] = source[i];
    }
    // Null-terminate the destination string if the source was shorter than num
    if (i < num) {
        destination[i] = '\0';
    } else {
        // Ensure null-termination if source was longer than num
        destination[num - 1] = '\0';
    }
}