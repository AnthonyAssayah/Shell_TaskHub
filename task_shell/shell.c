#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>





int main() {


    char command[1024];
    char last_command[1024] = ""; //initialize last_command as empty string
    int last_command_flag = 0; 
    char *token;
    char *outfile;
    int i, fd, amper, redirect, retid, status;
    char *argv[10];
    char prompt[1024] = "hello: ";
    
    // signal handler for Ctrl-C
   void Ctrl_CHandler(int sig) {
    if (sig == SIGINT) {
        printf("\nYou typed Control-C!\n%s", prompt);
        fflush(stdout);
     }  
    }

    signal(SIGINT, Ctrl_CHandler); 

    while (1)
    {
        if (last_command_flag ) {
            printf("I'm doing last command \n");
            last_command_flag = 0;
        }
        else {
            printf("%s ", prompt);
            fgets(command, 1024, stdin);
            command[strlen(command) - 1] = '\0';

            if (strcmp(command, "!!") != 0) {
                strcpy(last_command, command);
            }
            
        }
        
        // execute last command
        if (strcmp(command, "!!") == 0) {
            last_command_flag = 1;
            strcpy(command, last_command);
      
        }

        /* parse command line */
        i = 0;
        token = strtok (command," ");
        while (token != NULL)
        {
            argv[i] = token;
            token = strtok (NULL, " ");
            i++;
        }
        argv[i] = NULL;

        // Exit shell
        if (strcmp(argv[0], "quit") == 0) {
            return 0;
        }

        // Echo command    
        else if (strcmp(argv[0], "echo") == 0) {

            for (int j = 1; j < i; j++) {
                printf("%s ", argv[j]);
                if (strcmp(argv[j], "$?") == 0) {
                    printf("%d ", WEXITSTATUS(status)); // prints the status of the last command executed
                }
            
            }
            printf("\n");
            continue;
        }

        
        // Change prompt
        else if(strcmp(argv[0], "prompt") == 0 && strcmp(argv[1], "=") == 0){
            char new_prompt[1024] = "";
            for (int j = 2; j < i; ++j) {
                strcat(new_prompt, argv[j]);
                strcat(new_prompt, " ");
            }
            strcpy(prompt,new_prompt);
            continue;
        }


//  else if (strcmp(argv[0], "echo") == 0 && argv[1][0] == '$') {
        //     char* value = getenv(argv[1]+1);
        //     printf("%s\n", value ? value : "");
        //     continue;
        // }

       // add variables in this format: $var = value
     else if (argv[0][0] == '$' && strcmp(argv[1], "=") == 0) {
        char var[1024] = "";
        char value[1024] = "";
        strcat(var, &argv[0][1]); // remove $ from variable name
        strcat(value, argv[2]);
        setenv(var, value, 1);
        char* value2 = getenv(var);

        // printf("%s\n", value2 ? value2 : "");
        continue;
        }

        
        // Change directory
        else if (strcmp(argv[0], "cd") == 0) {
            if (i < 2) {
                printf("i = %d\n", i);
                printf("cd: missing operand\n");
                continue;
            } else if (chdir(argv[1]) < 0) {
                printf("cd: %s: No such file or directory\n", argv[1]);
                continue;

            }
            //print current directory
            char cwd[1024];
            printf ("%s \n", getcwd(cwd, sizeof(cwd)));
            continue;

        }
        // Print current directory
        else if(strcmp(argv[0], "pwd") == 0){
            char cwd[1024];
            printf ("%s \n", getcwd(cwd, sizeof(cwd)));
            continue;
        }

        // else if (strcmp(argv[0], "ls") == 0) {
        //     if (i > 1) {
        //         printf("ls: too many arguments\n");
        //         continue;
        //     } else {
        //         system("ls");
        //         continue;
        //     }
        // }
    
        
        else {
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            } 
            else if (pid == 0) {
                if (execvp(argv[0], argv) == -1) {
                    perror("execvp");
                    exit(EXIT_FAILURE);
                }
            }
            else {
                waitpid(pid, &status, 0);
            }
            int fd = open("stderr.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
            dup2(fd, 2);
            close(fd);
            continue;

        }

       
        

        // /* Is command empty */
        // if (argv[0] == NULL)
        //     continue;

        // /* Does command line end with & */
        // if (! strcmp(argv[i - 1], "&")) {
        //     amper = 1;
        //     argv[i - 1] = NULL;
        // }
        // else
        //     amper = 0;

        // if (! strcmp(argv[i - 2], ">")) {
        //     redirect = 1;
        //     argv[i - 2] = NULL;
        //     outfile = argv[i - 1];
        // }
        // else
        //     redirect = 0;

        /* for commands not part of the shell command language */

        // if (fork() == 0) {
        //     /* redirection of IO ? */
        //     if (redirect) {
        //         fd = creat(outfile, 0660);
        //         close (STDOUT_FILENO) ;
        //         dup(fd);
        //         close(fd);
        //         /* stdout is now redirected */
        //     }
        //     execvp(argv[0], argv);
        // }
        /* parent continues here */
        // if (amper == 0)
        //     retid = wait(&status);

        
    }
}
