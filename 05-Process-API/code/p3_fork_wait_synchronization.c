#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    if (fork() == 0) {
        printf("hello?\n");
    }
    
    else {
        wait(NULL);
        printf("goodbye!\n");
    }
    
    return 0;
}
