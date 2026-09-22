cmake_minimum_required(VERSION 3.20)
project(Assembly2)

enable_language(ASM_MASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Assembly2 
        src/main.cpp
        src/asm_basics.asm
)
