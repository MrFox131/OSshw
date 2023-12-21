cmake_minimum_required(VERSION 3.25)

project("Default test project by MrFox131")

add_executable(main main.cpp SharedMemory.hpp)

target_link_libraries(main)

include_directories(SYSTEM /usr/x86_64-w64-mingw32/include)