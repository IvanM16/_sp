#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#define MAX_INPUT  1024
#define MAX_ARGS   128
#define MAX_PIPE   16
#define PROMPT     "myshell> "

volatile sig_atomic_t child_exited = 0;

void sigchld_handler(int sig) {
    (void)sig;
    child_exited = 1;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void sigint_handler(int sig) {
    (void)sig;
    write(STDOUT_FILENO, "\n", 1);
}

int parse_line(char *line, char *args[], int *background) {
    int i = 0;
    char *token = strtok(line, " \t\n\r");
    *background = 0;

    while (token != NULL && i < MAX_ARGS - 1) {
        if (strcmp(token, "&") == 0) {
            *background = 1;
            break;
        }
        args[i++] = token;
        token = strtok(NULL, " \t\n\r");
    }
    args[i] = NULL;
    return i;
}

void split_pipe(char *args[], char *left_args[], char *right_args[]) {
    int i = 0, j = 0;
    int pipe_pos = -1;

    for (i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "|") == 0) {
            pipe_pos = i;
            break;
        }
    }

    if (pipe_pos == -1) return;

    for (i = 0; i < pipe_pos; i++)
        left_args[i] = args[i];
    left_args[pipe_pos] = NULL;

    j = 0;
    for (i = pipe_pos + 1; args[i] != NULL; i++)
        right_args[j++] = args[i];
    right_args[j] = NULL;
}

int has_pipe(char *args[]) {
    for (int i = 0; args[i] != NULL; i++)
        if (strcmp(args[i], "|") == 0)
            return 1;
    return 0;
}

int setup_redirections(char *args[]) {
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">") == 0) {
            if (args[i + 1] == NULL) {
                fprintf(stderr, "myshell: missing filename after '>'\n");
                return -1;
            }
            int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) { perror("myshell"); return -1; }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            args[i] = NULL;
        } else if (strcmp(args[i], ">>") == 0) {
            if (args[i + 1] == NULL) {
                fprintf(stderr, "myshell: missing filename after '>>'\n");
                return -1;
            }
            int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd < 0) { perror("myshell"); return -1; }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            args[i] = NULL;
        } else if (strcmp(args[i], "<") == 0) {
            if (args[i + 1] == NULL) {
                fprintf(stderr, "myshell: missing filename after '<'\n");
                return -1;
            }
            int fd = open(args[i + 1], O_RDONLY);
            if (fd < 0) { perror("myshell"); return -1; }
            dup2(fd, STDIN_FILENO);
            close(fd);
            args[i] = NULL;
        }
    }
    return 0;
}

int run_builtin(char *args[]) {
    if (args[0] == NULL) return 1;

    if (strcmp(args[0], "exit") == 0) {
        printf("myshell: goodbye\n");
        exit(0);
    }
    if (strcmp(args[0], "cd") == 0) {
        const char *dir = args[1] ? args[1] : getenv("HOME");
        if (!dir) dir = "/";
        if (chdir(dir) != 0)
            perror("cd");
        return 1;
    }
    if (strcmp(args[0], "pwd") == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)))
            printf("%s\n", cwd);
        else
            perror("pwd");
        return 1;
    }
    if (strcmp(args[0], "help") == 0) {
        printf("myshell - A simple Linux shell\n"
               "Built-in commands: cd, pwd, exit, help\n"
               "Features: I/O redirection (<, >, >>), pipe (|), background (&)\n"
               "Example: ls -l | grep .c > out.txt &\n");
        return 1;
    }
    return 0;
}

int execute_single(char *args[], int background) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    }
    if (pid == 0) {
        signal(SIGINT, SIG_DFL);
        if (setup_redirections(args) < 0)
            exit(1);
        execvp(args[0], args);
        fprintf(stderr, "myshell: %s: command not found\n", args[0]);
        exit(127);
    }

    if (!background) {
        int status;
        waitpid(pid, &status, 0);
    } else {
        printf("[%d] %s\n", pid, args[0]);
    }
    return 0;
}

int execute_pipe(char *args[]) {
    char *left_args[MAX_ARGS], *right_args[MAX_ARGS];
    split_pipe(args, left_args, right_args);
    if (left_args[0] == NULL || right_args[0] == NULL) {
        fprintf(stderr, "myshell: invalid pipe command\n");
        return -1;
    }

    int pipe_fd[2];
    if (pipe(pipe_fd) < 0) { perror("pipe"); return -1; }

    pid_t left_pid = fork();
    if (left_pid < 0) { perror("fork"); return -1; }
    if (left_pid == 0) {
        signal(SIGINT, SIG_DFL);
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
        setup_redirections(left_args);
        execvp(left_args[0], left_args);
        fprintf(stderr, "myshell: %s: command not found\n", left_args[0]);
        exit(127);
    }

    pid_t right_pid = fork();
    if (right_pid < 0) { perror("fork"); return -1; }
    if (right_pid == 0) {
        signal(SIGINT, SIG_DFL);
        close(pipe_fd[1]);
        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[0]);
        setup_redirections(right_args);
        execvp(right_args[0], right_args);
        fprintf(stderr, "myshell: %s: command not found\n", right_args[0]);
        exit(127);
    }

    close(pipe_fd[0]);
    close(pipe_fd[1]);
    waitpid(left_pid, NULL, 0);
    waitpid(right_pid, NULL, 0);
    return 0;
}

int execute_command(char *args[], int background) {
    if (args[0] == NULL) return 1;

    if (run_builtin(args))
        return 0;

    if (has_pipe(args))
        return execute_pipe(args);

    return execute_single(args, background);
}

int main() {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];
    int background;

    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, sigchld_handler);

    printf(
        "╔══════════════════════════════════════╗\n"
        "║          Welcome to myshell          ║\n"
        "║  Built-in: cd pwd exit help         ║\n"
        "║  Redirect: <  >  >>                  ║\n"
        "║  Pipe:    |                          ║\n"
        "║  BG:      &                          ║\n"
        "╚══════════════════════════════════════╝\n\n");

    while (1) {
        if (child_exited) {
            child_exited = 0;
        }

        printf(PROMPT);
        fflush(stdout);

        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            printf("\n");
            break;
        }

        if (input[0] == '\n') continue;

        int argc = parse_line(input, args, &background);
        if (argc == 0) continue;

        execute_command(args, background);
    }

    printf("myshell: goodbye\n");
    return 0;
}
