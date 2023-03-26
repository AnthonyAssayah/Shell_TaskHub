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
char *outfile = NULL, *token;
int i, fd, amper, redirect, retid, status, piping, input_redirect;
char *argv1[10], *argv2[10];
int argc1, redirect_fd, append;
char prompt[MAX_LINE_LEN + 1] = "hello:";
sigjmp_buf jmpbuf;
int fildes[2];


void handleOutputRedirect();

int handleShellCommands();

void resetGlobalVars();

int changeDir();

int addVar();

int echoShell();

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
    memset(last_command, 0, MAX_LINE_LEN + 1);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa, NULL);
    sigsetjmp(jmpbuf, 1);

    while (1) {

        printf("%s ", prompt);

        if (!fgets(command, MAX_LINE_LEN, stdin)) {
            break;
        }
        command[strlen(command) - 1] = '\0';

        // 6. Execute previous command
        if (strcmp(command, "!!") == 0) {
            strcpy(command, last_command); // strcpy(src, dest)
        } else {
            strcpy(last_command, command); // save command for next iteration
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
                } else if (input_redirect) {
                    fd = open(outfile, O_RDONLY, 0660);
                } else {
                    fd = creat(outfile, 0660);
                }
                close(redirect_fd);
                dup(fd);
                close(fd);
            }
            if (piping) {
                pipe(fildes);
                if (fork() == 0) {
                    /* first component of command line */
                    close(STDOUT_FILENO);
                    dup(fildes[1]);
                    close(fildes[1]);
                    close(fildes[0]);
                    /* stdout now goes to pipe */
                    /* child process does command */
                    execvp(argv1[0], argv1);
                }
                /* 2nd command component of command line */
                close(STDIN_FILENO);
                dup(fildes[0]);
                close(fildes[0]);
                close(fildes[1]);
                /* standard input now comes from pipe */
                execvp(argv2[0], argv2);
            } else {
                execvp(argv1[0], argv1);
            }
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
        return echoShell();
    }

    // 5. Change dir
    if (strcmp(argv1[0], "cd") == 0) {
        return changeDir();
    }

    // 10. Add new variables
    if (argv1[0][0] == '$' && strcmp(argv1[1], "=") == 0 && argc1 >= 3) {
        return addVar();
    }

    return 0;
}

int echoShell() {
    for (int j = 1; j < argc1; ++j) {
        if (argv1[j][0] == '$') { // check if argument is a variable
            if (strcmp(argv1[j], "$?") == 0) {
                printf("%d ", WEXITSTATUS(status)); // prints the status of the last command executed
            } else {
                char *var_value = getenv(argv1[j] + 1); // get the value of the variable
                if (var_value != NULL) {
                    printf("%s ", var_value); // print the value of the variable
                }
            }
        } else {
            printf("%s ", argv1[j]);
        }
    }
    printf("\n");
    return 1;
}

int addVar() {
    char new_var[MAX_LINE_LEN] = "";
    for (int j = 2; j < argc1; j++) {
        strcat(new_var, argv1[j]);
        strcat(new_var, " ");
    }
    setenv(argv1[0] + 1, new_var, 1);
    return 1;
}

int changeDir() {
    if (i < 2) {
        printf("i = %d\n", i);
        printf("cd: missing operand\n");
    } else if (chdir(argv1[1]) < 0) {
        printf("cd: %s: No such file or directory\n", argv1[1]);
    } else {
        // print current directory
        char cwd[MAX_LINE_LEN];
        printf("%s \n", getcwd(cwd, sizeof(cwd)));
    }
    return 1;
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
        } else if (!strcmp(argv1[argc1 - 2], "<")) {
            redirect = flag = input_redirect = 1;
            redirect_fd = STDIN_FILENO;
        }
    }
    if (flag) {
        argv1[argc1 - 2] = NULL;
        outfile = argv1[argc1 - 1];
    }
}
