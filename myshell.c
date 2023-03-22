#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_LINE_LEN 1024
#define WHITESPACE " \t\n\r"

char **parse_outfile_name(char *token, char **outfile, char **argv) {
    if (strlen(token) != 0) { // check for empty string
        *outfile = token;
        return argv;
    }
    // there was a space after > - next token should be the outfile name
    if ((token = strtok(NULL, WHITESPACE)) == NULL) { // check for error
        free(argv);
        return NULL;
    }
    *outfile = token;
    return argv;
}

char **tokenize_command(char *command, char **outfile, int *redirect, int *amper) {
    char *token = NULL;
    size_t capacity = 10;
    char **argv = calloc(capacity, sizeof(char *));

    size_t argv_len = 1; // start from 1 (first arg is the command name)
    while ((token = strtok(NULL, WHITESPACE)) != NULL) {
        if (token[0] == '>') {
            *redirect = 1;
            return parse_outfile_name(&token[1], outfile, argv);
        } else if (token[0] == '&') {
            *amper = 1;
            return argv;
        }
        if ((argv_len + 1) == capacity) { // check if argv array is full
            capacity *= 2;
            argv = realloc(argv, capacity * sizeof(char *));
        }
        argv[argv_len++] = token; // add token to expanded argv array
    }
    return argv;
}

//void exec_child(char *command, char **argv, char *outfile) {
//    int out_fd = -1;
//    if (outfile != NULL) { // check for redirection
//        out_fd = open(outfile, O_RDWR | O_CREAT);
//        if (out_fd < 0) {
//
//        }
//    }
//}

int main() {
    // init variables for shell
    char command[MAX_LINE_LEN + 1];
    char last_command[MAX_LINE_LEN + 1] = ""; // initialize last_command as empty string
    int last_command_flag = 0;
    char *token;
    char *outfile = NULL;
    int i, fd, amper, redirect, retid, status;
    char **argv;
    char prompt[MAX_LINE_LEN + 1] = "hello:";

    memset(command, 0, MAX_LINE_LEN + 1);

    while (1) {

        printf("%s ", prompt);

        memset(command, 0, MAX_LINE_LEN + 1);
        if (!fgets(command, MAX_LINE_LEN, stdin)) {
            break;
        }

        token = strtok(command, WHITESPACE); // get first command
        if (strcmp(token, "quit") == 0) {
            break;
        }

        redirect = 0;
        amper = 0;
        // get command line arguments
        argv = tokenize_command(command, &outfile, &redirect, &amper);
        argv[0] = token;

        /* Is command empty */
        if (argv[0] == NULL) {
            continue;
        }

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