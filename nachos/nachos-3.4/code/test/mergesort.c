#include "syscall.h"
#include "copyright.h"
#define MAX 99

void merge(int arr[], int start, int mid, int end)
{
    int start2 = mid + 1;
    int value;
    int index;

    if (arr[mid] <= arr[start2]) {
        return;
    }

    while (start <= mid && start2 <= end) {

        if (arr[start] <= arr[start2]) {
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

void mergeSort(int arr[], int l, int r)
{
    int m;
    if (l < r) {
        m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
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
        Write(space, 1, output);
    }

    PrintString("Write to file mergesort.txt successfully\n");

    Close(output);

    for (count = 0; count < n; count++) {
        ClearFloat(arr[count]);
    }

    Halt();
}