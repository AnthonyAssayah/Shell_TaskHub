#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>



// need to fix !! after some commands


int main() {
char command[1024];
char last_command[1024] = ""; // initialize last_command as empty string
int last_command_flag = 0;
char *token;
int i;
char *outfile;
int fd, amper, redirect, piping, retid, status, argc1;
int fildes[2];
// char *argv[10];
char *argv1[10], *argv2[10];
char prompt[1024] = "hello: ";


// signal handler for Ctrl-C
void Ctrl_CHandler(int sig)
{
    strcpy(command, last_command);
    if (sig == SIGINT)
    {
        strcpy(command, last_command);
        printf("\nYou typed Control-C!\n%s", prompt);
        fflush(stdout);
        
    }
    
}

signal(SIGINT, Ctrl_CHandler);


while (1)
{
    if (last_command_flag)
    {
        printf("I'm doing last command \n");
        last_command_flag = 0;
    }
    else
    {
        printf("%s ", prompt);
        fgets(command, 1024, stdin);
        command[strlen(command) - 1] = '\0';
        piping = 0;

        if (strcmp(command, "!!") != 0)
        {
            strcpy(last_command, command);
        }
    }
   

    /* parse command line */
    i = 0;
    token = strtok (command," ");
    while (token != NULL)
    {
        argv1[i] = token;
        token = strtok (NULL, " ");
        i++;
        if (token && ! strcmp(token, "|")) {
            piping = 1;
            break;
        }
    }
    argv1[i] = NULL;
    argc1 = i;

    // execute last command
    if (strcmp(command, "!!") == 0)
    {
        last_command_flag = 1;
        strcpy(command, last_command);
    }

    // Exit shell
    else if (strcmp(argv1[0], "quit") == 0)
    {
        return 0;
    }

    // Echo command
    else if (strcmp(argv1[0], "echo") == 0)
    {

        for (int j = 1; j < i; j++)
        {
            printf("%s ", argv1[j]);
            if (strcmp(argv1[j], "$?") == 0)
            {
                // strcpy(command, last_command);
                printf("%d ", WEXITSTATUS(status)); // prints the status of the last command executed
            }
            strcpy(command, last_command);
        }
        printf("\n");
        continue;
    }

     // Change prompt
    else if (strcmp(argv1[0], "prompt") == 0 && strcmp(argv1[1], "=") == 0)
    {
        char new_prompt[1024] = "";
        for (int j = 2; j < i; ++j)
        {
            strcat(new_prompt, argv1[j]);
            strcat(new_prompt, " ");
        }
        strcpy(prompt, new_prompt);
        continue;
    }

    // Change directory
    else if (strcmp(argv1[0], "cd") == 0)
    {
        if (i < 2)
        {
            printf("i = %d\n", i);
            printf("cd: missing operand\n");
            continue;
        }
        else if (chdir(argv1[1]) < 0)
        {
            printf("cd: %s: No such file or directory\n", argv1[1]);
            continue;
        }
        // print current directory
        char cwd[1024];
        printf("%s \n", getcwd(cwd, sizeof(cwd)));
        continue;
    }

    // Add new variables (still need to fix)
    // else if (strcmp(argv1[0], "$") == 0 && strcmp(argv1[2], "=") == 0)
    // {
    //     char new_var[1024] = "";
    //     for (int j = 3; j < i; ++j)
    //     {
    //         strcat(new_var, argv1[j]);
    //         strcat(new_var, " ");
    //     }
    //     setenv(argv1[1] + 1, new_var, 1); // skip the '$' character
    //     continue;
    // }

    /* Is command empty */
    if (argv1[0] == NULL)
        continue;

    /* Does command contain pipe */
    if (piping) {
        i = 0;
        while (token!= NULL)
        {
            token = strtok (NULL, " ");
            argv2[i] = token;
            i++;
        }
        argv2[i] = NULL;
    }

    /* Does command line end with & */ 
    if (! strcmp(argv1[argc1 - 1], "&")) {
        amper = 1;
        argv1[argc1 - 1] = NULL;
        }
    else 
        amper = 0; 

    if (argc1 > 1 && ! strcmp(argv1[argc1 - 2], ">")) {
        redirect = 1;
        argv1[argc1 - 2] = NULL;
        outfile = argv1[argc1 - 1];
        }
    else 
        redirect = 0; 

    /* for commands not part of the shell command language */ 

    if (fork() == 0) { 
        /* redirection of IO ? */
        if (redirect) {
            fd = creat(outfile, 0660); 
            close (STDOUT_FILENO) ; 
            dup(fd); 
            close(fd); 
            /* stdout is now redirected */
        } 
        if (piping) {
            pipe (fildes);
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
        } 
        else
            execvp(argv1[0], argv1);
    }
    /* parent continues over here... */
    /* waits for child to exit if required */
    if (amper == 0)
        retid = wait(&status);
}
}



// else
        // {
        //     pid_t pid = fork();
        //     if (pid == -1)
        //     {
        //         perror("fork");
        //         exit(EXIT_FAILURE);
        //     }
        //     else if (pid == 0)
        //     {
        //         if (execvp(argv[0], argv) == -1)
        //         {
        //             perror("execvp");
        //             exit(EXIT_FAILURE);
        //         }
        //     }
        //     else
        //     {
        //         waitpid(pid, &status, 0);
        //     }
        //     int fd = open("stderr.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
        //     dup2(fd, 2);
        //     close(fd);
        //     continue;
        // }
