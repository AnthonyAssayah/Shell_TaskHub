#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <setjmp.h>
#include "history.h"

#define MAX_LINE_LEN 1024
#define WHITESPACE " \t\n\r"
#define BREAK_LOOP -1
#define CONTINUE_NEXT_ITER 0


char command[MAX_LINE_LEN + 1];
char last_command[MAX_LINE_LEN + 1] = ""; // initialize last_command as empty string
char *outfile = NULL, *token;
int i, fd, amper, redirect, retid, status, piping, input_redirect;
char *argv1[10], *argv2[10];
int argc1, redirect_fd, append;
char prompt[MAX_LINE_LEN + 1] = "hello:";
sigjmp_buf jmpbuf, sigtstp_jmpbuf;
int fildes[2];
struct History history;

int getCommand();
void parseCommand(char *command);
int execute();
int forkProcess();
void resetGlobalVars();
int handleShellCommands();
int echoShell();
int addVar();
int readShell();
int changeDir();
void handleOutputRedirect();
int handle_arrows();
int ifThen();


// signal handler for Ctrl-C
void sigint_handler(int sig) {
    write(STDOUT_FILENO, "\nYou typed Control-C!\n", 22);
    siglongjmp(jmpbuf, 1);
}


int getCommand() {
    printf("%s ", prompt);
    if (!fgets(command, MAX_LINE_LEN, stdin)) {
        return -1;
    }
    command[strlen(command) - 1] = '\0';
    // 6. Execute previous command
    if (strcmp(command, "!!") == 0) {
        strcpy(command, last_command); // strcpy(src, dest)
    } else {
        strcpy(last_command, command); // save command for next iteration
    }
    return 1;
}

void parseCommand(char *command) {
    /* parse command line */
    i = 0;
    piping = 0;
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

int execute() {

    handleOutputRedirect();
 
    if (handleShellCommands() ) {
 
        return 1;
    }
    if (forkProcess()) {
   
        return 1;
    }

 
    return 1;
}

int forkProcess() {
   
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
                           // ls - l | wc - l | wc - l | > output.txt 
            pipe(fildes);
            if (fork() == 0) {
                /* first component of command line */
                close(STDOUT_FILENO);
                dup(fildes[1]);
                close(fildes[1]);
                    close(fildes[0]);
                    /* stdout now goes to pipe */
                    /* child processIfThen does command */
                    execvp(argv1[0], argv1);
                    exit(0);
            }
            /* 2nd command component of command line */
            close(STDIN_FILENO);
            dup(fildes[0]);
            close(fildes[0]);
            close(fildes[1]);
            /* standard input now comes from pipe */
            execvp(argv2[0], argv2);
            exit(0);
        
        } else {
            // printf("enter in else exec\n");
            execvp(argv1[0], argv1);
            exit(0);
        }
    }
    /* parent continues here */
    if (amper == 0) {
        retid = wait(&status);
    }
    return 1;
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

    // 11. Read shell
    if (strcmp(argv1[0], "read") == 0) {
        return readShell();
    }

    // 12. IfThen
    if (!strcmp(argv1[0], "if")) {
        return ifThen();
    }

    return 0;
}

int ifThen(){
        int j = 1;
        while (argv1[j] != NULL) {
            argv1[j-1] = argv1[j];
            j++;
        }
        argv1[j-1] = NULL;

        execute();
        int currstatus = WEXITSTATUS(status);//(args);

        char condition[MAX_LINE_LEN];
        if(!currstatus){
            if (fgets(condition, 1024, stdin) != NULL) {
                condition[strlen(condition) - 1] = '\0';
                if (!strcmp(condition,"then")){
                    if (fgets(condition, MAX_LINE_LEN, stdin) != NULL) {
                        condition[strlen(condition) - 1] = '\0';
                        int elseFlag = 1;
                        while(strcmp(condition, "fi")){
                            if(!strcmp(condition, "else")){
                                elseFlag = 0;
                            }
                            if(elseFlag){
                                parseCommand(condition);
                                execute();
                            }
                            if (fgets(condition, 1024, stdin) == NULL) {
                                break;
                            }
                            condition[strlen(condition) - 1] = '\0';
                        }
                    }
                }
                else{
                    printf("Bad if statement\n");
                    return 0;
                }
            }
        }
        else{
            if (fgets(condition, 1024, stdin) != NULL) {
                condition[strlen(condition) - 1] = '\0';
                while(strcmp(condition, "else")){
                        if (fgets(condition, 1024, stdin) == NULL) {
                            break;
                        }
                        condition[strlen(condition) - 1] = '\0';
                    }
                while(strcmp(condition, "fi")){
                        parseCommand(condition);
                        execute();
                        if (fgets(condition, 1024, stdin) == NULL) {
                            break;
                        }
                        condition[strlen(condition) - 1] = '\0';
                    }
            }
        }
        return 1;
    }


