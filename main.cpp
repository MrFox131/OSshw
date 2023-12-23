#if defined(WIN32)

#include "windows.h"
#define THREAD_HANDLE HANDLE

#else

#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#define DWORD void*
#define WINAPI
#define LPVOID void*
#define THREAD_HANDLE pthread_t
#define HANDLE int

#endif

#include <iostream>
#include "SharedMemory.hpp"
#include <ctime>


using namespace std;

HANDLE fd;

void WriteLog(const char* data) {
#if defined(WIN32)
    DWORD written;

    OVERLAPPED ol = {0};

    LockFileEx(fd, LOCKFILE_EXCLUSIVE_LOCK, 0, 0, MAXDWORD, &ol);

    GetOverlappedResult(fd, &ol, &written, true);

    WriteFile(fd, data, strlen(data), &written, NULL);
    UnlockFileEx(fd, 0, 0, MAXDWORD, &ol);
#else
    write(fd, data, strlen(data));
#endif
}


#if !defined(WIN32)
void Sleep(int milliseconds) {
    usleep(milliseconds*1000);
}
#endif

DWORD WINAPI Incrementor(SharedMemory<int>* shmem) {
    while(true) {
        shmem->Lock();
        shmem->content->data++;
        shmem->Unlock();
        Sleep(300);
    }
#if defined(WIN32)
    return 0;
#else
    return nullptr;
#endif
}

