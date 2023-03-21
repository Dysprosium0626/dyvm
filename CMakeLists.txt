cmake_minimum_required(VERSION 3.24)
project(dyvm)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)

FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG v3.11.2
        )

FetchContent_MakeAvailable(json)


add_executable(dyvm main.cpp src/file_stream.cpp src/file_stream.hpp src/expression_calculator.cpp src/expression_calculator.hpp src/nfa.cpp src/nfa.hpp src/dfa.cpp src/dfa.hpp src/lex.cpp src/lex.hpp)
target_link_libraries(dyvm PRIVATE nlohmann_json::nlohmann_json)
