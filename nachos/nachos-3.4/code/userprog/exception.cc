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

// Utility functions for syscall of float
int fromFToI(float* num) {
    void *ptr = (void*)num;
    int tmp = (int)ptr;
    return tmp;
}

// char* fromIToC(int num) {
//     char *buffer = new char[5];
//     for (int i = 0; i < 4; i++) {
//         buffer[i] = *reinterpret_cast<char*>(&num);
//         num >>= 8;
//     }
//     buffer[4] = '\0';
//     return buffer;
// }

// char* fromFToC(float* num) {
//     return fromIToC(*reinterpret_cast<int*>(num));
// }

// int fromCToI(char* buffer) {
//     int num = 0;
//     for (int i = 0; i < 4; i++) {
//         num += buffer[i] << (i * 8);
//     }
//     return num;
// }

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
    for (i = 0; i < limit; i++) {
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
    do {
        oneChar = (int) buffer[i];
        machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < len && oneChar != 0);
    return i;
}

void IncPCReg() {
    // // Advance program counters.
    // registers[PrevPCReg] = registers[PCReg];	// for debugging, in case we
	// 					// are jumping into lala-land
    // registers[PCReg] = registers[NextPCReg];
    // registers[NextPCReg] = pcAfter;

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
                    int virtAddr, type;
                    char* filename;
                    DEBUG('a', "\n SC_Open call ...");
                    DEBUG('a', "\n Reading virtual address of filename");
                    // Lấy tham số tên tập tin từ thanh ghi r4
                    virtAddr = machine->ReadRegister(4);
                    DEBUG('a', "\n Reading type of file");
                    // Lấy tham số type của file từ thanh ghi r5
                    type = machine->ReadRegister(5);
                    if (type != 0 && type != 1) {
                        printf("\n Invalid type of file");
                        DEBUG('a', "\n Invalid type of file");
                        machine->WriteRegister(2, -1);
                        break;
                    }
                    DEBUG ('a', "\n Reading filename.");
                    // MaxFileLength là = 32
                    filename = User2System(virtAddr, MaxFileLength + 1);
                    if (filename == NULL) {
                        printf("\n Not enough memory in system");
                        DEBUG('a', "\n Not enough memory in system");
                        machine->WriteRegister(2, -1); // trả về lỗi cho chương
                        // trình người dùng
                        delete[] filename;
                        break;
                    }
                    DEBUG('a', "\n Finish reading filename.");
                    // DEBUG('a', "\n File name : '" << filename <<"'");
                    
                    // Mở file
                    // Kiểm tra xem có chỗ trống trong bảng file không
                    int freeBlock = fileSystem->FindFreeBlock();
                    if (freeBlock == -1) {
                        printf("\n No free block in file system");
                        DEBUG('a', "\n No free block in file system");
                        machine->WriteRegister(2, -1);
                        delete[] filename;
                        break;
                    }
                    else {
                        if (type == 0 || type == 1) { // Đọc ghi hoặc chỉ đọc
                            OpenFile* file = fileSystem->Open(filename, type);
                            if (file == NULL) {
                                printf("\n Error open file '%s'", filename);
                                machine->WriteRegister(2, -1);
                                delete[] filename;
                                break;
                            }
                            fileSystem->openFileList[freeBlock] = file;
                            machine->WriteRegister(2, freeBlock);
                        }
                        else if (type == 2) { // stdin
                            machine->WriteRegister(2, 0);
                        }
                        else {
                            machine->WriteRegister(2, 1);
                        }

                        delete[] filename;
                        break;
                    }
                }
                case SC_Close:
                {
                    DEBUG('a', "\n SC_Close call ...");
                    DEBUG('a', "\n Reading file id");
                    // Lấy tham số id của file từ thanh ghi r4
                    int fid = machine->ReadRegister(4);
                    // Đóng file
                    if (!fileSystem->Close(fid)) {
                        printf("\n Error close file with id %d", fid);
                        machine->WriteRegister(2, -1);
                        break;
                    }
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

                    if (fID < 2 || fID > 9) {
                        printf("\n Invalid file id");
                        machine->WriteRegister(2, -1);
                        break;
                    }

                    OpenFile* file = fileSystem->openFileList[fID]; // Tạo biến con trỏ đọc file
                    if (file == NULL) {
                        printf("\n Error reading file with id %d", fID);
                        machine->WriteRegister(2, -1);
                        break;
                    }

                    char *buffer = new char [size + 1]; // Khai báo một chuỗi có độ dài là kích thước tối đa + 1
                    int len = file->Read(buffer, size); // Lấy số kí tự trong chuỗi đọc ra
                    buffer[len] = '\0'; // Đặt kí tự kết thúc chuỗi

                    // Kiểm tra kết thúc file hoặc kết thúc chuỗi
                    bool isEOF = false;
                    for (int i = 0; i < len; i++) {
                        if (buffer[i] == '\0') {
                            len = i + 1;
                            break;
                        } else if (buffer[i] == EOF) {
                            len = i + 1;
                            buffer[i] = '\0';
                            isEOF = true;
                            break;
                        }
                    }

                    System2User(addr, len + 1, buffer); // Chuyển dữ liệu từ Kernel sang User space

                    if (isEOF) machine->WriteRegister(2, -2); // Gặp điểm kết thúc file
                    else machine->WriteRegister(2, len); // Không gặp điểm kết thúc file thì in ra số kí tự đọc được

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

                    if (fID < 2 || fID > 9) {
                        printf("\n Invalid file id");
                        machine->WriteRegister(2, -1);
                        break;
                    }

                    char *buffer = User2System(virtAddr, MaxString); // chuyển dữ liệu bộ đệm từ User space sang Kernel space

                    OpenFile* file = fileSystem->openFileList[fID]; // Tạo biến con trỏ đọc file
                    if (file == NULL) {
                        printf("\n Error writing file with id %d", fID);
                        DEBUG('a', "\n Error writing file with id %d", fID);
                        machine->WriteRegister(2, -1);
                        break;
                    }

                    if (file->GetType() == 1) { // File chỉ đọc 
                        printf("\n This is a read-only file, can not write!");
                        DEBUG('a', "\n This is a read-only file, can not write!");
                        machine->WriteRegister(2, -1);
                        break;
                    }

                    int len = file->Write(buffer, size); // Ghi buffer vào file
                    if (len != size - 1) machine->WriteRegister(2, -2);
                    else machine->WriteRegister(2, len);
                    delete[] buffer;
                    break;
                }
                case SC_WriteF2File:
                {
                    DEBUG('a', "\n SC_Write call ...");
                    DEBUG('a', "\n Reading virtual address of filename");
                    float* number = (float*)machine->ReadRegister(4); // Lấy số thực từ thanh ghi r4
                    DEBUG ('a', "\n Reading file ID.");
                    int fID = machine->ReadRegister(5); // Lấy tham số id của file

                    if (fID < 2 || fID > 9) {
                        printf("\n Invalid file id");
                        machine->WriteRegister(2, -1);
                        break;
                    }

                    OpenFile* file = fileSystem->openFileList[fID]; // Tạo biến con trỏ đọc file
                    if (file == NULL) {
                        printf("\n Error writing file with id %d", fID);
                        DEBUG('a', "\n Error writing file with id %d", fID);
                        machine->WriteRegister(2, -1);
                        break;
                    }

                    if (file->GetType() != 0) { // File chỉ đọc 
                        printf("\n This is not a read and write file, can not write!");
                        DEBUG('a', "\n This is not a read and write file, can not write!");
                        machine->WriteRegister(2, -1);
                        break;
                    }

                    char *buffer = new char[MaxString];
                    int length = sprintf(buffer, "%f", *number); // Chuyển số thực thành chuỗi
                    int count = file->Write(buffer, length); // Ghi buffer vào file

                    if (count != length - 1) machine->WriteRegister(2, -2);
                    else machine->WriteRegister(2, count);
                    delete[] buffer;
                    break;
                }
                case SC_CompareFloat:
                {
                    DEBUG('a', "\n SC_CompareFloat call ...");
                    DEBUG('a', "\n Reading address of float number");
                    float* first = (float*)machine->ReadRegister(4);
                    float* second = (float*)machine->ReadRegister(5);

                    if (*first > *second) machine->WriteRegister(2, 1);
                    else if (*first == *second) machine->WriteRegister(2, 0);
                    else machine->WriteRegister(2, -1);

                    break;
                }
                case SC_ClearFloat:
                {
                    DEBUG('a', "\n SC_ClearFloat call ...");
                    DEBUG('a', "\n Reading address of float number");
                    float* number = (float*)machine->ReadRegister(4);
                    if (number != NULL) {
                        delete number;
                    }
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

                    delete[] buffer;
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
                    char *buffer = new char[MaxString];
                    float* number = new float;
                    int res;

                    DEBUG('a', "\n SC_ReadFloat call ...");
                    DEBUG('a', "\n Reading buffer from console ...");

                    DEBUG('a', "\n Synching number from console ...");
                    // Đọc tối đa 40 kí tự
                    int numOfChars = gSynchConsole->Read(buffer, 40);
                    
                    // Nếu có kí tự đọc được
                    if (numOfChars > 0) {
                        *number = atof(buffer);
                    }
                    else {
                        // Ghi 0 vào thanh ghi r2
                        printf("\n No number read!");
                        DEBUG('a', "\n No number read!");
                        *number = 0;;
                    }
                    res = fromFToI(number);

                    machine->WriteRegister(2, res);

                    delete[] buffer;
                    break;
                }
                case SC_PrintFloat:
                {
                    // Đọc số thực từ thanh ghi r4
                    float* number = (float*)machine->ReadRegister(4);
                    char *buffer = new char[MaxString];
                    int length = sprintf(buffer, "%f", *number);

                    // In chuỗi ra màn hình bắt đầu từ vị trí đọc
                    int count = gSynchConsole->Write(buffer, length);
                    machine->WriteRegister(2, count);

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