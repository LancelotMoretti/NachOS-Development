#include "syscall.h"

int main() {
    int i;
    for (i = 0; i < 10; i++) {
        Down("First");
        PrintChar('A');
        Up("Second");
    }
    return 0;
}