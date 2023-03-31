#ifndef TASK_SHELL_HISTORY_H
#define TASK_SHELL_HISTORY_H

struct History {
    char **cmd_history;
    int capacity;
    int start_idx;
};

void initHistory(struct History *h, int capacity);

void addHistoryEntry(struct History *h, char *command);

void printHistory(struct History *h);

void freeHistory(struct History *h);

#endif //TASK_SHELL_HISTORY_H
