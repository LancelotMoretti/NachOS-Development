# Hiểu Nachos

## progtest.cc

### Thông tin
Chứa các thủ tục để kiểm tra xem Nachos có thể chạy được user program hay không. Ngoài ra, các thủ tục này còn để kiểm tra các thiết bị phần cứng (bao gồm thiết bị nhập xuất).

### Các hàm và biến
- `void StartProcess(char* filename)`: tải user program lên bộ nhớ chính và nhảy đến đó.
- `static Console *console`, `static Semaphore *readAvail`, `static Semaphore *writeDone`: các biến dùng để kiểm tra console.
- `ConsoleInterruptHandlers`: gọi các tiến trình yêu cầu nhập xuất.
- `void ConsoleTest(char* in, char* out)`: kiểm tra console bằng cách lấy các ký tự từ quá trình nhập qua quá trình xuất; sẽ dừng lại khi người dùng nhập ký tự 'q'.

## syscall.h

### Thông tin
Chứa các thủ tục ở kernel, có thể được người dùng gọi thông qua lệnh "syscall".
Tập tin này phải được include ở cả user program và chương trình hệ thống (kernel).

### Mã thủ tục
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
### Các hàm của kernel
Các thủ tục dưới đây có thể được người dùng gọi trực tiếp.

- `void Halt()`: dừng Nachos
- `void Exit(int status)`: user program đã hoàn thành (status = 0 nghĩa là chương trình thoát bình thường)
- `SpaceId Exec(char* name)`: chạy chương trình thực thi được lưu trong Nachos và trả về địa chỉ vùng không gian mà chương trình thực thi chiếm giữ.
- `int Join(SpaceID id)`: trả ra exit status sau khi chương trình thực thi có mã nhận diện là `id` đã hoàn thành.

### Các hàm tương tác file
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

## exception.cc

### Thông tin

Các hàm chuyển giao user program về cho kernel của Nachos. Có hai tình huống dẫn đến việc chuyển giao: user program gọi syscall hoặc có lỗi khiến cho CPU không thể thực hiện tác vụ.

### Các hàm
- `char* User2System(int virtAddr, int limit)`: sao chép string từ vùng nhớ của người dùng qua vùng nhớ của hệ thống
- `int System2User(int virtAddr, int len, char* buffer)`: sao chép string từ vùng nhớ của hệ thống qua vùng nhớ của người dùng
- `void ExceptionHandler(ExceptionType which)`: xử lý các exception, bao gồm syscall hoặc exception do lỗi từ user program

## bitmap.h và bitmap.cc

### Thông tin

Quản lý bitmap - có nhiệm vụ lưu vết các ô nhớ vật lý. Các bitmap được lưu dưới dạng một số nguyên không dấu, và có thể thực hiện các thao tác trên bit để lấy được thông tin cần thiết.

### bitmap.h

Lớp `BitMap` được định nghĩa là một mảng các bit. Thường được sử dụng để quản lý việc cấp phát cho các thành phần, chẳng hạn như các sector hoặc các trang trên bộ nhớ chính. Giá trị của từng bit cho biết sector hoặc trang tương ứng có đang free (tương ứng giá trị `0`) hay không (tương ứng giá trị `1`).

### bitmap.cc

 Chứa các hàm quản lý bitmap:
 - `BitMap::BitMap(int nitems)`: khởi tạo một bitmap với số bit là `nitems`
 - `void BitMap::Mark(int which)`: cài giá trị `1` cho bit ở vị trí `which`
 - `void BitMap::Clear(int which)`: cài giá trị `0` cho bit ở vị trí `which`
 - `bool BitMap::Test(int which)`: kiểm tra giá trị ở bit `which`, trả về true nếu giá trị là `1`
 - `int BitMap::Find()`: tìm bit đầu tiên có giá trị `0`, sau đó gán chúng bằng `1`
 - `int BitMap::NumClear()`: đếm số bit có giá trị `0`
 - `void BitMap::Print()`: in toàn bộ bitmap ra, thường dùng cho debug
 - `void BitMap::FetchFrom(OpenFile *file)`: đọc bitmap từ một Nachos file
 - `void BitMap::WriteBack(OpenFile *file)`: ghi bitmap lên một Nachos file

## openfile.h

### Thông tin
Thực hiện các thao tác mở, đóng, đọc và ghi lên từng file. Chúng tương tự với các thao tác trên UNIX. 

