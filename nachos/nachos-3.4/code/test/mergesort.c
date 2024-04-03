#include "syscall.h"
#include "copyright.h"
#define MAX 99

void merge(float* arr[], int start, int mid, int end)
{
    int start2 = mid + 1;
    float* value;
    int index;

    // if (arr[mid] <= arr[start2]) {
    //     return;
    // }

    if (CompareFloat(arr[mid], arr[start2]) <= 0) {
        return;
    }

    while (start <= mid && start2 <= end) {

        if (CompareFloat(arr[start], arr[start2]) <= 0) {
        // if (arr[start] <= arr[start2]) {
            start++;
        }
        else {
            value = arr[start2];
            index = start2;

            while (index != start) {
                arr[index] = arr[index - 1];
                index--;
            }
            arr[start++] = value;
            mid++;
            start2++;
        }
    }
}

void mergeSort(float* arr[], int left, int right)
{
    int mid;
    if (left < right) {
        mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

int main() {
    float* arr[MAX];
    int n;
    int count = 0;
    OpenFileId output;
    char *inputEle = "Elem ";
    char *colon = ": ";
    char *space = " ";

    PrintString("Enter n: ");
    n = ReadInt();
    if (n <= 0 || n > MAX) {
        PrintString("Invalid number of elements\n");
        Halt();
    }
    PrintString("Array:\n");
    while (count < n) {
        PrintString(inputEle);
        PrintInt(count);
        PrintString(colon);
        arr[count] = ReadFloat();
        count++;
    }
    count = 0;

    mergeSort(arr, 0, n - 1);

    PrintString("Mergesort success\n");

    if (Create("mergesort.txt") == -1) Halt();

    PrintString("Creating file mergesort.txt successfully\n");

    output = Open("mergesort.txt", 0);
    if (output == -1) Halt();

    PrintString("Opening file mergesort.txt successfully\n");

    while (count++ < n) {
        WriteF2File(arr[count - 1], output);
        if (count < n) Write(space, 1, output);
    }

    PrintString("Write to file mergesort.txt successfully\n");

    Close(output);

    for (count = 0; count < n; count++) {
        ClearFloat(arr[count]);
    }

    Halt();
}