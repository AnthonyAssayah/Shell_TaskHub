#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <setjmp.h>
#include "myshell.h"

char prompt[MAX_LINE_LEN + 1] = "hello:";
char *argv[MAX_LINE_LEN + 1];
char *pipe_buffer[MAX_LINE_LEN + 1];
sigjmp_buf jmpbuf;
shell_history history;
int status = 0;

// signal handler for Ctrl-C
void sigint_handler(int sig) {
    write(STDOUT_FILENO, "\nYou typed Control-C!\n", 22);
    siglongjmp(jmpbuf, 1);
}

/**
 * Split user command arguments.
 * @return Number of command arguments.
 */
int parseCommand(char *command) {
    int i = 0;
    char *token = strtok(command, WHITESPACE);
    while (token != NULL) {
        argv[i++] = token;
        token = strtok(NULL, WHITESPACE);
    }
    argv[i] = NULL;
    return i;
}

int parsePipes(char *command) {
    int i = 0;
    char *token = strtok(command, "|");
    while (token != NULL) {
        pipe_buffer[i++] = token;
        token = strtok(NULL, "|");
    }
    pipe_buffer[i] = NULL;
    return i;
}

/* handle the arrows keys */
int handleArrows(shell_history *hist, char *command) {
    printHistory(hist); // FOR DEBUGGING //
    char tmp_command[MAX_LINE_LEN + 1];

    // initialize history counter and position to the end of history
    int history_counter = hist->history_size;
    int history_pos = hist->history_pos;

    strcpy(tmp_command, command);

    while (1) {
        if ((strcmp(command, "\033[A")) == 0) { // check if up arrow key was pressed
            if (history_pos >= 0) { // if not at the beginning of history
                // set temp command buffer to the previous command from history
                strcpy(tmp_command, hist->cmd_history[history_pos]);
                --history_pos;
                printf("\033[1A"); // line up
                printf("\x1b[2K"); // delete line
                printf("%s %s", prompt, tmp_command);
                fflush(stdout);
            } else { // if already at beginning of history
                printf("\033[1A"); // line up
                printf("\x1b[2K"); // delete line
                printf("%s %s", prompt, tmp_command);
                fflush(stdout);
            }
        } else if ((strcmp(command, "\033[B")) == 0) {// check if down arrow key was pressed
            if (history_pos < history_counter - 1) { // if not at the end of history
                ++history_pos;
                strcpy(tmp_command,
                       hist->cmd_history[history_pos]); // set temp command buffer to the next command from history
                printf("\033[1A"); // line up
                printf("\x1b[2K"); // delete line
                printf("%s %s", prompt, tmp_command);
                fflush(stdout);
            } else {
                printf("\033[1A"); // line up
                printf("\x1b[2K"); // delete line
                fflush(stdout);
                return 1;
            }
        }
        fgets(command, MAX_LINE_LEN, stdin);
        command[strlen(command) - 1] = '\0';
        if (strcmp(command, "") == 0) { // if user input is empty
            strcpy(command, tmp_command);
            break;
        }
    }
    return 0;
}

int ifThen(int argc) {
    char *commands[MAX_COMMANDS];
    char condition[MAX_LINE_LEN + 1];
    int commandCount = 0, j = 1;

    // remove if from argv and shift the rest of the arguments
    while (argv[j] != NULL) {
        argv[j - 1] = argv[j];
        ++j;
    }
    argv[j - 1] = NULL;
    --argc;

    status = execute(argc);
    int currstatus = WEXITSTATUS(status);//(args);
    printf("CURRENT STATUS IS: %d\n", currstatus);

    int elseFlag = !currstatus ? 1 : 0;
    /* if !currstatus is True: need to execute 'then' statement. flag will be true until else is reached.
     * otherwise: need to execute 'else' statement. flag will be false until else is reached.
     */
    if (fgets(condition, MAX_LINE_LEN, stdin) != NULL) {
        condition[strlen(condition) - 1] = '\0';
        if (!strcmp(condition, "then")) {
            while (fgets(condition, MAX_LINE_LEN, stdin) != NULL) {
                condition[strlen(condition) - 1] = '\0';
                if(!strcmp(condition, "fi")) {
                    break;
                }
                if (!strcmp(condition, "else")) {
                    printf("CHANGE FLAG: %d\n", elseFlag);
                    elseFlag = !elseFlag;
                    printf("FLAG IS NOW: %d\n", elseFlag);
                } else if (elseFlag) {
                    printf("ADD COMMAND\n");
                    commands[commandCount++] = strdup(condition);
                }
            }
        } else {
            printf("Bad if statement\n");
            return 0;
        }
    }

    argc = 0;
    for (int i = 0; i < commandCount; ++i) {
        argc = parseCommand(commands[i]);
        execute(argc);
        free(commands[i]);
    }
    return 1;
}

int addVar(int argc) {
    char new_var[MAX_LINE_LEN + 1] = "";
    for (int j = 2; j < argc; j++) {
        strcat(new_var, argv[j]);
        strcat(new_var, " ");
    }
    setenv(argv[0] + 1, new_var, 1);
    return 0;
}

int readShell() {
    char input[MAX_LINE_LEN + 1];
    fgets(input, MAX_LINE_LEN, stdin);
    input[strlen(input) - 1] = '\0';
    setenv(argv[1], input, 1);
    return 0;
}

int changeDir(int argc) {
    if (argc < 2) {
        printf("cd: missing operand\n");
    } else if (chdir(argv[1]) < 0) {
        printf("cd: %s: No such file or directory\n", argv[1]);
    } else {
        // print current directory
        char cwd[MAX_LINE_LEN];
        printf("%s \n", getcwd(cwd, sizeof(cwd)));
    }
    return 0;
}

