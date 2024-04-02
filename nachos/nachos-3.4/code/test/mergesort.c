#include "syscall.h"
#include "copyright.h"
#define MAX 30

float btof(byte* buffer) {
    int tmp = 0;
    int i;
    for (i = 0; i < 4; i++) {
        buffer[i] = buffer[i] == 0xFF ? 0 : buffer[i];
    }
    for (i = 0; i < 4; i++) {
        tmp |= buffer[i] << (i * 8);
    }
    return *(float*)(tmp);
}

byte* ftob(float num, byte* buffer) {
    int tmp = *(int*)(&num);
    int i;
    for (i = 0; i < 4; i++) {
        buffer[i] = (tmp >> (i * 8)) & 0xFF;
    }
    for (i = 0; i < 4; i++) {
        buffer[i] = buffer[i] == 0 ? 0xFF : buffer[i];
    }
    buffer[4] = '\0';
    return buffer;
}

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
    float arr[MAX];
    int n;
    int count = 0;
    OpenFileId output;
    char *inputEle = "Elem ";
    char *colon = ": ";
    char buffer[5];

    PrintString("Enter the number of elements: ");
    n = ReadInt();
    PrintString("Enter the elements:\n");
    while (count < n) {
        PrintString(inputEle);
        PrintInt(count);
        PrintString(colon);
        ReadFloat((char*)buffer, 5);
        arr[count] = btof((byte*)buffer);
        count++;
    }
    count = 0;

    mergeSort(arr, 0, n - 1);

    PrintString("Merge sort successfully!\n");

    if (Create("mergesort.txt") == -1) Halt();

    PrintString("Creating file mergesort.txt successfully\n");

    output = Open("mergesort.txt", 0);
    if (output == -1) Halt();

    PrintString("Opening file mergesort.txt successfully\n");

    while (count++ < n) {
        WriteF2File((char*)ftob(arr[count], (byte*)buffer), output);
    }

    PrintString("Write to file mergesort.txt successfully\n");

    Close(output);

    Halt();
}