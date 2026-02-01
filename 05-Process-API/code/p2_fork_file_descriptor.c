#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd = open("test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fork() == 0) {
        write(fd, "child\n", 6);
    }
    
    else {
        write(fd, "parent\n", 7);
    }
    
    return 0;
}