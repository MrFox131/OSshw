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

DWORD WINAPI ThreadFunc(SharedMemory<int>* lpParam) {
    cout << lpParam->content->data;
#if defined(WIN32)
    return 0;
#else
    return nullptr;
#endif
}

int main() {
    auto shmem = new SharedMemory<int>("test_name");
    shmem->content->data = 55;
    DWORD thread_id;
    THREAD_HANDLE h;
    #if defined(WIN32)
        h = CreateThread(
                nullptr,
                0,
                reinterpret_cast<LPTHREAD_START_ROUTINE>(ThreadFunc),
                shmem,
                0,
                &thread_id
                );
        cout << "Hello";
        WaitForSingleObject(h, 2000);
    #else
        pthread_attr_t attrs;
        pthread_attr_init(&attrs);
        pthread_create(&h, &attrs, reinterpret_cast<void *(*)(void *)>(ThreadFunc), shmem);
        cout << "Hello";
        pthread_join(h, nullptr);
    #endif
    
    return 0;
}
