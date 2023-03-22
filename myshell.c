#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_LINE_LEN 1024
#define WHITESPACE " \t\n\r"


int parse_command(char **argv, char *command) {
    /* parse command line */
    int i = 0;
    char *token = strtok(command, WHITESPACE);
    while (token != NULL) {
        argv[i++] = token;
        token = strtok(NULL, WHITESPACE);
    }
    argv[i] = NULL;
    return i;
}

void args_handler(char **argv, int argc, char **outfile, int *redirect, int *amper, char *prompt) {
    if (!strcmp(argv[argc - 1], "&")) {
        *amper = 1;
        argv[argc - 1] = NULL;
    }
    else if (argc > 1 && !strcmp(argv[argc - 2], ">")) {
        *redirect = 1;
        argv[argc - 2] = NULL;
        *outfile = argv[argc - 1];
    }
    // Change prompt
    else if (argc == 3 && strcmp(argv[0], "prompt") == 0 && strcmp(argv[1], "=") == 0) {
        strcpy(prompt, argv[2]);
    }
}

int main() {
    // init variables for shell
    char command[MAX_LINE_LEN + 1];
    char last_command[MAX_LINE_LEN + 1] = ""; // initialize last_command as empty string
    int last_command_flag = 0;
    char *token;
    char *outfile = NULL;
    int i, fd, amper, redirect, retid, status;
    char *argv[10];
    char prompt[MAX_LINE_LEN + 1] = "hello";

    memset(command, 0, MAX_LINE_LEN + 1);

    while (1) {

        memset(command, 0, MAX_LINE_LEN + 1);

        printf("%s: ", prompt);

        if (!fgets(command, MAX_LINE_LEN, stdin)) {
            break;
        }

        command[strlen(command) - 1] = '\0'; // replace \n with \0
        redirect = amper = 0;

        // get command line arguments
        int argc = parse_command(argv, command);

        /* Is command empty */
        if (argv[0] == NULL) {
            continue;
        }

        if (strcmp(argv[0], "quit") == 0) {
            break;
        }

        args_handler(argv, argc, &outfile, &redirect, &amper, prompt);

        /* for commands not part of the shell command language */
        if (fork() == 0) {
            /* redirection of IO ? */
            if (redirect) {
                fd = creat(outfile, 0660);
                close(STDOUT_FILENO);
                dup(fd);
                close(fd);
                /* stdout is now redirected */
            }
            execvp(argv[0], argv);
        }
        /* parent continues here */
        if (amper == 0) {
            retid = wait(&status);
        }
    }

    return 0;
}