int copyA() {
    auto shmem = new SharedMemory<int>("test_name");
    char output[255];
#if defined(WIN32)
    SYSTEMTIME st;
    GetLocalTime(&st);
    sprintf(output, "%02d:%02d:%02d.%03d %d\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, GetCurrentProcessId());
#else
    auto t = time(nullptr);
    auto lt = localtime(&t);
    sprintf(output, "%02d:%02d:%02d %d\n", lt->tm_hour, lt->tm_min, lt->tm_sec, getpid());
#endif
    shmem->Lock();
    WriteLog(output);
    shmem->content->data += 10;
#if defined(WIN32)
    GetLocalTime(&st);
    sprintf(output, "%02d:%02d:%02d.%03d %d %d\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, GetCurrentProcessId());
#else
    t = time(nullptr);
    lt = localtime(&t);
    sprintf(output, "%02d:%02d:%02d %d\n", lt->tm_hour, lt->tm_min, lt->tm_sec, getpid());
#endif
    WriteLog(output);
    shmem->Unlock();
    return 0;
}

int copyB() {
    auto shmem = new SharedMemory<int>("test_name");
    char output[255];
#if defined(WIN32)
    SYSTEMTIME st;
    GetLocalTime(&st);
    sprintf(output, "%02d:%02d:%02d.%03d %d\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, GetCurrentProcessId());
#else
    auto t = time(nullptr);
    auto lt = localtime(&t);
    sprintf(output, "%02d:%02d:%02d %d\n", lt->tm_hour, lt->tm_min, lt->tm_sec, getpid());
#endif
    shmem->Lock();
    WriteLog(output);
    shmem->content->data *= 2;
    shmem->Unlock();
    Sleep(2000);
    shmem->Lock();
    shmem->content->data = shmem->content->data / 2;

#if defined(WIN32)
    GetLocalTime(&st);
    sprintf(output, "%02d:%02d:%02d.%03d %d %d\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, GetCurrentProcessId());
#else
    t = time(nullptr);
    lt = localtime(&t);
    sprintf(output, "%02d:%02d:%02d %d\n", lt->tm_hour, lt->tm_min, lt->tm_sec, getpid());
#endif
    WriteLog(output);
    shmem->Unlock();
    return 0;
}

char *current_executable_path;

DWORD WINAPI CopyRunner(SharedMemory<int>* shmem) {
#if defined(WIN32)
    PROCESS_INFORMATION a, b;
    char *ca = (char*)calloc(strlen(current_executable_path)+7, sizeof(char));
    char *cb = (char*)calloc(strlen(current_executable_path)+7, sizeof(char));
    memcpy(ca, current_executable_path, strlen(current_executable_path));
    memcpy(cb, current_executable_path, strlen(current_executable_path));
    memcpy(ca + strlen(current_executable_path), " copyA", 7);
    memcpy(cb + strlen(current_executable_path), " copyB", 7);
    while (true) {
        Sleep(3000);
        DWORD exit_code_a = 0, exit_code_b = 0;
        auto a_status = GetExitCodeProcess(a.hProcess, &exit_code_a);
        auto b_status = GetExitCodeProcess(b.hProcess, &exit_code_b);
        if (exit_code_a != STILL_ACTIVE && exit_code_b != STILL_ACTIVE) {
            CloseHandle(a.hProcess);
            CloseHandle(a.hThread);
            STARTUPINFO si;
            ZeroMemory( &si, sizeof(si) );
            si.cb = sizeof(si);
            CreateProcess(nullptr, ca, nullptr, nullptr, false, 0, nullptr, nullptr, &si, &a);

            CloseHandle(b.hProcess);
            CloseHandle(b.hThread);
            ZeroMemory( &si, sizeof(si) );
            si.cb = sizeof(si);
            CreateProcess(nullptr, cb, nullptr, nullptr, false, 0, nullptr, nullptr, &si, &b);
        } else {
            WriteLog("Copies still working. Wait...\n");
        }
    }
#else
    int a = 0, b = 0;
    while (true)
    {
        Sleep(3000);
        int status;
        if (a==0 && b==0 || waitpid(a, &status, WNOHANG) && waitpid(b, &status, WNOHANG)) {
            a = fork();
            if (a == 0) {
                copyA();
                return nullptr;
            }
            b = fork();
            if (b == 0) {
                copyB();
                return nullptr;
            }
        } else {
            WriteLog("Copies still working. Wait...\n");
        }
    }


#endif
}

DWORD WINAPI Logger(SharedMemory<int>* shmem, HANDLE fd) {
    while(true) {
        Sleep(1000);
        shmem -> Lock();
#if defined(WIN32)
        SYSTEMTIME st;
        GetLocalTime(&st);

        char output[255];
        sprintf(output, "%02d:%02d:%02d.%03d %d %d\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, GetCurrentProcessId(),shmem->content->data);
#else
        auto t = time(nullptr);
        auto lt = localtime(&t);
        char output[255];
        sprintf(output, "%02d:%02d:%02d %d %d\n", lt->tm_hour, lt->tm_min, lt->tm_sec, getpid(),shmem->content->data);
#endif
        WriteLog(output);
        shmem->Unlock();
    }
}

HANDLE OpenFile(const char* filename, bool new_file) {
#if defined(WIN32)

    HANDLE h;
    if (new_file) {
        h = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h == INVALID_HANDLE_VALUE)
            h = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    } else {
        h = CreateFile(filename, GENERIC_WRITE | FILE_APPEND_DATA, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    }
    return h;

#else
    if (new_file)
        return open(filename, O_WRONLY | O_CREAT | O_TRUNC);
    else
        return open(filename, O_APPEND);
#endif
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        fd = OpenFile("logs.log", false);
        if (strcmp(argv[1], "copyA") == 0) {
            copyA();
            return 0;
        } else if (strcmp(argv[1], "copyB") == 0) {
            copyB();
            return 0;
        }

        return 0;
    } else {
        fd = OpenFile("logs.log", true);
    }
    current_executable_path = argv[0];
    auto shmem = new SharedMemory<int>("test_name");

    shmem->content->data=1;

    THREAD_HANDLE incrementer_h;
    #if defined(WIN32)
        DWORD thread_id;
        incrementer_h = CreateThread(
                nullptr,
                0,
                reinterpret_cast<LPTHREAD_START_ROUTINE>(Incrementor),
                shmem,
                0,
                &thread_id
                );
    #else
        pthread_attr_t attrs;
        pthread_attr_init(&attrs);
        pthread_create(
                &incrementer_h,
                &attrs,
                reinterpret_cast<void *(*)(void *)>(Incrementor),
                shmem
                );
    #endif

    THREAD_HANDLE copy_runner_h;
    #if defined(WIN32)
        copy_runner_h = CreateThread(
                nullptr,
                0,
                reinterpret_cast<LPTHREAD_START_ROUTINE>(CopyRunner),
                shmem,
                0,
                &thread_id
                );
    #else
        pthread_attr_t copy_runner_attr;
        pthread_attr_init(&copy_runner_attr);
        pthread_create(
                &copy_runner_h,
                &copy_runner_attr,
                reinterpret_cast<void *(*)(void *)>(CopyRunner),
                shmem
                );
    WriteLog("Run runner\n");
    #endif

    THREAD_HANDLE logger_h;
#if defined(WIN32)
    logger_h = CreateThread(
                nullptr,
                0,
                reinterpret_cast<LPTHREAD_START_ROUTINE>(Logger),
                shmem,
                0,
                &thread_id
                );
#else
    pthread_attr_t logger_attr;
    pthread_attr_init(&logger_attr);
    pthread_create(
            &logger_h,
            &logger_attr,
            reinterpret_cast<void *(*)(void *)>(Logger),
            shmem
    );
#endif

    while (true) {
        int tmp;
        cin >> tmp;
        shmem->Lock();
        shmem->content->data = tmp;
        shmem->Unlock();
    }
    
    return 0;
}
