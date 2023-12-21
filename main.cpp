#if defined(WIN32)
#include "windows.h"
#else
#define DWORD unsigned long int
#define WINAPI
#define LPVOID void*
#endif
#include <iostream>
#include "SharedMemory.hpp"


using namespace std;

DWORD WINAPI ThreadFunc(LPVOID lpParam) {
    cout << "Hi";
    return 0;
}

int main() {
    auto shmem = SharedMemory<int>("test_name");
    DWORD thread_id = 0;
    #if defined(WIN32)
        HANDLE h = CreateThread(
                nullptr,
                0,
                ThreadFunc,
                nullptr,
                0,
                &thread_id
                );
        cout << "Hello";
        WaitForSingleObject(h, 2000);
    #endif
    
    return 0;
}
