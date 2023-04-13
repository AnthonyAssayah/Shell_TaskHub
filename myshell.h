#ifndef TASK_SHELL_H
#define TASK_SHELL_H

#include "history.h"

#define MAX_LINE_LEN 1024
#define WHITESPACE " \t\n\r"
#define MAX_COMMANDS 100

void sigint_handler(int sig);

int parseCommand(char *command);

int ifThen();

int addVar(int argc);

int readShell();

int changeDir(int argc);

int echoShell();

int handleOutputRedirect(int argc, char **outfile, int *append, int *input_redirect);

int execute(int argc);

int handleArrows(shell_history *hist, char *command);

#endif //TASK_SHELL_H
