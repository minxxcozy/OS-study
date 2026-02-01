#include <stdio.h>
#include <unistd.h>

int main() {
    int fd[2];
    pipe(fd);

    if (fork() == 0) {          // writer
        close(fd[0]);
        write(fd[1], "hello\n", 6);
    }
    
    else {                      // reader
        close(fd[1]);
        char buf[10];
        read(fd[0], buf, 10);
        printf("%s", buf);
    }
    
    return 0;
}
