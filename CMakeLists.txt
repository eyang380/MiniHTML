cmake_minimum_required(VERSION 3.16)

project(MiniHTML)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(minihtml
    src/main.cpp
    src/parser/parser.cpp
    src/tokenizer/tokenizer.cpp
)

target_include_directories(minihtml PRIVATE src)