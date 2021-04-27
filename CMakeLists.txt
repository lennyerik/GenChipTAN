cmake_minimum_required(VERSION 3.13)

project(GenChipTAN)
set(CMAKE_CXX_STANDARD 17)

option(COMPILE_WITH_CLI_GRAPHICS "Compile the executable without cli graphics" ON)

add_executable(GenChipTAN
        src/main.cpp
        src/ChipTAN.cpp
        src/ChipTAN.h
        src/CRC.h
        )

find_library(QRENCODE_LIB qrencode)
if (NOT QRENCODE_LIB)
    message("libqrencode not found, compiling without cli graphics")
    set(COMPILE_WITH_CLI_GRAPHICS OFF)
endif()

find_library(NCURSES_LIB ncurses)
if(NOT NCURSES_LIB)
    message("ncurses not found, compiling without cli graphics")
    set(COMPILE_WITH_CLI_GRAPHICS OFF)
endif()

if (COMPILE_WITH_CLI_GRAPHICS)
    target_link_libraries(GenChipTAN ${QRENCODE_LIB} ${NCURSES_LIB})
    add_definitions(-DCOMPILE_WITH_CLI_GRAPHICS)
endif()