int echoShell() {
    int j = 1;
    while (argv1[j] != NULL) {
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
        j++;

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

int readShell() {
    char input[1024];
    fgets(input, 1024, stdin);
    input[strlen(input) - 1] = '\0';
    setenv(argv1[1], input, 1);
    return 1;
}


int changeDir() {
    if (i < 2) {
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


// int handle_arrows()
// {
//     int current_index = (history.last_index - 1) % history.capacity;
//     printf("history.cmd_history[current_index] = %s\n", history.cmd_history[current_index]);
//     printf("current_index = %d\n", current_index);
//     printf("history.last_index = %d\n", history.last_index);
//     printf("argc1 = %d\n", argc1);
//     printf("argv1[0][2] = %c\n", argv1[0][2]);

//     int ans = 0;
//     if ((argc1 == 1) && (history.cmd_history[current_index] != NULL) && (argv1[0][2] == 'A' || argv1[0][2] == 'B'))
//     {
//         printf("\033[1A"); // line up
//         printf("\x1b[2K"); // delete line
//         // printf("%s ", prompt);
//         printf("%s", history.cmd_history[current_index]);
//         ans = 1;
//         int c = 0;
//         while (c != 'Q' && c != '\n')
//         {
//             c = getchar();

//             if (c == '\033')
//             {
//                 // printf("\033[1A"); // line up
//                 printf("\x1b[2K"); // delete line 
//                 getchar();         // remove [
//                 switch (getchar())
//                 {
//                 case 'A': /* UP arrow */
//                     if (getchar() == '\n')
//                     {
//                         if (current_index == (history.first_index - 1 + history.capacity) % history.capacity)
//                         {
//                             // printf("\tlast command = %s  index = %d\n", history.cmd_history[current_index % history.capacity], current_index);
//                             printf("%s", history.cmd_history[current_index % history.capacity]);
//                         }
//                         else
//                         {
//                             current_index = (current_index - 1 + history.capacity) % history.capacity; 
//                             // printf("\tlast command = %s  index = %d\n", history.cmd_history[current_index % history.capacity], current_index);
//                             printf("%s", history.cmd_history[current_index % history.capacity]);
//                         }
//                     }
//                     break;

//                 case 'B': /* DOWN arrow*/
//                     if (getchar() == '\n')
//                     {
//                         if (current_index == (history.last_index - 1 + history.capacity) % history.capacity)
//                         {
//                             // printf("\tlast command = %s  index = %d\n", history.cmd_history[current_index % history.capacity], current_index);
//                             printf("%s", history.cmd_history[current_index % history.capacity]);
//                         }
//                         else
//                         {
//                             // printf("\tlast command = %s  index = %d\n", history.cmd_history[current_index % history.capacity], current_index);
//                             current_index = (current_index + 1) % history.capacity;
//                             printf("%s", history.cmd_history[current_index % history.capacity]);
//                         }
//                     }
//                     break;
//                 default:
//                     break;
//                 }
//             }
//               else if (c == '\n') /* if the user press Enter - repeat the command*/
//             {
//                 memset(command, 0, sizeof(command));
//                 strcpy(command, history.cmd_history[current_index]);
//                 // printf("after strcpy command = %s\n", command);
//                 handleShellCommands();
//                 // printf("after execute command = %s\n", command);
//                 fflush(stdout);
//                 current_index = (history.last_index - 1) % 20;
//                 printf("%s", history.cmd_history[current_index]);
//             }
//         }
//     }
//     return ans;
    
// }


int handle_arrows()
{
    static int history_index = -1;

    if (argc1 == 1 && (argv1[0][2] == 'A' || argv1[0][2] == 'B')) {
        // If arrow up or arrow down key is pressed
        if (argv1[0][2] == 'A' && history_index < history.cmd_count - 1) {
            // Move up in history
            history_index++;
        } else if (argv1[0][2] == 'B' && history_index > -1) {
            // Move down in history
            history_index--;
        }
        
        // If a valid history entry is selected, replace the current command with the selected command
        if (history_index >= 0 && history_index < history.cmd_count) {
            // Delete the current line
            printf("\033[1A");
            printf("\x1b[2K");
            
            int cmd_idx = (history.last_index - 1 - history_index + history.capacity) % history.capacity;
            strncpy(command, history.cmd_history[cmd_idx], MAX_LINE_LEN);
            printf("%s", command);
        }
    } else {
        // Reset history index when a new command is entered
        history_index = -1;
    }
    
    return 0;
}



int main(){
   
    // Ctrl-C sigaction
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa, NULL);
    sigsetjmp(jmpbuf, 1);


    memset(last_command, 0, MAX_LINE_LEN + 1);
    initHistory(&history, 20);
    

    while (1) {
        resetGlobalVars();

        if (getCommand() < 0) {
            break;
        }
        addHistoryEntry(&history, command);
        parseCommand(command);

        
        handle_arrows();
        

        if (argv1[0] == NULL) {
            continue;
        } else if (strcmp(argv1[0], "quit") == 0) {
            break;
        }


        if (!strcmp(argv1[argc1 - 1], "&")) {
            amper = 1;
            argv1[argc1 - 1] = NULL;
        }


        execute();
  
    }
    freeHistory(&history);
    sigsetjmp(sigtstp_jmpbuf, 0);
    return 0;
}

