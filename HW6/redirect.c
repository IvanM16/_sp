#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    int fd;
    int status;

    printf("=== Demo 1: Redirect stdout to a file ===\n");

    pid = fork();
    if (pid == 0) {
        fd = open("redirect_out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) { perror("open"); exit(1); }

        dup2(fd, STDOUT_FILENO);
        close(fd);

        char *args[] = {"ls", "-la", NULL};
        execvp("ls", args);
        perror("execvp");
        exit(1);
    }
    waitpid(pid, &status, 0);
    printf("[Parent] ls -la output written to redirect_out.txt\n\n");

    printf("=== Demo 2: Redirect stdin from a file ===\n");

    int pipe_fd[2];
    if (pipe(pipe_fd) < 0) { perror("pipe"); exit(1); }

    pid = fork();
    if (pid == 0) {
        close(pipe_fd[1]);
        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[0]);
        char *args[] = {"wc", "-l", NULL};
        execvp("wc", args);
        perror("execvp");
        exit(1);
    }

    close(pipe_fd[0]);
    dprintf(pipe_fd[1], "line 1\nline 2\nline 3\n");
    close(pipe_fd[1]);
    waitpid(pid, &status, 0);

    printf("=== Demo 3: Redirect stderr to a file ===\n");

    pid = fork();
    if (pid == 0) {
        fd = open("redirect_err.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) { perror("open"); exit(1); }

        dup2(fd, STDERR_FILENO);
        close(fd);

        fprintf(stderr, "This message goes to redirect_err.txt instead of terminal!\n");
        fprintf(stdout, "stdout still goes to terminal\n");
        exit(0);
    }
    waitpid(pid, &status, 0);
    printf("[Parent] Check redirect_err.txt for the stderr message\n\n");

    printf("=== File descriptor table ===\n");
    printf("stdin  = %d\n", STDIN_FILENO);
    printf("stdout = %d\n", STDOUT_FILENO);
    printf("stderr = %d\n", STDERR_FILENO);

    return 0;
}
