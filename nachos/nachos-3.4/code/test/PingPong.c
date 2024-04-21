#include "syscall.h"

int main() {
    SpaceId ping = -1, pong = -1;
    int i;
    if (CreateSemaphore("First", 1) == -1 || CreateSemaphore("Second", 0) == -1) {
        PrintString("Error creating semaphore\n");
        Halt();
    }
    ping = Exec("../test/Ping");
    pong = Exec("../test/Pong");
    
    Join(ping);
    Join(pong);
    
    Halt();
}