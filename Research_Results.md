# Folder 'threads'

**1.** The result maybe not true since this is only a research about folder 'threads' in NachOS-3.4 and there are many main functions in NachOS-3.4

**2.** NachOS start in function [main.cc](../NachOS-Development/nachos/nachos-3.4/code/threads/main.cc)

**3.** NachOS call Initialize function and pass all terminal arguments. In Initialize function, the for loop won't run if we use the command ***"./nachos"***

**4.** After the for loop, NachOS will allocate memory for Staticstics, Interrupt, Timer, and Thread (with the name "main")

**5.** For Timer, when being allocated, NachOS will pass function TimerInterruptHandler to timerHandler, 0 to callArg, and false to doRandom. Then, NachOS will call interrupt->Schedule with the parameter TimerHandler (void function), address of Timer (?), 100 (TimerTicks), IntType::TimerInt (0)

**6.** In Schedule method of Interrupt class, NachOS will initialize a PendingInterrupt object and Insert to pending list using SortedInsert method for ???

**7.** After all of previous steps, somehow the ASSERT in Interrupt::SetLevel(IntStatus) receive FALSE and not abort (inHandler is set to TRUE and it mean that Interrupt::CheckIfDue is called somewhere?). Then the NachOS finish interrupt->Enable() and go out from Initialize function

**8.** NachOS successfull go to the end of main function and call currentThread->Finish() to finish the main thread (which will print "No threads ready or runnable, and no pending interrupts." and then call interrupt->Halt() to halt the machine)

> **_NOTE:_** Remember to check where the inHandler is set to TRUE

> **_NOTE:_** Remember to further research the Timer constructor called in Initialize function since somehow the inHandler isn't appear in it which is really strange