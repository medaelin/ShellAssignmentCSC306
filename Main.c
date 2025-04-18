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

    while (should_run) {
        printf("osh> ");
        fflush(stdout);

        if (fgets(input, MAX_LINE, stdin) == NULL) continue;

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0) {
            should_run = 0;
            continue;
        }

        if (strcmp(input, "!!") == 0) {
            if (strlen(last_command) == 0) {
                printf("No commands in history.\n");
                continue;
            } else {
                printf("%s\n", last_command);
                strcpy(input, last_command);
            }
        } else {
            strcpy(last_command, input);
        }

        // Tokenize
        int i = 0;
        char *token = strtok(input, " ");
        while (token != NULL && i < MAX_LINE / 2) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        // Check for redirection
        int redirect = 0;
        int redirect_type = 0;
        char *filename = NULL;

        for (int j = 0; args[j] != NULL; j++) {
            if (strcmp(args[j], ">") == 0) {
                redirect = 1;
                redirect_type = 1; // Output
                filename = args[j + 1];
                args[j] = NULL;
                break;
            } else if (strcmp(args[j], "<") == 0) {
                redirect = 1;
                redirect_type = 2; // Input
                filename = args[j + 1];
                args[j] = NULL;
                break;
            }
        }

        // Now fork
        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed");
        } else if (pid == 0) {
            // In child
            if (redirect) {
                int fd;
                if (redirect_type == 1) {
                    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0) {
                        perror("Failed to open file for writing");
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                } else if (redirect_type == 2) {
                    fd = open(filename, O_RDONLY);
                    if (fd < 0) {
                        perror("Failed to open file for reading");
                        exit(1);
                    }
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                }
            }

            execvp(args[0], args);
            perror("Execution failed");
            exit(1);
        } else {
            // In parent
            wait(NULL);
        }
    }

    return 0;
}
