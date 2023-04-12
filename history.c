#include "history.h"
#include <stdlib.h>
#include <string.h>

int initHistory(shell_history* history) {
    history->cmd_history = (char**) malloc(sizeof(char*) * MAX_SIZE);
    for (int i = 0; i < MAX_SIZE; i++) {
        history->cmd_history[i] = NULL;
    }
    history->history_size = 0;
    history->history_pos = history->history_size -1;
    return 0;
}

int addHistoryEntry(shell_history* history, char* command) {
    if (history->history_size >= MAX_SIZE) {
        free(history->cmd_history[0]);
        for (int i = 0; i < MAX_SIZE - 1; i++) {
            history->cmd_history[i] = history->cmd_history[i+1];
        }
        history->history_size--;
    }

    history->cmd_history[history->history_size] = (char*) malloc(sizeof(char) * (strlen(command) + 1));
    strcpy(history->cmd_history[history->history_size], command);
    history->history_size++;
    history->history_pos = history->history_size - 1;

    return 0;
}

int printHistory(shell_history* history) {
    // printf("History:\n");
    for(int i = 0; i < (history->history_size) ; i++) {
        printf("%d. %s\n", i, history->cmd_history[i]);
    }
    printf("\n");

    return 0;
}

int history_destroy(shell_history* history) {
    for (int i = 0; i < history->history_size; i++) {
        free(history->cmd_history[i]);
    }
    free(history->cmd_history);
    return 0;
}
