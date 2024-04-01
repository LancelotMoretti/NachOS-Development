#include "syscall.h"
#include "copyright.h"
#define maxlen 32

int main() {
    int len;
    char filename[] = "text.txt";

    /*Create a file*/
    if (Create(filename) == -1) {
        PrintString("\n Create file ~");
        PrintString(filename);
        PrintString(" failed.\n");
    }
    else {
        PrintString("\n Create file ~");
        PrintString(filename);
        PrintString(" successfully.\n");
    }

    Halt();
}