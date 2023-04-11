#include <stddef.h>
#include <malloc.h>
#include <string.h>
#include "history.h"

// source: https://stackoverflow.com/questions/22289428/making-a-simple-history-function-in-c-shell-program

void initHistory(struct History *h, int capacity) {
    h->capacity = capacity;
    h->cmd_history = malloc(capacity * sizeof(char *));
    for (int i = 0; i < capacity; ++i) {
        h->cmd_history[i] = NULL;
    }
    h->start_idx = 0;
    h->cmd_count = 0;
}

void addHistoryEntry(struct History *h, char *command) {
    if (h->cmd_history[h->last_index] != NULL) {
        free(h->cmd_history[h->last_index]);
    }
    h->cmd_history[h->last_index] = strdup(command);
    h->last_index = (h->last_index + 1) % h->capacity;
    if (h->cmd_count < h->capacity) {
        h->cmd_count++;
    } else {
        h->first_index = (h->first_index + 1) % h->capacity;
    }
}

void printHistory(struct History *h) {
    char *curr_line;
    int curr_idx = h->last_index;
    for (int i = 0; i < h->cmd_count; ++i) {
        curr_line = h->cmd_history[curr_idx];
        if (curr_line != NULL) {
            printf("%d: %s\n", h->cmd_count - i, curr_line);
        }
        curr_idx = (curr_idx - 1 + h->capacity) % h->capacity;
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
