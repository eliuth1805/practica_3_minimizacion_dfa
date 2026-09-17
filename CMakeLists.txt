cmake_minimum_required(VERSION 3.10)

project(nfa_to_dfa)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(nfa_to_dfa
    src/main.cpp
    src/nfa.cpp
)

target_include_directories(nfa_to_dfa PRIVATE src)
