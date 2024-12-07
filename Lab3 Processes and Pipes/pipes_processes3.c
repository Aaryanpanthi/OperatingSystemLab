#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h> // Fix for exit() and EXIT_FAILURE
#include <sys/wait.h> 

/**
 * Executes the command "cat scores | grep Lakers | sort".  
 * In this quick-and-dirtywha implementation, the parent doesn't wait for 
 * the child to finish, so the command prompt may reappear before the child terminates.
 */

int main(int argc, char **argv)
{

    if (argc != 2) {
      fprintf(stderr,"Usage %s <grep_argument>\n", argv[0]);
      exit(EXIT_FAILURE);
    }

    int pipe1[2];
    int pipe2[2];
    pid_t pid1, pid2;

    char *cat_args[] = {"cat", "scores", NULL};
    char *grep_args[] = {"grep", argv[1], NULL};
    char *sort_args[] = {"sort", NULL};

    pipe(pipe1);
    pipe(pipe2);

    pid1 = fork();

    if (pid1 == 0)
    {
        // Child process for "grep Lakers"
        pid2 = fork();

        if (pid2 == 0)
        {
            // Grandchild process for "sort"
            close(pipe1[0]);
            close(pipe1[1]);

            close(pipe2[1]);
            dup2(pipe2[0], STDIN_FILENO);
            close(pipe2[0]);

            execvp("sort", sort_args);
        }
        else
        {
            close(pipe1[1]);
            dup2(pipe1[0], STDIN_FILENO);
            close(pipe1[0]);

            close(pipe2[0]);
            dup2(pipe2[1], STDOUT_FILENO);
            close(pipe2[1]);

            execvp("grep", grep_args);
        }
    }
    else
    {
        // Parent process for "cat scores"
        close(pipe1[0]);
        dup2(pipe1[1], STDOUT_FILENO);
        close(pipe1[1]);

        execvp("cat", cat_args);
    }

    return 0;
}
