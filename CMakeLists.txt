cmake_minimum_required(VERSION 3.6)
project(2_x)
include(PrecompiledHeader.cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/main.cpp)

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(pch_source src/stdafx.cpp)
endif ()

add_executable(2_x ${SOURCE_FILES} src/stdafx.h ${pch_source})
add_precompiled_header(2_x src/stdafx.h)