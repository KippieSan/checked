cmake_minimum_required(VERSION 3.16)
project(checked)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++20")

add_executable(
    ${PROJECT_NAME}
    main.cpp
    checked.cpp
    test.cpp
)
