cmake_minimum_required(VERSION 3.10)

project(dfa_minimization)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(dfa_minimization
    src/main.cpp
    src/nfa.cpp
    src/minimizer.cpp
    src/regex_to_nfa.cpp
)

target_include_directories(dfa_minimization PRIVATE src)
