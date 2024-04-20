#include "syscall.h"

int main() {
    //// Khai báo
    int *listTime = 0;
    int i = 0, j = 0, size = 0, index = 0, curSize = 0;
    OpenFileId testFile = Open("input.txt", 1);

    if (testFile == -1) {
        PrintString("Error opening file\n");
    }
    else {
        ReadFileFormat(listTime, testFile);
        size = listTime[index++];
        PrintInt(size);
        for (i; i < size; i++) {
            curSize = listTime[index++];
            PrintInt(curSize);
            for (j = 0; j < curSize; j++) {
                PrintInt(listTime[index++]);
            }
        }
    }
    Close(testFile);
    Halt();
}