Có hai cách cài đặt: một loại là "STUB", có chức năng thay đổi các thao tác trên file thành các thao tác trên UNIX; loại thứ hai là các thao tác đọc ghi trên các sector. Cả hai cách cài đặt đều gần tương tự nhau trong lớp `OpenFile`.

### Lớp `OpenFile`

Các phương thức của lớp:
- `OpenFile(int sector)`: constructor mở file ở `sector`
- `OpenFile(int sector)`: constructor mở file ở `sector` có dạng `type`
- `void Seek(int position)`: đặt con trỏ file ở vị trí tương ứng
- `int ReadAt(char *into, int numBytes, int position)`: đọc một phần của file bắt đầu từ vị trí `position` và lưu vào vị trí `into` trỏ đến, trả về số lượng byte đã đọc
- `int Read(char *into, int numBytes)`: giống như phương thức `ReadAt` nhưng kèm theo việc tăng vị trí con trỏ đọc trên file
- `int Write(char *from, int numBytes)`: ghi một phần của file bắt đầu từ vị trí `position` với nội dung được lấy từ vị trí mà `from` trỏ đến, trả về số lượng byte đã ghi
- `int WriteAt(char *from, int numBytes, int position)`: giống như phương thức `WriteAt` nhưng kèm theo việc tăng vị trí con trỏ ghi trên file
- `int Length()`: trả về kích thước của file (đơn vị: byte)

## translate.h và translate.cc

### Thông tin

Hệ thống quản lý việc ánh xạ từ trang ảo (virtual page) qua trang vật lý (physical page). `TranslationEntry` có hai vai trò: vừa là bảng trang, vừa là bộ đệm tìm trang (TLB).

### translate.h

Chứa lớp `TranslationEntry`, là một entry trong cả bảng trang và TLB. Mỗi entry là một ánh xạ giữa trang ảo và trang vật lý.

Các thuộc tính của `TranslationEntry`:
- `int virtualPage`: trang ảo
- `int physicalPage`: trang vật lý
- `bool valid`: mang giá trị `true` khi tồn tại trên bảng trang
- `bool readOnly`: mang giá trị `true` khi chỉ được phép đọc
- `bool use`: mang giá trị `true` khi trang này được đọc hoặc ghi (dùng để chọn trang nạn nhân khi thay trang)
- `bool dirty`: mang giá trị `true` khi trang này có sự thay đổi và phải được cập nhật lại khi đem ra bộ nhớ phụ 

### translate.cc

Chứa các hàm để ánh xạ từ trang ảo ra trang vật lý. Có hai dạng ánh xạ: bảng trang tuyến tính (linear page table) và TLB.

Các hàm trong translate.cc:
- `unsigned int WordToHost(unsigned int word)` và `unsigned short ShortToHost(unsigned short shortword)`: dịch từ word và shortword sang dạng little endian (và ngược lại).
- `bool Machine::ReadMem(int addr, int size, int *value)`: đọc một số lượng `size` các byte từ bộ nhớ ảo tại địa chỉ `addr` và lưu vào `value`. Trả về `false` nếu dịch từ địa chỉ ảo qua địa chỉ vật lý không thành công.
- `bool Machine::WriteMem(int addr, int size, int value)`: ghi một số lượng `size` các byte nằm trong `value` lên bộ nhớ ảo tại địa chỉ `addr`. Trả về `false` nếu dịch từ địa chỉ ảo qua địa chỉ vật lý không thành công.
- `ExceptionType Machine::Translate(int virtAddr, int* physAddr, int size, bool writing)`: dịch từ địa chỉ ảo qua địa chỉ vật lý thông qua bảng trang hoặc TLB. Sau khi kiểm tra và nhận thấy không có lỗi thì gán `true` cho bit `use` hoặc `dirty` và lưu địa chỉ vật lý trong `physAddr`. Nếu có lỗi thì trả ra exception tương ứng. Nếu `writing = true` thì kiểm tra trạng thái `readOnly` trong entry. 

## machine.h và machine.cc

### Thông tin
Mô phỏng lại các thành phần của Nachos (RAM, register...). Vì Nachos là hệ điều hành đơn chương (ở thời điểm hiện tại), nên chỉ có một instruction được thực thi tại một thời điểm.

