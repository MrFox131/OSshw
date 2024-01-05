//
// Created by mrfox131 on 03.01.24.
//

#ifndef DEFAULT_TEST_PROJECT_BY_MRFOX131_FILEUTILS_H
#define DEFAULT_TEST_PROJECT_BY_MRFOX131_FILEUTILS_H

#include "IncludesAndDefines.h"

HANDLE OpenFile(const char* filename, bool new_file);
void _CloseFile(HANDLE fd);
void WriteLog(const char* data, HANDLE fd);

#endif //DEFAULT_TEST_PROJECT_BY_MRFOX131_FILEUTILS_H
