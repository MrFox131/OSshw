#include <iostream>
#include "SharedMemory.hpp"
#define WIN32
#if defined(WIN32)
#include "windows.h"
#else
#define DWORD int
#endif

using namespace std;

DWORD WINAPI ThreadFunc(LPVOID lpParam) {
    cout << "test";
}

int main() {
    auto shmem = SharedMemory<int>("test_name");
    DWORD thread_id = 0;
    #if defined(WIN32)
        CreateThread(nullptr, 0, ThreadFunc, nullptr, 0, &thread_id);
    #endif
    
    return 0;
}
