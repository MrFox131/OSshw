//
// Created by mrfox131 on 21.10.23.
//
//#include <windef.h>
#ifndef SHARED_MEMORY
#define SHARED_MEMORY

#include <cstring>
#include <iostream>
#if defined (WIN32)

#include <winbase.h>
#define SEM_HANDLE HANDLE
#define MAP_NAME_PREFIX "Local\\"

#else

#include <fcntl.h>           /* For O_* constants */
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <unistd.h>
#include <semaphore.h>
#include <cstdlib>

#define HANDLE int
#define SEM_HANDLE sem_t*
#define INVALID_HANDLE_VALUE -1
#define MAP_NAME_PREFIX "/"

#endif

#define SEMAFOR_NAME_SUFFIX "_SEM"
template <class T> class SharedMemory {
private:

    bool CreateSharedMemoryAndSemafor(const char* memory_name, const char* semafor_name);

    bool OpenMem(const char* memory_name, const char* semafor_name);

    bool MapMemory();

    bool UnmapMemory();

    void CloseMem();

    void DestroyMem();

    void LockSemafor();

    void UnlockSemafor();

    bool IsValid();

    HANDLE _fd;
    SEM_HANDLE _sem;

    char *fd_full_name;
    char *semafor_full_name;

public:
    explicit SharedMemory(const char* name, bool create_if_does_not_exist = true);

    virtual ~SharedMemory();

    void Lock();

    void Unlock();

    class ContentData {
    public:
        T data;
        int number_acquired;
    };

    ContentData *content;

};

#include "SharedMemory.cpp"
#endif