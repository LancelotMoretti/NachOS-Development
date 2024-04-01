#include "syscall.h"
#include "copyright.h"

int main() {
    int n;
    char c;
    n = ReadInt();
    c = ReadChar();
    PrintInt(n);
    PrintChar(c);

    Halt();
}