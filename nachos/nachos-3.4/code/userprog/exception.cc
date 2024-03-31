// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

#define MaxFileLength 32 // Chiều dài tên file tối đa
#define MaxString 256 // Chiều dài chuỗi tối đa

// System call: copy string from User memory to System memory
char* User2System(int virtAddr, int limit) {
    int i; // index
    int oneChar;
    char* kernelBuf = NULL;
    kernelBuf = new char[limit +1]; //need for terminal string
    if (kernelBuf == NULL)
        return kernelBuf;
    memset(kernelBuf, 0, limit+1);
    //printf("\n Filename u2s: ");
    for (i = 0; i < limit; i++)
    {
        machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;
        if (oneChar == 0)
            break;
    }
    return kernelBuf;
}

// System call: copy string from System memory to User memory
int System2User(int virtAddr, int len, char* buffer) {
    if (len < 0) return -1;
    if (len == 0) return len;
    int i = 0;
    int oneChar = 0;
    do
    {
        oneChar = (int) buffer[i];
        machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < len && oneChar != 0);
    return i;
}

void IncPCReg() {
    int curPC = machine->ReadRegister(PCReg);
    machine->WriteRegister(PrevPCReg, curPC);
    curPC = machine->ReadRegister(NextPCReg);
    machine->WriteRegister(PCReg, curPC);
    machine->WriteRegister(NextPCReg, curPC + 4);
}

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    switch(which)
    {
        case NoException:
            return;

        case SyscallException:
            switch(type)
            {
                case SC_Halt:
                    DEBUG('a', "Shutdown, initiated by user program.\n");
                    interrupt->Halt();
                    break;
                case SC_Create:
                {
                    int virtAddr;
                    char* filename;
                    DEBUG('a', "\n SC_Create call ...");
                    DEBUG('a', "\n Reading virtual address of filename");
                    // Lấy tham số tên tập tin từ thanh ghi r4
                    virtAddr = machine->ReadRegister(4);
                    DEBUG ('a', "\n Reading filename.");
                    // MaxFileLength là = 32
                    filename = User2System(virtAddr, MaxFileLength + 1);
                    if (filename == NULL) {
                        printf("\n Not enough memory in system");
                        DEBUG('a', "\n Not enough memory in system");
                        machine->WriteRegister(2,-1); // trả về lỗi cho chương
                        // trình người dùng
                        delete[] filename;
                        break;
                    }
                    DEBUG('a', "\n Finish reading filename.");
                    //DEBUG(‘a’,"\n File name : '"<<filename<<"'");
                    // Create file with size = 0
                    // Dùng đối tượng fileSystem của lớp OpenFile để tạo file,
                    // việc tạo file này là sử dụng các thủ tục tạo file của hệ điều
                    // hành Linux, chúng ta không quản ly trực tiếp các block trên
                    // đĩa cứng cấp phát cho file, việc quản ly các block của file
                    // trên ổ đĩa là một đồ án khác
                    if (!fileSystem->Create(filename, 0)) {
                        printf("\n Error create file '%s'", filename);
                        machine->WriteRegister(2, -1);
                        delete[] filename;
                        break;
                    }
                    machine->WriteRegister(2, 0); // trả về cho chương trình
                    // người dùng thành công
                    delete[] filename;
                    break;
                }
                case SC_Open:
                {
                    int virtAddr;
                    char* filename;
                    DEBUG('a', "\n SC_Open call ...");
                    DEBUG('a', "\n Reading virtual address of filename");
                    // Lấy tham số tên tập tin từ thanh ghi r4
                    virtAddr = machine->ReadRegister(4);
                    DEBUG ('a', "\n Reading filename.");
                    // MaxFileLength là = 32
                    filename = User2System(virtAddr, MaxFileLength + 1);
                    if (filename == NULL) {
                        printf("\n Not enough memory in system");
                        DEBUG('a', "\n Not enough memory in system");
                        machine->WriteRegister(2,-1); // trả về lỗi cho chương
                        // trình người dùng
                        delete[] filename;
                        break;
                    }
                    DEBUG('a', "\n Finish reading filename.");
                    // DEBUG('a', "\n File name : '" << filename <<"'");
                    // Mở file
                    // Dùng đối tượng fileSystem của lớp OpenFile để mở file,
                    // việc mở file này là sử dụng các thủ tục mở file của hệ điều
                    // hành Linux, chúng ta không quản ly trực tiếp các block trên
                    // đĩa cứng cấp phát cho file, việc quản ly các block của file
                    // trên ổ đĩa là một đồ án khác
                    OpenFile* file = fileSystem->Open(filename);
                    if (file == NULL) {
                        printf("\n Error open file '%s'", filename);
                        machine->WriteRegister(2, -1);
                        delete[] filename;
                        break;
                    }
                    // Trả về id của file
                    machine->WriteRegister(2, file->GetID());
                    delete file;
                    delete[] filename;
                    break;
                }
                case SC_Close:
                {
                    DEBUG('a', "\n SC_Close call ...");
                    DEBUG('a', "\n Reading file id");
                    // Lấy tham số id của file từ thanh ghi r4
                    int fid = machine->ReadRegister(4);
                    // Đóng file
                    DEBUG('a', "\n Point to file with id %d", fid);
                    // Dùng con trỏ của lớp OpenFile để trỏ đến file cần đóng
                    OpenFile* file = new OpenFile(fid);
                    if (file == NULL) {
                        printf("\n Error close file with id %d", fid);
                        machine->WriteRegister(2, -1);
                        delete file;
                        break;
                    }
                    DEBUG('a', "\n Closing file with id %d", fid);
                    delete file;
                    // Trả về 0 nếu đóng file thành công
                    machine->WriteRegister(2, 0);
                    
                    break;
                }
                case SC_Read:
                {
                    DEBUG('a', "\n SC_Write call ...");
                    DEBUG('a', "\n Reading virtual address of filename");
                    int addr = machine->ReadRegister(4); // Lấy địa chỉ lưu bộ đệm
                    DEBUG ('a', "\n Reading size of buffer.");
                    int size = machine->ReadRegister(5); // Lấy kích cỡ bộ đệm
                    DEBUG ('a', "\n Reading file ID.");
                    int fID = machine->ReadRegister(6); // Lấy tham số id của file

                    OpenFile* file = new OpenFile(fID); // Tạo biến con trỏ đọc file
                    if (file == NULL) {
                        printf("\n Error writing file with id %d", fID);
                        machine->WriteRegister(2, -1);
                        delete file;
                        break;
                    }

                    char *buffer = new char [size + 1]; // Khai báo một chuỗi có độ dài là kích thước tối đa + 1
                    int len = file->Read(buffer, size); // Lấy số kí tự trong chuỗi đọc ra
                    buffer[len] = '\0'; // Đặt kí tự kết thúc chuỗi

                    // Kiểm tra kết thúc file
                    bool isEOF = false;
                    for (int i = 0; i < len; i++) {
                        if (buffer[i] == EOF) {
                            len = i + 1;
                            buffer[i] = '\0';
                            isEOF = true;
                            break;
                        }
                    }

                    System2User(addr, len + 1, buffer);

                    if (isEOF) machine->WriteRegister(2, -2); // Gặp điểm kết thúc file
                    else machine->WriteRegister(2, len); // Không gặp điểm kết thúc file thì in ra số kí tự đọc được

                    delete file;
                    delete[] buffer;
                    
                    break;
                }
                case SC_Write:
                {
                    DEBUG('a', "\n SC_Write call ...");
                    DEBUG('a', "\n Reading virtual address of filename");
                    int virtAddr = machine->ReadRegister(4); // Lấy địa chỉ lưu bộ đệm
                    DEBUG ('a', "\n Reading size of buffer.");
                    int size = machine->ReadRegister(5); // Lấy kích cỡ bộ đệm
                    DEBUG ('a', "\n Reading file ID.");
                    int fID = machine->ReadRegister(6); // Lấy tham số id của file

                    char *buffer = User2System(virtAddr, MaxString); // chuyển dữ liệu bộ đệm từ User space sang Kernel space

                    OpenFile* file = new OpenFile(fID); // Tạo biến con trỏ đọc file
                    if (file == NULL) {
                        printf("\n Error writing file with id %d", fID);
                        machine->WriteRegister(2, -1);
                        delete file;
                        break;
                    }

                    int len = file->Write(buffer, size); // Ghi buffer vào file
                    if (len != size - 1) machine->WriteRegister(2, -2);
                    else machine->WriteRegister(2, len);
                    delete file;
                    delete[] buffer;
                    break;
                }
                case SC_ReadInt:
                {
                    char *buffer = new char[12];
                    int number = 0;
                    short multiplier = 1;
                    DEBUG('a', "\n SC_ReadInt call ...");
                    DEBUG('a', "\n Synching number from console ...");
                    // Đọc tối đa 11 kí tự
                    int numOfChars = gSynchConsole->Read(buffer, 11);

                    // Nếu có kí tự đọc được
                    if (numOfChars > 0) {
                        // Chuyển kí tự thành số
                        for (int i = 0; i < numOfChars; i++) {
                            // Nếu gặp dấu ở vị trí đầu tiên
                            if (i == 0 && buffer[i] == '-') {
                                multiplier = -1;
                                continue;
                            }
                            else if (i == 0 && buffer[i] == '+') {
                                continue;
                            }
                            else if (i == 0 && numOfChars == 11) numOfChars = 10;

                            if (buffer[i] < 48 || buffer[i] > 57) {
                                number = 0;
                                printf("\n Invalid number");
                                DEBUG('a', "\n Invalid number");
                                break;
                            }
                            else {
                                number = number * 10 + (buffer[i] - 48);
                            }
                        }

                        // Ghi kết quả vào thanh ghi r2
                        number *= multiplier;
                        machine->WriteRegister(2, number);
                    }
                    else {
                        // Ghi 0 vào thanh ghi r2
                        printf("\n No number read!");
                        DEBUG('a', "\n No number read!");
                        machine->WriteRegister(2, 0);
                    }

                    delete buffer;
                    break;
                }
                case SC_PrintInt:
                {
                    char* buffer = new char[12];
                    int printPos = 11;
                    bool isNeg = false;
                    DEBUG('a', "\n SC_PrintInt call ...");
                    DEBUG('a', "\n Reading number");
                    // Đọc số nguyên từ thanh ghi r4
                    int number = machine->ReadRegister(4);
                    if (number < 0) {
                        isNeg = true;
                        number = -number;
                    }

                    // Chuyển số nguyên thành chuỗi
                    for (int i = 0; i < 12; i++) {
                        if (number == 0) {
                            buffer[11-i] = 48;
                            break;
                        }
                        else {
                            buffer[11-i] = number % 10 + 48;
                            number /= 10;
                            printPos = 11 - i; // Lưu lại vị trí đọc
                        }
                    }

                    if (isNeg) buffer[printPos--] = '-';

                    // In chuỗi ra màn hình bắt đầu từ vị trí đọc
                    gSynchConsole->Write(buffer + printPos, 12 - printPos);

                    delete[] buffer;
                    break;
                }
                case SC_ReadFloat:
                {
                    char *buffer = new char[41];
                    bool dotEncountered = false;
                    int decimalPart = 0;
                    int maxDecimalPart = 7;
                    short multiplier = 1;
                    float number = 0;
                    DEBUG('a', "\n SC_ReadFloat call ...");

                    DEBUG('a', "\n Synching number from console ...");
                    // Đọc tối đa 40 kí tự
                    int numOfChars = gSynchConsole->Read(buffer, 40);

                    if (numOfChars == 40 && buffer[0] > 3 && buffer[1] > 4 && buffer[2] > 0) {
                        printf("\n Invalid number");
                        DEBUG('a', "\n Invalid number");
                        machine->WriteRegister(2, 0);
                    }
                    
                    // Nếu có kí tự đọc được
                    if (numOfChars > 0) {
                        for (int i = 0; i < numOfChars; i++) {
                            // Nếu gặp dấu chấm
                            if (buffer[i] == '.') {
                                dotEncountered = true;
                                continue;
                            }

                            // Nếu gặp dấu ở vị trí đầu tiên
                            if (i == 0 && buffer[i] == '-') {
                                multiplier = -1;
                                continue;
                            }
                            else if (i == 0 && buffer[i] == '+') {
                                continue;
                            }
                            else if (i == 0 && numOfChars == 40) numOfChars = 39;

                            // Nếu đã đọc đủ phần thập phân
                            if (maxDecimalPart == 0) {
                                break;
                            }

                            if (buffer[i] < 48 || buffer[i] > 57) {
                                number = 0;
                                printf("\n Invalid number");
                                DEBUG('a', "\n Invalid number");
                                break;
                            }
                            else {
                                if (dotEncountered) {
                                    decimalPart = decimalPart * 10 + (buffer[i] - 48);
                                    maxDecimalPart--;
                                }
                                else {
                                    number = number * 10 + (buffer[i] - 48);
                                }
                            }
                        }

                        // Chuyển phần thập phân thành số thực
                        for (int i = 0; i < maxDecimalPart; i++) {
                            decimalPart *= 10;
                        }
                        number += (float)decimalPart / 10000000;

                        // Ghi kết quả vào thanh ghi r2
                        number *= multiplier;
                        machine->WriteRegister(2, *(int*)&number);
                    }
                    else {
                        // Ghi 0 vào thanh ghi r2
                        printf("\n No number read!");
                        DEBUG('a', "\n No number read!");
                        machine->WriteRegister(2, 0);
                    }

                    delete[] buffer;
                    break;
                }
                case SC_PrintFloat:
                {
                    char* buffer = new char[41];
                    int printPos = 40;
                    int endPos = 33;
                    // Đọc số thực từ thanh ghi r4
                    int tmpI = machine->ReadRegister(4);
                    float number = *((float*)&tmpI);
                    int decimalPart = 0;
                    
                    // Lấy phần thập phân
                    while (true) {
                        float temp = number;
                        if (temp > 3.4e+2) {
                            temp -= 3.4e+8;
                        }
                        else if (temp < -3.4e+2) {
                            temp += 3.4e+3;
                        }
                        else {
                            decimalPart = (int)(temp * 10000000) % 10000000;
                            break;
                        }
                    }

                    number -= (float)decimalPart / 10000000;

                    // Chuyển phần thập phân thành chuỗi
                    for (int i = 0; i < 7; i++) {
                        if (decimalPart == 0) {
                            endPos += i;
                            break;
                        }

                        buffer[endPos-i-1] = decimalPart % 10 + 48;
                        decimalPart /= 10;
                    }

                    // Chuyển phần nguyên thành chuỗi
                    for (int i = 0; i < endPos; i++) {
                        if (number == 0) {
                            buffer[endPos-i-1] = 48;
                            break;
                        }
                        else {
                            buffer[endPos-i-1] = (int)number % 10 + 48;
                            number /= 10;
                            printPos = endPos-i-1; // Lưu lại vị trí đọc
                        }
                    }

                    // In chuỗi ra màn hình bắt đầu từ vị trí đọc
                    gSynchConsole->Write(buffer + printPos, endPos - printPos);

                    delete[] buffer;
                    break;
                }
                case SC_ReadChar:
                {
                    char *buffer = new char [MaxString];
                    int size = gSynchConsole->Read(buffer, MaxString); // Đọc 1 kí tự do người dùng nhập

                    if (size == 0) {
                        printf("\n Invalid input: The input must not be null");
                        DEBUG('a', "\n Invalid input: The input must not be null");
                        machine->WriteRegister(2, 0);
                    } else if (size > 1) {
                        printf("\n Invalid input: The input must be one character");
                        DEBUG('a', "\n Invalid input: The input must be one character");
                        machine->WriteRegister(2, 0);                        
                    } else 
                        machine->WriteRegister(2, *buffer);

                    delete []buffer;
                    break;
                }
                case SC_PrintChar:
                {
                    char c = (char)machine->ReadRegister(4); // Lấy kí tự vào biến c
                    gSynchConsole->Write(&c, 1);
                    break;
                }
                case SC_ReadString:
                {
                    int addr = machine->ReadRegister(4); // Lấy địa chỉ buffer
                    int len = machine->ReadRegister(5); // Lấy kích thước là tham số truyền vào hàm

                    char *buffer = new char [len + 1]; // Khai báo một chuỗi có độ dài là kích thước tối đa + 1
                    int size = gSynchConsole->Read(buffer, len); // Lấy số kí tự trong chuỗi nhập vào
                    buffer[size] = '\0'; // Đặt kí tự kết thúc chuỗi

                    System2User(addr, size + 1, buffer);
                    machine->WriteRegister(2, size);

                    delete []buffer;
                    break;
                }
                case SC_PrintString:
                {
                    int addr = machine->ReadRegister(4);
                    char *buffer = User2System(addr, MaxString); // chuyển dữ liệu từ User space sang Kernel space
                    int index = 0, count = 0;
                    while (buffer[index] != '\0' && index < MaxString) {
                        count += gSynchConsole->Write(buffer + index++, 1); // In một kí tự ở vị trí thứ i trong chuỗi
                    }

                    machine->WriteRegister(2, count); // Ghi số kí tự in được vào thanh ghi r2
                    delete []buffer;
                    break;
                }
                default:
                    printf("\n Unexpected user mode exception (%d %d) \n", which, type);
                    interrupt->Halt();
                    break;
            }
            IncPCReg(); // Tăng thanh ghi PC
            break;

        case PageFaultException:
            printf("\n PageFaultException");
            interrupt->Halt();
            break;

        case ReadOnlyException:
            printf("\n ReadOnlyException");
            interrupt->Halt();
            break;

        case BusErrorException:
            printf("\n BusErrorException");
            interrupt->Halt();
            break;

        case AddressErrorException:
            printf("\n AddressErrorException");
            interrupt->Halt();
            break;

        case OverflowException:
            printf("\n OverflowException");
            interrupt->Halt();
            break;

        case IllegalInstrException:
            printf("\n IllegalInstrException");
            interrupt->Halt();
            break;

        case NumExceptionTypes:
            printf("\n NumExceptionTypes");
            interrupt->Halt();
            break;

        default:
            printf("\n Unexpected user mode exception (%d %d)\n", which, type);
            interrupt->Halt();
            break;
    }
}

#undef MaxFileLength
#undef MaxString