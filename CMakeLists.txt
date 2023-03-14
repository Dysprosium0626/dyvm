cmake_minimum_required(VERSION 3.24)
project(dyvm)

set(CMAKE_CXX_STANDARD 14)





add_executable(dyvm main.cpp src/file_stream.cpp src/file_stream.hpp src/expression_calculator.cpp src/expression_calculator.hpp src/nfa.cpp src/nfa.hpp src/dfa.cpp src/dfa.hpp)
