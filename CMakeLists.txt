cmake_minimum_required(VERSION 3.25)

project("Default test project by MrFox131")

add_executable(main main.cpp SharedMemory.hpp SerialPort.cpp SerialPort.h FileUtils.cpp FileUtils.h IncludesAndDefines.h)

target_link_libraries(main)

#include_directories(SYSTEM /usr/x86_64-w64-mingw32/include)