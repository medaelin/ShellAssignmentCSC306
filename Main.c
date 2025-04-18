#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_LINE 80 /* The maximum length command */

int main(void)
{
    char *args[MAX_LINE/2 + 1]; /* command line arguments */
    char input[MAX_LINE]; // user input
    char last_command[MAX_LINE] = ""; // variable to save the last command
    int should_run = 1;         /* flag to determine when to exit program */

    int redirect = 0;
    int redirect_type = 0;
    char *filename = NULL;


    while (should_run) {
        printf("osh> ");
        fflush(stdout);

        /**
         * After reading user input, the steps are:
         * (1) fork a child process using fork()
         * (2) the child process will invoke execvp()
         * (3) parent will invoke wait() unless command included &
         */

        // Read the input from the user 
        if (fgets(input, MAX_LINE, stdin) == NULL) {
            continue;
        }

        // Remove trailing newline character
        input[strcspn(input, "\n")] = '\0';

        // Exit command
        if (strcmp(input, "exit") == 0) {
            should_run = 0;
            continue;
        }
        // Check history command
        if (strcmp(input, "!!") == 0) {
            if (strlen(last_command) == 0) {
                printf("No commands in history.\n");
                continue;
            } else {
                printf("%s\n", last_command); // Shows what command will be executed
                strcpy(input, last_command); // Reuse the last command
            }
        } else {
            // Save the command to history
            strcpy(last_command, input);
        }

        // Tokenize the input 
        int i = 0;
        char *token = strtok(input, " ");

        while (token != NULL && i < MAX_LINE / 2) {
           args[i++] = token;
           token = strtok(NULL, " ");
        }
        args[i] = NULL;

        if (pid < 0) {
           perror("Fork did not work");
        }
        else if (pid == 0) {
           // child process will invoke execvp()
           if (redirect) {
            int fd;

            if(redirect_type == 1){
                fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd < 0) {
                    perror("Writing: Failed to open file");
                    exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            }
            else if(redirect_type == 2){
                fd = open(filename, O_RDONLY);
                if (fd < 0) {
                    perror("Reading: Failed to open file");
                    exit(1);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
           }
           execvp(args[0], args);
           perror("Failed execution");
           exit(1);
        }
    }
        else {
           // parent will invoke wait() unless command included 
           wait(NULL);
        }

        // Part 3 is to implement redirection feature:

        for (int j = 0; args[j] != NULL; j++) {
            if (strcmp(args[j], ">") == 0) {
                redirect = 1;
                redirect_type = 1; // Output redirection
                args[j] = NULL; // Terminate the command arguments
                filename = args[j + 1];
                break;
            } 
            else if (strcmp(args[j], "<") == 0) {
                redirect = 1;
                redirect_type = 2; // Input redirection
                args[j] = NULL; // Terminate the command arguments
                filename = args[j + 1];
                break;
            }
        }

         // Fork a child using fork()
         pid_t pid = fork();

    }

    return 0;
}
