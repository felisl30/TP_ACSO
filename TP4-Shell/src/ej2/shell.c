#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_COMMANDS 200

int main() {

    char command[256];
    char *commands[MAX_COMMANDS];
    int command_count = 0;


    while (1) 
    {
        printf("Shell> ");
        
        /*Reads a line of input from the user from the standard input (stdin) and stores it in the variable command */
        fgets(command, sizeof(command), stdin);
        
        /* Removes the newline character (\n) from the end of the string stored in command, if present. 
           This is done by replacing the newline character with the null character ('\0').
           The strcspn() function returns the length of the initial segment of command that consists of 
           characters not in the string specified in the second argument ("\n" in this case). */
        command[strcspn(command, "\n")] = '\0';

        /* Tokenizes the command string using the pipe character (|) as a delimiter using the strtok() function. 
           Each resulting token is stored in the commands[] array. 
           The strtok() function breaks the command string into tokens (substrings) separated by the pipe character |. 
           In each iteration of the while loop, strtok() returns the next token found in command. 
           The tokens are stored in the commands[] array, and command_count is incremented to keep track of the number of tokens found. */
        char *token = strtok(command, "|");
        while (token != NULL) 
        {
            commands[command_count++] = token;
            token = strtok(NULL, "|");
        }

        /* You should start programming from here... */

        if (command_count == 0)
            continue;

        char *temp_args[100];
        int arg_i = 0;
        char* temp_cmd = malloc(strlen(commands[0]) + 1);
        strcpy(temp_cmd, commands[0]);
        char *arg = strtok(temp_cmd, " ");
        while (arg != NULL) {
            temp_args[arg_i++] = arg;
            arg = strtok(NULL, " ");
        }
        temp_args[arg_i] = NULL;

        if (temp_args[0] && strcmp(temp_args[0], "exit") == 0) {
            free(temp_cmd);
            exit(0);
        }
        free(temp_cmd);


        //defino los pipeS
        int pipes[MAX_COMMANDS - 1][2];
        for (int i = 0; i < command_count - 1; i++) {
            if (pipe(pipes[i]) == -1) {
                perror("pipe");
                exit(1);
            }
        }

        for (int i = 0; i < command_count; i++) {
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                exit(1);
            }

            if (pid == 0) {
                
                if (i > 0)
                    dup2(pipes[i - 1][0], STDIN_FILENO);

                
                if (i < command_count - 1)
                    dup2(pipes[i][1], STDOUT_FILENO);

                
                for (int j = 0; j < command_count - 1; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                
                char *args[100];
                int arg_i = 0;
                char *arg = strtok(commands[i], " ");
                while (arg != NULL) {
                    args[arg_i++] = arg;
                    arg = strtok(NULL, " ");
                }
                args[arg_i] = NULL;

                if (args[0] && strcmp(args[0], "exit") == 0)
                    exit(0);

                execvp(args[0], args);
                perror("execvp");
                exit(1);
            }
        }

       
        for (int i = 0; i < command_count - 1; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        for (int i = 0; i < command_count; i++){
            wait(NULL);
            }

        command_count = 0;
    }
}
