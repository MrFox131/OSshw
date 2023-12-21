#include <iostream>
#include "SharedMemory.hpp"
#if defined(WIN32)
#include "windows.h"
#else
#define DWORD unsigned long int
#define WINAPI
#define LPVOID void*
#endif

using namespace std;

DWORD WINAPI ThreadFunc(LPVOID lpParam) {
    cout << "test";
}

int main() {
    auto shmem = SharedMemory<int>("test_name");
    DWORD thread_id = 0;
    #if defined(WIN32)
        CreateThread(
                nullptr,
                0,
                reinterpret_cast<int>(ThreadFunc),
                nullptr,
                0,
                &thread_id
                );
    #endif
    
    return 0;
}