### machine.h
- `Instruction`: câu lệnh của MIPS
- `Machine`: mô phỏng lại các thành phần của máy tính

### machine.cc
- `Machine::Machine(bool debug)`: constructor để tạo ra một "máy tính" mới; `debug = true` nếu muốn sử dụng debugger sau mỗi instruction
- `void Machine::RaiseException(ExceptionType which, int badVAddr)`: đưa quyền điều khiển cho kernel khi có system call hoặc có exception;
- `void Machine::Debugger()`: debugger dành riêng cho user program
- `void Machine::DumpState()`: in ra trạng thái CPU khi thực thi user program
- `int Machine::ReadRegister(int num)`: đọc thanh ghi của user program
- `void Machine::WriteRegister(int num, int value)`: ghi lên thanh ghi của user program

## mipssim.cc

### Thông tin

Mô phỏng lại CPU MIPS R2/3000.

### Các hàm
- `void Machine::run()`: mô phỏng quá trình thực thi của user program trên Nachos; được gọi bởi kernel khi user program khởi động.
- `static int TypeToReg(RegType reg, Instruction *instr)`: lấy giá trị thanh ghi trong một instruction.
- `void Machine::OneInstruction(Instruction *instr)`: thực thi các instruction từ user program. 
- `void Machine::DelayedLoad(int nextReg, int nextValue)`
- `void Instruction::Decode()`: giải mã MIPS instruction
- `static void Mult(int a, int b, bool signedArith, int* hiPtr, int* loPtr)`: thực hiện phép nhân trên R2000.

## console.h và console.cc

### Thông tin

Mô phỏng lại thiết bị nhập xuất qua terminal. 

Console là thiết bị không đồng nhất. Khi một ký tự được ghi lên thiết bị, thủ tục trả về ngay lập tức và interrupt handler sẽ được gọi khi I/O hoàn tất. Khi đọc, interrupt handler được gọi khi một ký tự mới xuất hiện.

Người dùng có thể gọi bất kỳ thủ tục nào khi I/O interrupt xuất hiện.

### console.h

Lớp `Console` là một thiết bị nhập xuất. Quá trình nhập xuất được mô phỏng qua việc đọc/ghi lên file UNIX.

### console.cc

Các hàm
- `Console::Console(char *readFile, char *writeFile, VoidFunctionPtr readAvail, VoidFunctionPtr writeDone, int callArg)`: khởi tạo console mới.
    - `readFile`: UNIX file mô phỏng nhập (`NULL` tức là dùng `stdin`)
    - `writeFile`: UNIX file mô phỏng xuất (`NULL` tức là dùng `stdout`)
    - `readAvail`: interrupt handler được gọi khi có ký tự mới xuất hiện
    - `writeDone`: interrupt handler được gọi khi ký tự đã được ghi để ký tự tiếp theo được ghi.
- `void Console::CheckCharAvail()`: kiểm tra xem ký tự tiếp theo có thể được nhập hay không, bằng cách kiểm tra kích thước của bộ đệm có đủ lớn hay không. Gọi interrupt handler cho việc nhập khi ký tự được đưa vào buffer
- `void Console::WriteDone()`: thủ tục được gọi khi interrupt handlder cho việc xuất được gọi, để báo hiệu cho kernel việc in ký tự đã hoàn thành.
- `char Console::GetChar()`: đọc ký tự từ input buffer.
- `void Console::PutChar(char ch)`: ghi ký tự lên màn hình và lên lịch cho interrupt

## synchcons.h và synchcons.cc

### Thông tin

Các hàm truy xuất giữa console và người dùng trong Nachos. Được định nghĩa ở lớp `SynchConsole` trong file synchcons.h.

## synchcons.cc

- `SynchConsole::SynchConsole()`: tạo một thiết bị console đồng bộ hóa với nhập xuất tiêu chuẩn (`stdin`/`stdout`).
- `SynchConsole::SynchConsole(char *in, char *out)`: tương tự như trên, nhưng với tên file nhập và xuất
- `int SynchConsole::Write(char *from, int numBytes)`: ghi một số lượng `numBytes` các byte từ buffer lên thiết bị I/O, trả về số lượng byte đã ghi
- `int SynchConsole::Read(char *into, int numBytes)`: đọc một số lượng `numBytes` các byte từ thiết bị I/O lên buffer, trả về số lượng byte đã đọc