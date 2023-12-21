#include "SharedMemory.hpp"

template <class T>
bool SharedMemory<T>::CreateSharedMemoryAndSemafor(const char* memory_name, const char* semafor_name) {
#if defined (WIN32)
    _fd = CreateFileMapping(INVALID_HANDLE_VALUE,
                                NULL,
                                PAGE_READWRITE,
                                0,
                                sizeof(ContentData),
                                memory_name
                                );
        if (_fd != INVALID_HANDLE_VALUE) {
            _sem = CreateSemaphore(NULL, 0, 1, semafor_name);
        }
#else
    _fd = shm_open(memory_name, O_RDWR | O_CREAT | O_EXCL, 0644);
    if (_fd != INVALID_HANDLE_VALUE) {
        ftruncate(_fd, sizeof(ContentData));
        _sem = sem_open(semafor_name, O_CREAT | O_EXCL, 0644, 1);
        if (_sem == SEM_FAILED) {
            _sem = nullptr;
        }
    }
#endif
    return (_fd != INVALID_HANDLE_VALUE && _sem != nullptr);
}

template <class T>
bool SharedMemory<T>::OpenMem(const char* memory_name, const char* semafor_name) {
#if defined (WIN32)
    _fd = OpenFileMapping(FILE_MAP_WRITE, true, memory_name);
        if (_fd != INVALID_HANDLE_VALUE) {
            _sem = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false, semafor_name);
        }
#else
    _fd = shm_open(memory_name, O_RDWR, 0644);
    if (_fd != INVALID_HANDLE_VALUE) {
        _sem = sem_open(semafor_name, 0);
        if (_sem == SEM_FAILED) {
            _sem = nullptr;
        }
    }
#endif
    return (_sem != nullptr && _fd != INVALID_HANDLE_VALUE);
}

template <class T>
bool SharedMemory<T>::MapMemory() {
    if (_fd == INVALID_HANDLE_VALUE) {
        return false;
    }
#if defined (WIN32)
    content = reinterpret_cast<ContentData*>(MapViewOfFile(_fd, FILE_MAP_WRITE, 0, 0, sizeof(ContentData)));
#else
    void *res = mmap(NULL, sizeof(ContentData), PROT_WRITE | PROT_READ, MAP_SHARED, _fd, 0);
    if (res == MAP_FAILED) {
        content = nullptr;
    } else {
        content = reinterpret_cast<ContentData*>(res);
    }
#endif
    return content != nullptr;
}

template <class T>
bool SharedMemory<T>::UnmapMemory() {
    if (_fd == INVALID_HANDLE_VALUE) {
        return false;
    }
#if defined (WIN32)
    UnmapViewfFile(content);
#else
    munmap(content, sizeof(ContentData));
#endif
    content = nullptr;
    return true;
}

template <class T>
void SharedMemory<T>::CloseMem() {
    UnmapMemory();
    if (_fd != INVALID_HANDLE_VALUE) {
#if defined(WIN32)
        CloseHandle(_fd);
#else
        close(_fd);
#endif
        _fd = INVALID_HANDLE_VALUE;
    }
    if (_sem != nullptr) {
#if defined(WIN32)
        CloseHandle(_sem);
#else
        sem_close(_sem);
#endif
        _sem = nullptr;
    }
}

template <class T>
void SharedMemory<T>::DestroyMem() {
    CloseMem();
#if not defined(WIN32)
    shm_unlink(fd_full_name);
    sem_unlink(semafor_full_name);
#endif
}

template <class T>
void SharedMemory<T>::LockSemafor() {
#if defined(WIN32)
    ReleaseSemaphore(_sem, 1, NULL);
#else
    sem_post(_sem);
#endif
}

template <class T>
void SharedMemory<T>::UnlockSemafor() {
#if defined(WIN32)
    WaitForSingleObject(_sem, 0);
#else
    sem_wait(_sem);
#endif
}

template <class T>
bool SharedMemory<T>::IsValid() {
    return (_sem != nullptr && _fd != INVALID_HANDLE_VALUE);
}

template <class T>
SharedMemory<T>::SharedMemory(const char* name, bool create_if_does_not_exist):_fd(INVALID_HANDLE_VALUE), content(NULL), _sem(nullptr) {
    fd_full_name = (char*) malloc(strlen(name) + strlen(MAP_NAME_PREFIX) + 1);
    semafor_full_name = (char*) malloc(strlen(name) + strlen(MAP_NAME_PREFIX) + strlen(SEMAFOR_NAME_SUFFIX) + 1);

    memcpy(fd_full_name, MAP_NAME_PREFIX, strlen(MAP_NAME_PREFIX));
    memcpy(fd_full_name+ strlen(MAP_NAME_PREFIX), name, strlen(name) + 1);

    memcpy(semafor_full_name, name, strlen(name));
    memcpy(semafor_full_name + strlen(name), SEMAFOR_NAME_SUFFIX, strlen(SEMAFOR_NAME_SUFFIX) + 1);

    bool is_new = false;

    bool opened_successfully = OpenMem(fd_full_name, semafor_full_name);

    if (!opened_successfully && create_if_does_not_exist) {
        opened_successfully = CreateSharedMemoryAndSemafor(fd_full_name, semafor_full_name);
        if (opened_successfully) {
            is_new = true;
        }
    }

    if (opened_successfully) {
        opened_successfully = MapMemory();
    }

    if (opened_successfully) {
        if (is_new) {
            content->number_acquired = 0;
            content->data = T();
        }

        Lock();
        content->number_acquired += 1;
        Unlock();
    } else {
        if (is_new)
            DestroyMem();
        else
            CloseMem();
    }

}

template <class T>
SharedMemory<T>::~SharedMemory() {
    if (IsValid()) {
        int cnt = 0;
        Lock();
        content -> number_acquired--;
        cnt = content -> number_acquired;
        UnlockSemafor();
        if (cnt <= 0) {
            DestroyMem();
        } else {
            CloseMem();
        }
    }
    free(semafor_full_name);
    free(fd_full_name);
}

template <class T>
void SharedMemory<T>::Lock() {
    LockSemafor();
}

template <class T>
void SharedMemory<T>::Unlock() {
    UnlockSemafor();
}