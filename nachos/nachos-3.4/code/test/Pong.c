#include "syscall.h"

int main() {
    int i;
    for (i = 0; i < 10; i++) {
        Down("Second");
        PrintChar('B');
        Up("First");
    }
    return 0;
}