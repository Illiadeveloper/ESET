cmake_minimum_required(VERSION 3.20)

project(ESET LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(file_searcher main.cpp FileSearcher.cpp)
