#include "syscall.h"
#include "copyright.h"
#define MAX 99

void swap(int* p1, int* p2)
{
    *p1 += *p2;
    *p2 = *p1 - *p2;
    *p1 -= *p2;
}

int partition(int arr[], int low, int high)
{
    int pivot = arr[high];
    int i = (low - 1);
    int j = low;

    while (j++ <= high) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

// The Quicksort function Implement

void quickSort(int arr[], int low, int high)
{
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int main() {
    int arr[MAX];
    int n;
    int count;
    OpenFileId output;
    char buffer[12];
    char *inputEle = "Elem ";
    char *colon = ": ";

    PrintString("Enter n: ");
    n = ReadInt();
    PrintString("Array:\n");
    while (count < n) {
        PrintString(inputEle);
        PrintInt(count);
        PrintString(colon);
        arr[count] = ReadInt();
        count++;
    }
    count = 0;

    quickSort(arr, 0, n - 1);

    PrintString("Quicksort success\n");

    if (Create("quicksort.txt") == -1) Halt();

    PrintString("Creating file quicksort.txt successfully\n");

    output = Open("quicksort.txt", 0);
    if (output == -1) Halt();

    PrintString("Opening file quicksort.txt successfully\n");

    while (count < n) {
        int length = 0;
        int tmp = arr[count];
        int isNeg = 0;
        int i = 0;
        char* strBuffer = buffer;

        if (tmp < 0) {
            length++;
            strBuffer[0] = '-';
            isNeg = 1;
            tmp = -tmp;
        }
        else if (tmp == 0) {
            length++;
            strBuffer[0] = '0';
        }

        while (tmp > 0) {
            length++;
            tmp /= 10;
        }

        tmp = arr[count];
        i = length + isNeg - 1;

        while (i >= isNeg && tmp > 0) {
            strBuffer[i--] = (tmp % 10) + '0';
            tmp /= 10;
        }

        if (count != n - 1) strBuffer[length++] = ' ';
        strBuffer[length] = '\0';

        Write(strBuffer, length, output);
        count++;
    }
    count = 0;

    PrintString("Writing to file quicksort.txt successfully\n");

    Close(output);

    Halt();
}