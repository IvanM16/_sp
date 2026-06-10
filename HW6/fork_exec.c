#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    printf("[Parent PID=%d] Before fork\n", getpid());

    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        printf("[Child  PID=%d] I am the child, parent PID=%d\n",
               getpid(), getppid());
        printf("[Child  PID=%d] Executing 'ls -l' via execvp...\n", getpid());

        char *args[] = {"ls", "-l", NULL};
        execvp("ls", args);

        perror("execvp failed");
        exit(1);
    } else {
        printf("[Parent PID=%d] Forked child PID=%d\n", getpid(), pid);
        printf("[Parent PID=%d] Waiting for child to finish...\n", getpid());

        int status;
        pid_t waited = waitpid(pid, &status, 0);

        if (waited == -1) {
            perror("waitpid failed");
        } else if (WIFEXITED(status)) {
            printf("[Parent PID=%d] Child PID=%d exited with status %d\n",
                   getpid(), waited, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("[Parent PID=%d] Child PID=%d killed by signal %d\n",
                   getpid(), waited, WTERMSIG(status));
        }

        printf("[Parent PID=%d] Done\n", getpid());
    }

    return 0;
}
