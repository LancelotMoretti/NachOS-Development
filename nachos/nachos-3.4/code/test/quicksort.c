#include "syscall.h"
#include "copyright.h"

#define MAX 99

int quicksort(int *arr, int n) {
    int piv, beg[MAX], end[MAX];
    int i = 0, L, R;
    int totalShifting = 0;

    beg[0] = 0;
    end[0] = n;
    while (i >= 0) {
        totalShifting++;
        L = beg[i];
        R = end[i] - 1;
        if (L < R) {
            piv = arr[L];
            if (i == MAX - 1)
                return -1;
            while (L < R) {
                while (arr[R] >= piv && L < R)
                    R--;
                if (L < R)
                    arr[L++] = arr[R];
                while (arr[L] <= piv && L < R)
                    L++;
                if (L < R)
                    arr[R--] = arr[L];
            }
            arr[L] = piv;
            beg[i + 1] = L + 1;
            end[i + 1] = end[i];
            end[i++] = L;
        } else {
            i--;
        }
    }

    PrintString("Total shifting: ");
    PrintInt(totalShifting);
    PrintString("\n");

    return 0;
}

int main() {
    int arr[MAX];
    int n;
    int count;
    OpenFileId output;
    char buffer[20];
    char *inputEle = "Element ";
    char *colon = ": ";

    PrintString("Enter the number of elements: ");
    n = ReadInt();
    if (n <= 0 || n > MAX) {
        PrintString("Invalid number of elements\n");
        Halt();
    }
    PrintString("Enter the elements:\n");
    while (count < n) {
        PrintString(inputEle);
        PrintInt(count);
        PrintString(colon);
        arr[count] = ReadInt();
        count++;
    }
    count = 0;

    quicksort(arr, n);

    PrintString("Quick sort successfully\n");

    if (Create("quicksort.txt") == -1) {
        PrintString("Error creating file\n");
        Halt();
    }

    PrintString("Creating file quicksort.txt successfully\n");

    output = Open("quicksort.txt", 0);
    if (output == -1) {
        PrintString("Error opening file\n");
        Halt();
    }

    PrintString("Opening file quicksort.txt successfully\n");

    while (count < n) {
        int size = 0;
        int tmp = arr[count];
        int isNeg = 0;
        int i = 0;
        char* strBuffer = buffer;

        if (tmp < 0) {
            size++;
            strBuffer[0] = '-';
            isNeg = 1;
            tmp = -tmp;
        }
        else if (tmp == 0) {
            size++;
            strBuffer[0] = '0';
        }

        while (tmp > 0) {
            size++;
            tmp /= 10;
        }

        tmp = arr[count];
        i = size + isNeg - 1;

        while (i >= isNeg && tmp > 0) {
            strBuffer[i--] = (tmp % 10) + '0';
            tmp /= 10;
        }

        if (count != n - 1) strBuffer[size++] = ' ';
        strBuffer[size] = '\0';

        Write(strBuffer, size, output);
        count++;
    }
    count = 0;

    PrintString("Writing to file quicksort.txt successfully\n");

    Close(output);

    Halt();
}