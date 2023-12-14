#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 4096

int main() {
    // Crear una vista de memoria compartida
    HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SIZE, L"MySharedMemory");
    if (hMapFile == NULL) {
        perror("CreateFileMapping");
        exit(EXIT_FAILURE);
    }

    // Mapear la vista de memoria compartida en el espacio de direcciones del proceso
    LPVOID shared_memory = MapViewOfFile(hMapFile, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, SIZE);
    if (shared_memory == NULL) {
        perror("MapViewOfFile");
        CloseHandle(hMapFile);
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        UnmapViewOfFile(shared_memory);
        CloseHandle(hMapFile);
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        printf("Child reads: %s\n", (char*)shared_memory);
        UnmapViewOfFile(shared_memory);
        CloseHandle(hMapFile);
        exit(EXIT_SUCCESS);
    } else {
        strcpy((char*)shared_memory, "Hello, child process!");

        // Esperar a que el proceso hijo termin
        WaitForSingleObject(pid, INFINITE);

        UnmapViewOfFile(shared_memory);
        CloseHandle(hMapFile);
    }

    return 0;
}