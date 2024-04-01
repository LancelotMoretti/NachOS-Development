#include "syscall.h"

int main() {
    OpenFileId helpFile = Open("mota.txt");

    char *buffer;
    while (Read(buffer, 1, helpFile) >= 0) PrintChar(*buffer);

    Close(helpFile);
    Halt();
}