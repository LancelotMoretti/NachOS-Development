#include "syscall.h"

int main() {
    int i;
    for (i = 0; i < 1000; i++) {
        Down("First");
        PrintString("A");
        Up("Second");
    }
    return 0;
}