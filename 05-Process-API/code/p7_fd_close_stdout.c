#include <stdio.h>
#include <unistd.h>

int main() {
    if (fork() == 0) {
        close(STDOUT_FILENO);
        printf("hello?\n");
    }
    return 0;
}
