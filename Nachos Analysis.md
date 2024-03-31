# progtest.cc

## Thông tin
Chứa các thủ tục để kiểm tra xem Nachos có thể chạy được user program hay không. Ngoài ra, các thủ tục này còn để kiểm tra các thiết bị phần cứng (bao gồm thiết bị nhập xuất).

## Các hàm và biến
- `void StartProcess(char* filename)`: tải user program lên bộ nhớ chính và nhảy đến đó.
- `static Console *console`, `static Semaphore *readAvail`, `static Semaphore *writeDone`: các biến dùng để kiểm tra console.
- `ConsoleInterruptHandlers`: gọi các tiến trình yêu cầu nhập xuất.
- `void ConsoleTest(char* in, char* out)`: kiểm tra console bằng cách lấy các ký tự từ quá trình nhập qua quá trình xuất; sẽ dừng lại khi người dùng nhập ký tự 'q'.

# syscall.h

## Thông tin
Chứa các thủ tục ở kernel, có thể được người dùng gọi thông qua lệnh "syscall".
Tập tin này phải được include ở cả user program và chương trình hệ thống (kernel).

## Mã thủ tục
Dưới đây là các mã thủ tục tương ứng để kernel nhận diện được. Nếu người lập trình muốn thêm một thủ tục thì bổ sung ở đây.

```cpp
#define SC_Halt		    0
#define SC_Exit		    1
#define SC_Exec		    2
#define SC_Join		    3
#define SC_Create	    4
#define SC_Open		    5
#define SC_Read		    6
#define SC_Write	    7
#define SC_Close	    8
#define SC_Fork		    9
#define SC_Yield	    10
#define SC_ReadInt	    11
#define SC_PrintInt     12
#define SC_ReadFloat	13
#define SC_PrintFloat	14
#define SC_ReadChar 	15
#define SC_PrintChar	16
#define SC_ReadString   17
#define SC_PrintString  18
```
## Các hàm của kernel
Các thủ tục dưới đây có thể được người dùng gọi trực tiếp.

- `void Halt()`: dừng Nachos
- `void Exit(int status)`: user program đã hoàn thành (status = 0 nghĩa là chương trình thoát bình thường)
- `SpaceId Exec(char* name)`: chạy chương trình thực thi được lưu trong Nachos và trả về địa chỉ vùng không gian mà chương trình thực thi chiếm giữ.
- `int Join(SpaceID id)`: trả ra exit status sau khi chương trình thực thi có mã nhận diện là `id` đã hoàn thành.

## Các hàm tương tác file
Các thủ tục tương tác file bao gồm: tạo, mở, đọc, viết và đóng.

Khi một chương trình yêu cầu thiết bị nhập xuất, nó sẽ có 2 tập tin: nhập từ bàn phím và xuất ra màn hình (hay nói cách khác là `stdin` và `stdout`). Có thể đọc và ghi thẳng trên các tập tin này mà không cần mở thiết bị console.

`typedef int OpenFileId`: mã nhận diện đặc biệt để phân biệt tập tin Nachos.

Định nghĩa nhập và xuất trên console:
```cpp
#define ConsoleInput	0  
#define ConsoleOutput	1  
```

Các hàm tương tác tập tin và console
- `int Create(char* name)`: tạo tập tin mới
- `OpenFileId Open(char* name)`: mở tập tin Nachos và trả ra `OpenFileId` có thể được dùng cho việc nhập xuất
- `void Write(char *buffer, int size, OpenFileId id)`: ghi nội dung từ buffer vào tập tin với kích thước là `size`
- `int Read(char *buffer, int size, OpenFileId id)`: đọc nội dung từ tập tin vào buffer với kích thước là `size`
- `void Close(OpenFileId id)`: đóng tập tin với mã nhận diện là `id`
- `int ReadInt()`: đọc số nguyên từ console
- `void PrintInt(int number)`: in số nguyên ra console
- `float ReadFloat()`: đọc số thập phân từ console
- `void PrintFloat(float number)`: in số thập phân ra console
- `char ReadChar()`: đọc ký tự từ console
- `void PrintChar(char character)`: ghi ký tự ra console
- `void ReadString(char *buffer, int length)`: đọc string từ console
- `void PrintString(char *buffer)`: ghi string ra console

# exception.cc

## Thông tin

Các hàm chuyển giao user program về cho kernel của Nachos. Có hai tình huống dẫn đến việc chuyển giao:

- user program gọi syscall
- user program có lỗi khiến cho CPU không thể thực hiện tác vụ

## Các hàm
- `char* User2System(int virtAddr, int limit)`: sao chép string từ vùng nhớ của người dùng qua vùng nhớ của hệ thống
- `int System2User(int virtAddr, int len, char* buffer)`: sao chép string từ vùng nhớ của hệ thống qua vùng nhớ của người dùng
- `void ExceptionHandler(ExceptionType which)`: xử lý các exception, bao gồm syscall hoặc exception do lỗi từ user program

# bitmap.h và bitmap.cc
> Chưa hoàn thiện

# openfile.h
> Chưa hoàn thiện

# translate.h và translate.cc
> Chưa hoàn thiện
## Thông tin

Hệ thống quản lý việc dịch từ trang ảo (virtual page) qua trang vật lý (physical page). `TranslationEntry` có hai vai trò: vừa là bảng trang, vừa là TLB.

# machine.h và machine.cc

## Thông tin
Mô phỏng lại các thành phần của Nachos (RAM, register...). Vì Nachos là hệ điều hành đơn chương (ở thời điểm hiện tại), nên chỉ có một instruction được thực thi tại một thời điểm.

## Các lớp trong machine.h
- `Instruction`: câu lệnh của MIPS
- `Machine`: mô phỏng lại các thành phần của máy tính

## Các hàm trong machine.cc
- `Machine::Machine(bool debug)`: constructor để tạo ra một "máy tính" mới; `debug = true` nếu muốn sử dụng debugger sau mỗi instruction
- `void Machine::RaiseException(ExceptionType which, int badVAddr)`: đưa quyền điều khiển cho kernel khi có system call hoặc có exception;
- `void Machine::Debugger()`: debugger dành riêng cho user program
- `void Machine::DumpState()`: in ra trạng thái CPU khi thực thi user program
- `int Machine::ReadRegister(int num)`: đọc thanh ghi của user program
- `void Machine::WriteRegister(int num, int value)`: ghi lên thanh ghi của user program

# mipssim.cc

## Thông tin

Mô phỏng lại CPU MIPS R2/3000.

## Các hàm
- `void Machine::run()`: mô phỏng quá trình thực thi của user program trên Nachos; được gọi bởi kernel khi user program khởi động.
- `static int TypeToReg(RegType reg, Instruction *instr)`: lấy giá trị thanh ghi trong một instruction.
- `void Machine::OneInstruction(Instruction *instr)`: thực thi các instruction từ user program. 
- `void Machine::DelayedLoad(int nextReg, int nextValue)`
- `void Instruction::Decode()`: giải mã MIPS instruction
- `static void Mult(int a, int b, bool signedArith, int* hiPtr, int* loPtr)`: thực hiện phép nhân trên R2000.

# console.h và console.cc
> Chưa hoàn thiện
## Thông tin

Mô phỏng lại thiết bị nhập xuất qua terminal, gồm hai phần chính: nhập qua bàn phím và xuất ra màn hình.

# synchcons.h và synchcons.cc
> Chưa hoàn thiện