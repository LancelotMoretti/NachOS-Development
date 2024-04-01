#include "syscall.h"

int main() {
    OpenFileId helpFile = Open("mota.txt", 1);

    if (helpFile == -1) {
        PrintString("Error opening file\n");
    }
    else {
        char* buffer = "Temporary buffer";
        while (Read(buffer, 1, helpFile) > 0) PrintChar(buffer[0]);
        PrintChar('\n');
        Close(helpFile);
    }
    Halt();
}