cmake_minimum_required(VERSION 3.17)
project(ProjetoSO C)

set(CMAKE_C_STANDARD 11)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

add_executable(ProjetoSO main.c files.c files.h gestorCorrida.c gestorCorrida.h gestorAvarias.c gestorAvarias.h gestorEquipas.c gestorEquipas.h SLinkedListManipulation.c SLinkedListManipulation.h main.h)