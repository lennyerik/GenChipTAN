cmake_minimum_required(VERSION 3.1)

project(GenChipTAN)
set(CMAKE_CXX_STANDARD 17)

add_executable(GenChipTAN
        src/main.cpp
        src/ChipTAN.cpp
        src/ChipTAN.h
        src/Graphics.cpp
        src/Graphics.h
        src/CRC16.h
        )

# Threading
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# External libraries
find_library(QRENCODE_LIB qrencode)
if (NOT QRENCODE_LIB)
    message(FATAL_ERROR "libqrencode not found!")
endif()

find_library(NCURSES_LIB ncurses)
if(NOT NCURSES_LIB)
    message(FATAL_ERROR "ncurses not found!")
endif()

target_link_libraries(GenChipTAN ${QRENCODE_LIB} ${NCURSES_LIB} Threads::Threads)
