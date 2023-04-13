#ifndef TASK_SHELL_HISTORY_H
#define TASK_SHELL_HISTORY_H

#include <stdio.h>


#define MAX_SIZE 20

typedef struct History {
    char** cmd_history;
    int history_capacity;
    int history_size;
    int history_pos;
    int history_last_index;
} shell_history;

int initHistory(shell_history* h);

int addHistoryEntry(shell_history* h, char* cmd);

int printHistory(shell_history* h);

int history_destroy(shell_history* h);

#endif //TASK_SHELL_HISTORY_H
