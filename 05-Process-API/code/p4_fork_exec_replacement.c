#include <unistd.h>

int main() {
    if (fork() == 0) {
        execl("/bin/ls", "ls", NULL);
    }
    
    return 0;
}
