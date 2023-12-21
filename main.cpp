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

DWORD WINAPI ThreadFunc(SharedMemory<int>* shmem) {
    shmem->Lock();
    cout << shmem->content->data << endl;
    shmem->content->data = 99;
    shmem->Unlock();
#if defined(WIN32)
    return 0;
#else
    return nullptr;
#endif
}

#if !defined(WIN32)
void Sleep(int milliseconds) {
    usleep(milliseconds*1000);
}
#endif

int main() {
    auto shmem = new SharedMemory<int>("test_name");
    shmem->Lock();
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
    #else
        pthread_attr_t attrs;
        pthread_attr_init(&attrs);
        pthread_create(&h, &attrs, reinterpret_cast<void *(*)(void *)>(ThreadFunc), shmem);
    #endif

    cout << "Hello" << endl;
    shmem->Unlock();
    Sleep(1000);
    shmem->Lock();
    cout << shmem->content->data << endl;
    shmem->Unlock();
    
    return 0;
}
