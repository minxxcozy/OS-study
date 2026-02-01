#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int pid = fork();
    
    if (pid > 0) {
        waitpid(pid, NULL, 0);
        printf("child done!!\n");
    }
    
    return 0;
}
