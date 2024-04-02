#include "syscall.h"

int main() {
    //// Khai báo
    int i = 0, j = 0, k = 0; // Biến đếm
    char *header = "Dec  Char  ", *endLine = '\n', *cur, *space1 = "   ", *space2 = "  ", *space3 = "     ", *num;
    char temp[4];
    num = &temp;
    OpenFileId asciiFile; // Con trỏ file

    //// Nội dung chương trình
    Create("ascii.txt"); // Tạo file
    asciiFile = Open("ascii.txt", 0); // Mở file

    if (asciiFile == -1) {
        PrintString("Error opening file\n");
    }
    else {
        // In header ra màn hình và ghi header vào file ascii.txt
        for (i; i < 8; i++) {
            PrintString(header);
            Write(header, 12, asciiFile);
        }

        // Xuống dòng để kết thúc header
        PrintChar(endLine);
        Write(endLine, 1, asciiFile);

        // In nội dung bảng ascii ra màn hình và ghi vào file ascii.txt
        for (i = 32; i <= 127; i += 8) { // Chỉ in các kí tự ascii từ 32 tới 127
            for (j; j < 8; j++) {
                if (i < 100) { // Trường hợp dạng decimal có 2 chữ số
                    // Chuyển i từ kiểu int sang kiểu char
                    temp[0] = i / 10 + 48; // Kí tự hàng chục
                    temp[1] = i % 10 + 48; // Kí tự hàng đơn vị
                    temp[2] = '\0';
                    temp[3] = '\0';

                    // In dạng decimal
                    PrintInt(i);
                    Write(num, 2, asciiFile); 
                    
                    // In khoảng trắng sau dạng decimal
                    PrintString(space1);
                    Write(space1, 3, asciiFile);
                }
                else { // Trường hợp dạng decimal có 3 chữ số
                    // Chuyển i từ kiểu int sang kiểu char
                    num = &temp;
                    temp[0] = 49; // Kí tự hàng trăm: 1
                    temp[1] = (i - 100) / 10 + 48; // Kí tự hàng chục
                    temp[2] = (i - 100) % 10 + 48; // Kí tự hàng đơn vị
                    temp[3] = '\0';

                    // In dạng decimal 
                    PrintInt(i);
                    Write(num, 3, asciiFile);

                    // In khoảng trắng sau dạng decimal
                    PrintString(space2);
                    Write(space2, 2, asciiFile);
                }

                // In dạng char
                *cur = i;
                PrintChar(cur);
                Write(cur, 1, asciiFile);

                // In khoảng trắng sau dạng char
                PrintString(space3);
                Write(space3, 5, asciiFile);
            }

            // Xuống dòng
            PrintChar(endLine);
            Write(endLine, 1, asciiFile);
        }
    }

    Close(asciiFile); // Đóng file
    Halt();
}