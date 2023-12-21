#if defined(WIN32)
#include "windows.h"
#define THREAD_HANDLE HANDLE
#else
#include <pthread.h>
#define DWORD void*
#define WINAPI
#define LPVOID void*
#define THREAD_HANDLE pthread_t
#endif
#include <iostream>
#include "SharedMemory.hpp"


using namespace std;

DWORD WINAPI ThreadFunc(LPVOID lpParam) {
    cout << "Hi";
    return nullptr;
}

int main() {
    auto shmem = SharedMemory<int>("test_name");
    DWORD thread_id;
    THREAD_HANDLE h;
    #if defined(WIN32)
        h = CreateThread(
                nullptr,
                0,
                ThreadFunc,
                nullptr,
                0,
                &thread_id
                );
        cout << "Hello";
        WaitForSingleObject(h, 2000);
    #else
        pthread_attr_t attrs;
        pthread_attr_init(&attrs);
        pthread_create(&h, &attrs, ThreadFunc, nullptr);
        cout << "Hello";
        pthread_join(h, nullptr);
    #endif
    
    return 0;
}
