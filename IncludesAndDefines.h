//
// Created by mrfox131 on 03.01.24.
//

#ifndef DEFAULT_TEST_PROJECT_BY_MRFOX131_INCLUDESANDDEFINES_H
#define DEFAULT_TEST_PROJECT_BY_MRFOX131_INCLUDESANDDEFINES_H

#if defined(WIN32)

#include "windows.h"
#define THREAD_HANDLE HANDLE

#else

#define DWORD void*
#define WINAPI
#define LPVOID void*
#define THREAD_HANDLE pthread_t
#define HANDLE int

#endif

#endif //DEFAULT_TEST_PROJECT_BY_MRFOX131_INCLUDESANDDEFINES_H
