cmake_minimum_required(VERSION 3.30)
project(ctask)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)
find_package(cxxopts REQUIRED)
find_package(nlohmann_json REQUIRED)

add_subdirectory(ctask_lib)
add_subdirectory(test)

add_executable(ctask app/main.cpp)
target_include_directories(ctask PRIVATE ${CMAKE_SOURCE_DIR}/ctask_lib)
target_link_libraries(ctask PRIVATE ctask_lib)
