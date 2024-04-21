#include "syscall.h"

int main() {
    char* buffer = "Temporary buffer";
    int totalLen = 0, curLen = 0, i = 0, curNum = 0;
    int isRun = 1; // Thay thế cho biến bool
    int time = 0;
    int time1 = 0;
    int time2 = 0;
    int time3 = 0;
    OpenFileId testFile = Open("input.txt", 1);

    if (testFile == -1) {
        PrintString("Error opening file\n");
    } else {
        // Đọc số thời gian
        while (Read(buffer, 1, testFile) > 0) {
            if (buffer[0] >= 48 && buffer[0] <= 57) {
                totalLen = totalLen * 10 + (buffer[0] - 48);
            } else if (buffer[0] == '\n' || buffer[0] == ' ') break;
            else {
                Close(testFile);
                Halt();
            }
        }

        if (totalLen == 0) { // Nếu không có mốc thời gian nào
            Halt();
        }

        for (i = 0; i < totalLen; i++) {
	        isRun = 1;
            curNum = 0;
            while (isRun == 1) {
                curLen = Read(buffer, 1, testFile);
                if (curLen == -1) {
                    Close(testFile);
                    Halt();
                }

                if (buffer[0] >= 48 && buffer[0] <= 57) {
                    curNum = curNum * 10 + (buffer[0] - 48);
                } else if (buffer[0] == ' ' || buffer[0] == '\n' || curLen == 0) { // Đã đọc xong số hiện tại
                    ////// YOUR CODE GOES HERE

                    ////// Nhớ lưu curNum và số thứ tự của máy vào file
                    PrintInt(curNum);
                    curNum = 0;
                }
                else { // Kí tự khác 
                    Close(testFile);
                    Halt();
                }
		        if (buffer[0] == '\n' || curLen == 0) isRun = 0; // Gặp dấu xuống dòng thì nghĩa là xong mốc thời gian hiện tại
            }
        }
    }

    Close(testFile);
    Halt();
}