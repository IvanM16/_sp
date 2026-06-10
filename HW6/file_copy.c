#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        return 1;
    }

    int src_fd = open(argv[1], O_RDONLY);
    if (src_fd < 0) {
        perror("open source failed");
        return 1;
    }
    printf("Opened source '%s' as fd=%d\n", argv[1], src_fd);

    int dst_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd < 0) {
        perror("open destination failed");
        close(src_fd);
        return 1;
    }
    printf("Opened destination '%s' as fd=%d\n", argv[2], dst_fd);

    char buf[BUF_SIZE];
    ssize_t bytes_read, bytes_written;
    ssize_t total = 0;

    while ((bytes_read = read(src_fd, buf, BUF_SIZE)) > 0) {
        bytes_written = write(dst_fd, buf, bytes_read);
        if (bytes_written != bytes_read) {
            perror("write error");
            close(src_fd);
            close(dst_fd);
            return 1;
        }
        total += bytes_written;
    }

    if (bytes_read < 0) {
        perror("read error");
    }

    printf("Copied %ld bytes\n", total);

    close(src_fd);
    close(dst_fd);
    printf("Closed both file descriptors\n");

    return 0;
}
