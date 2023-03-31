#include <stddef.h>
#include <malloc.h>
#include <string.h>
#include "history.h"

void initHistory(struct History *h, int capacity) {
    h->capacity = capacity;
    h->cmd_history = malloc(capacity * sizeof(char *));
    for (int i = 0; i < capacity; ++i) {
        h->cmd_history[i] = NULL;
    }
}

void addHistoryEntry(struct History *h, char *command) {
    if (h->cmd_history[h->start_idx] != NULL) {
        free(h->cmd_history[h->start_idx]);
    }
    h->cmd_history[h->start_idx] = strdup(command);
    h->start_idx = (h->start_idx + 1) % h->capacity;
}

void printHistory(struct History *h) {
    char *curr_line;
    int curr_idx = h->start_idx;
    for (int i = 0; i < h->capacity; ++i) {
        curr_line = h->cmd_history[curr_idx];
        if (curr_line != NULL) {
            printf("%s\n", curr_line);
        }
        ++curr_idx;
        curr_idx = curr_idx >= h->capacity ? 0 : curr_idx;
    }
}

void freeHistory(struct History *h) {
    for (int i = 0; i < h->capacity; ++i) {
        if (h->cmd_history[i] != NULL) {
            free(h->cmd_history[i]);
        }
    }
    free(h->cmd_history);
}
