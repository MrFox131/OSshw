#include <iostream>
#include "SharedMemory.hpp"

#if defined(WIN32)
#include "windows.h"
#else
#define LPDWORD int
#endif

using namespace std;

int ThreadFunc() {
    cout << "test";
}

int main() {
    auto shmem = SharedMemory<int>("test_name");
    LPDWORD* thread_id = new LPDWORD;
    #if defined(WIN32)
        CreateThread(NULL, 0, ThreadFunc, NULL, 0, &thread_id);
    #endif
    
    return 0;
}
