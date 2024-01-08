//
// Created by mrfox131 on 03.01.24.
//

#include "FileUtils.h"
#include "IncludesAndDefines.h"

#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <cerrno>

HANDLE OpenFile(const char* filename, bool new_file) {
#if defined(WIN32)

    HANDLE h;
    if (new_file) {
        h = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
        if (h == INVALID_HANDLE_VALUE)
            h = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
    } else {
        h = CreateFile(filename, GENERIC_WRITE | FILE_APPEND_DATA, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
    }
    return h;

#else
    if (new_file) {
        HANDLE fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd == -1) {
            printf("%d\n", errno);
        }
        return fd;
    } else
        return open(filename, O_APPEND, 0666);
#endif
}

void _CloseFile(HANDLE fd) {
#if defined(WIN32)
    CloseHandle(fd);
#else
    close(fd);
#endif
}



void WriteLog(const char* data, HANDLE fd, bool override) {
#if defined(WIN32)
    DWORD written;

    if (override) {
        SetFilePointer(fd, 0, NULL, FILE_BEGIN);
    }

    OVERLAPPED ol = {0};
    ol.Offset = 0xFFFFFFFF;
    ol.OffsetHigh = 0xFFFFFFFF;

    WriteFile(fd, data, strlen(data), &written, &ol);
    CloseHandle(ol.hEvent);
#else
    if (override) {
        lseek(fd, 0, SEEK_SET);
    }
    write(fd, data, strlen(data));
#endif
}