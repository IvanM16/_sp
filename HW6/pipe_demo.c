#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MSG "Hello from parent via pipe!"

int main() {
    int pipe_fd[2];
    pid_t pid;
    int status;

    if (pipe(pipe_fd) < 0) {
        perror("pipe");
        exit(1);
    }
    printf("Created pipe: fd[0]=%d (read), fd[1]=%d (write)\n",
           pipe_fd[0], pipe_fd[1]);

    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        close(pipe_fd[1]);

        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[0]);

        char *args[] = {"cat", NULL};
        printf("[Child PID=%d] Executing 'cat' to read from pipe...\n", getpid());
        fflush(stdout);

        execvp("cat", args);
        perror("execvp cat");
        exit(1);
    } else {
        close(pipe_fd[0]);

        printf("[Parent PID=%d] Writing message to pipe...\n", getpid());
        write(pipe_fd[1], MSG, strlen(MSG));
        write(pipe_fd[1], "\n", 1);

        close(pipe_fd[1]);

        waitpid(pid, &status, 0);
        printf("[Parent PID=%d] Child finished, exit status=%d\n",
               getpid(), WEXITSTATUS(status));
    }

    return 0;
}
