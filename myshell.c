#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <setjmp.h>

#define MAX_LINE_LEN 1024
#define WHITESPACE " \t\n\r"

// init global variables for shell
char command[MAX_LINE_LEN + 1];
char last_command[MAX_LINE_LEN + 1] = ""; // initialize last_command as empty string
int last_command_flag = 0;
char *outfile = NULL, *token;
int i, fd, amper, redirect, retid, status, piping;
char *argv1[10], *argv2[10];
int argc1, redirect_fd, append;
char prompt[MAX_LINE_LEN + 1] = "hello";
sigjmp_buf jmpbuf;

void handleOutputRedirect();

int handleShellCommands();

void resetGlobalVars();

// signal handler for Ctrl-C
void sigint_handler(int sig) {
    write(STDOUT_FILENO, "\nYou typed Control-C!\n", 22);
    siglongjmp(jmpbuf, 1);
}

void parseCommand() {
    /* parse command line */
    i = 0;
    token = strtok(command, WHITESPACE);
    while (token != NULL) {
        argv1[i++] = token;
        token = strtok(NULL, WHITESPACE);
        if (token && !strcmp(token, "|")) {
            piping = 1;
            break;
        }
    }
    argv1[i] = NULL;
    argc1 = i;

    /* Is command empty */
    if (argv1[0] == NULL) { return; }

    /* Does command contain pipe */
    if (piping) {
        i = 0;
        while (token != NULL) {
            token = strtok(NULL, WHITESPACE);
            argv2[i++] = token;
        }
        argv2[i] = NULL;
    }
}

int main() {
    memset(command, 0, MAX_LINE_LEN + 1);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa, NULL);
    sigsetjmp(jmpbuf, 1);

    while (1) {

        printf("%s: ", prompt);

        if (!fgets(command, MAX_LINE_LEN, stdin)) {
            break;
        }

        // get command line arguments
        parseCommand();

        /* Is command empty */
        if (argv1[0] == NULL) {
            continue;
        }
        if (strcmp(argv1[0], "quit") == 0) {
            break;
        }

        if (!strcmp(argv1[argc1 - 1], "&")) {
            amper = 1;
            argv1[argc1 - 1] = NULL;
        }

        handleOutputRedirect();

        if (handleShellCommands()) {
            continue;
        }

        /* for commands not part of the shell command language */
        if (fork() == 0) {
            /* redirection of IO */
            if (redirect) {
                if (append) {
                    fd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0660);
                } else {
                    fd = creat(outfile, 0660);
                }
                close(redirect_fd);
                dup(fd);
                close(fd);
            }
            execvp(argv1[0], argv1);
        }
        /* parent continues here */
        if (amper == 0) {
            retid = wait(&status);
        }

        resetGlobalVars();
    }

    return 0;
}

void resetGlobalVars() {
    memset(command, 0, MAX_LINE_LEN + 1);
    amper = redirect = append = 0;
    redirect_fd = STDOUT_FILENO;
}

int handleShellCommands() {
    // 2. Change prompt
    if (argc1 == 3 && strcmp(argv1[0], "prompt") == 0 && strcmp(argv1[1], "=") == 0) {
        strcpy(prompt, argv1[2]);
        return 1;
    }

    // 3. Echo command
    if (strcmp(argv1[0], "echo") == 0) {
        for (int j = 1; j < argc1; ++j) {
            if (strcmp(argv1[j], "$?") == 0) {
                printf("%d ", WEXITSTATUS(status)); // prints the status of the last command executed
            } else {
                printf("%s ", argv1[j]);
            }
        }
        printf("\n");
        return 1;
    }

    return 0;
}

void handleOutputRedirect() {
    int flag = 0;
    if (argc1 > 1) {
        if (!strcmp(argv1[argc1 - 2], ">")) {
            redirect = flag = 1;
        } else if (!strcmp(argv1[argc1 - 2], ">>")) {
            redirect = append = flag = 1;
        } else if (!strcmp(argv1[argc1 - 2], "2>")) { // redirect stderr
            redirect = flag = 1;
            redirect_fd = STDERR_FILENO;
        }
    }
    if (flag) {
        argv1[argc1 - 2] = NULL;
        outfile = argv1[argc1 - 1];
    }
}
