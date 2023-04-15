#ifndef TASK_SHELL_H
#define TASK_SHELL_H

#include "history.h"

#define MAX_LINE_LEN 1024
#define WHITESPACE " \t\n\r"
#define MAX_COMMANDS 100
#define READ_END 0
#define WRITE_END 1

void sigint_handler(int sig);

int parseCommand(char *command);

int ifThen();

int addVar(int argc);

int readShell();

int changeDir(int argc);

int echoShell();

int handleOutputRedirect(int argc, char **outfile, int *append, int *input_redirect);

int handlePipeExecution(char *command);

int execute(int argc);

int fullExecution(char *command, int is_if_command);

int handleArrows(shell_history *hist, char *command);

#endif //TASK_SHELL_H
