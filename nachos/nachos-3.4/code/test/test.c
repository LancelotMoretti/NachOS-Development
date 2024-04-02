#include "syscall.h"
#include "copyright.h"

int main() {
    char buffer[10];
    ReadString(buffer, 10);
    PrintString(buffer);
    Halt();
}