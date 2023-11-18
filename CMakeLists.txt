cmake_minimum_required(VERSION 3.27)

project("Default test project by MrFox131")

add_executable(main main.c)

add_library(my_func my_func.c)

target_link_libraries(main my_func)