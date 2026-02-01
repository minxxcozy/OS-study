#include <stdio.h>
#include <unistd.h>

int main() {
    int x = 100;

    int rc = fork();

    if (rc == 0) {          // child
        x = 200;
        printf("child x = %d\n", x);
    }
    
    else {                  // parent
        x = 300;
        printf("parent x = %d\n", x);
    }
    
    return 0;
}