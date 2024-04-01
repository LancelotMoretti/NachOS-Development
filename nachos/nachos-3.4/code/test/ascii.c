#include "syscall.h"

int main() {
    Create("ascii.txt");
    OpenFileId asciiFile = Open("ascii.txt", 0);

    if (asciiFile == -1) {
        PrintString("Error opening file\n");
    }
    else {
        // In header ra màn hình và ghi header vào file ascii.txt
        for (int i = 0; i < 8; i++) {
            char *header;
            *header = "Dec  Char  ";
            PrintString(header);
            Write(header, 12, asciiFile);
        }

        // Xuống dòng để kết thúc header
        char *endLine;
        *endLine = '\n';
        PrintChar(endLine);
        Write(endLine, 1, asciiFile);

        // In nội dung bảng ascii ra màn hình và ghi vào file ascii.txt
        for (int i = 32; i <= 127; i += 8) { // Chỉ in các kí tự ascii từ 32 tới 127
            for (int j = 0; j < 8; j++) {
                char *num; // Temp

                if (i < 100) { // Trường hợp dạng decimal có 2 chữ số
                    // Chuyển i từ kiểu int sang kiểu char
                    char temp[3];
                    num = &temp;
                    temp[0] = i / 10 + 48; // Kí tự hàng chục
                    temp[1] = i % 10 + 48; // Kí tự hàng đơn vị
                    temp[2] = '\0';
                
                    // In dạng decimal
                    PrintInt(i);
                    Write(num, 2, asciiFile); 
                    
                    // In khoảng trắng sau dạng decimal
                    char *space;
                    *space = "   ";
                    PrintString(space);
                    Write(space, 3, asciiFile);
                }
                else { // Trường hợp dạng decimal có 3 chữ số
                    // Chuyển i từ kiểu int sang kiểu char
                    char temp[4];
                    num = &temp;
                    temp[0] = 49; // Kí tự hàng trăm: 1
                    temp[1] = (i - 100) / 10 + 48; // Kí tự hàng chục
                    temp[2] = (i - 100) % 10 + 48; // Kí tự hàng đơn vị
                    temp[3] = '\0';

                    // In dạng decimal 
                    PrintInt(i);
                    Write(num, 3, asciiFile);

                    // In khoảng trắng sau dạng decimal
                    char *space;
                    *space = "  ";
                    PrintString(space);
                    Write(space, 2, asciiFile);
                }

                // In dạng char
                char *cur = i;
                PrintChar(cur);
                Write(cur, 1, asciiFile);

                // In khoảng trắng sau dạng char
                char *space;
                *space = "     ";
                PrintString(space);
                Write(space, 5, asciiFile);
            }

            // Xuống dòng
            PrintChar(endLine);
            Write(endLine, 1, asciiFile);
        }
    }

    Close(asciiFile);
    Halt();
}