#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 80 /* The maximum length command */

int main(void)
{
    char *args[MAX_LINE/2 + 1]; /* command line arguments */
    char input[MAX_LINE]; // user input
    int should_run = 1;         /* flag to determine when to exit program */

    while (should_run) {
        printf("osh> ");
        fflush(stdout);

        /**
         * After reading user input, the steps are:
         * (1) fork a child process using fork()
         * (2) the child process will invoke execvp()
         * (3) parent will invoke wait() unless command included &
         */

        // Tokenize the input 
         int i = 0;
         char *token = strtok(input, " ");

         while (token != NULL && i < MAX_LINE / 2) {
            args[i++] = token;
            token = strtok(NULL, " ");
         }
         args[i] = NULL;

        // Fork a child using fork()
         pid_t pid = fork();

         if (pid < 0) {
            perror("Fork did not work");
         }
         else if (pid == 0) {
            // child process will invoke execvp()
            execvp(args[0], args);
            perror("Failed execution");
            exit(1);
         }
         else {
            // parent will invoke wait() unless command included &
            wait(NULL);
         }

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

    }

    return 0;
}
