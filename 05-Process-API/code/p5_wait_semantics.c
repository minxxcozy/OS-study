#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int rc = fork();
    
    if (rc == 0) {
        return 0;
    }
    
    else {
        int pid = wait(NULL);
        printf("wait returned pid = %d\n", pid);
    }
    
    return 0;
}