int echoShell() {
    int j = 1;
    while (argv[j] != NULL) {
        if (argv[j][0] == '$') { // check if argument is a variable
            if (strcmp(argv[j], "$?") == 0) {
                printf("%d ", WEXITSTATUS(status)); // prints the status of the last command executed
            } else {
                char *var_value = getenv(argv[j] + 1); // get the value of the variable
                if (var_value != NULL) {
                    printf("%s ", var_value); // print the value of the variable
                }
            }
        } else {
            printf("%s ", argv[j]);
        }
        ++j;
    }
    printf("\n");
    return 0;
}

int handleOutputRedirect(int argc, char **outfile, int *append, int *input_redirect) {
    int flag = 0, redirect_fd = -1;
    if (argc > 1) {
        if (!strcmp(argv[argc - 2], ">")) {
            flag = 1;
            redirect_fd = STDOUT_FILENO;
        } else if (!strcmp(argv[argc - 2], ">>")) {
            flag = 1;
            *append = 1; // mark file to open with append flag
            redirect_fd = STDOUT_FILENO;
        } else if (!strcmp(argv[argc - 2], "2>")) { // redirect stderr
            flag = 1;
            redirect_fd = STDERR_FILENO;
        } else if (!strcmp(argv[argc - 2], "<")) {
            flag = 1;
            *input_redirect = 1;
            redirect_fd = STDIN_FILENO;
        }
    }
    if (flag) {
        *outfile = argv[argc - 1];
        argv[argc - 2] = NULL;
    }
    return redirect_fd;
}

int execute(int argc) {
    int amper = 0, rv = -1, append = 0, input_redirect = 0, fd, redirect_fd;
    char *outfile;

    /* Handle regular shell commands */

    if (argv[0] == NULL) {
        return 0;
    }

    if (!strcmp(argv[0], "if")) {
        return ifThen(argc);
    }

    if (argv[0][0] == '$' && strcmp(argv[1], "=") == 0 && argc >= 3) { // Add new variables
        return addVar(argc);
    }

    if (strcmp(argv[0], "read") == 0) {
        return readShell();
    }

    if (strcmp(argv[0], "cd") == 0) {
        return changeDir(argc);
    }

    if (argc == 3 && strcmp(argv[0], "prompt") == 0 && strcmp(argv[1], "=") == 0) { // Change prompt message
        strcpy(prompt, argv[2]);
        return 0;
    }

    if (strcmp(argv[0], "echo") == 0) {
        return echoShell();
    }

    if (!strcmp(argv[argc - 1], "&")) {
        amper = 1;
        argv[argc - 1] = NULL;
    }

    redirect_fd = handleOutputRedirect(argc, &outfile, &append, &input_redirect);

    /* for commands not part of the shell command language */

    if (fork() == 0) {
        /* redirection of IO */
        if (redirect_fd > -1) {
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
        execvp(argv[0], argv);
    }
    /* parent continues here */
    if (amper == 0) {
        wait(&status);
        rv = status;
    }

    return rv;
}

void handlePipeExecution(char *command) {
    int num_cmds = parsePipes(command);
    int fds[num_cmds][2];
    int argc = 0;
    for (int i = 0; i < num_cmds; ++i) {
        argc = parseCommand(pipe_buffer[i]);
        if (i != num_cmds - 1) { // if not last command
            if (pipe(fds[i]) < 0) {
                perror("Error in pipe!");
                exit(2);
            }
        }
        if (fork() == 0) {
            // Child process
            if (i != num_cmds - 1) { // if not last command - standard output goes to pipe
                dup2(fds[i][WRITE_END], STDOUT_FILENO);
                close(fds[i][READ_END]);
                close(fds[i][WRITE_END]);
            }
            if (i != 0) { // if not first command - standard input comes from pipe
                dup2(fds[i - 1][READ_END], STDIN_FILENO);
                close(fds[i - 1][READ_END]);
                close(fds[i - 1][WRITE_END]);
            }
            status = execute(argc);
            exit(0);
        }
        // Parent process
        if (i != 0) {
            close(fds[i - 1][READ_END]);
            close(fds[i - 1][WRITE_END]);
        }
        wait(NULL);
    }
}

int main() {
    char command[MAX_LINE_LEN + 1];
    char last_command[MAX_LINE_LEN + 1];
    int argc = 0;

    initHistory(&history);

    // Ctrl-C sigaction
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa, NULL);
    sigsetjmp(jmpbuf, 1);


    while (1) {
        // Reset global variables
        memset(command, 0, MAX_LINE_LEN + 1);

        // Get command from user
        printf("%s ", prompt);
        if (!fgets(command, MAX_LINE_LEN, stdin)) {
            break;
        }
        command[strlen(command) - 1] = '\0';

        if (((strcmp(command, "\033[A")) == 0) || ((strcmp(command, "\033[B")) == 0)) {
            if (handleArrows(&history, command)) {
                continue;
            }
        }

        if (strcmp(command, "quit") == 0) {
            printf("QUIT\n");
            break;
        }

        if (strcmp(command, "!!") == 0) {
            strcpy(command, last_command); // copies last_command into command
        } else {
            strcpy(last_command, command); // save command for next iteration
        }

        // Save command in history
        addHistoryEntry(&history, command);

        if(strcmp(command, "if") == 0) {
            printf("IF STATEMENT FOUND\n");
        }

        // strchr() returns a pointer to the first occurrence of the specified char or NULL if not found.
        if (strchr(command, '|') != NULL) {
            handlePipeExecution(command);
        } else {
            // Split command arguments (into argv variable)
            argc = parseCommand(command);
            // Handle command execution
            status = execute(argc);
        }
    }
    history_destroy(&history);
    return 0;
}
