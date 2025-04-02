cmake_minimum_required(VERSION 3.30)
project(ctask)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "=== Linux build 🐧 ===")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "=== MacOS build 🍎 ===")
else ()
    message(FATAL_ERROR "❌ Unsupported OS: ${CMAKE_SYSTEM_NAME}")
endif ()


if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DIGNORE_ONE_INSTANCE_CREATION_POLICY)
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)
find_package(cxxopts REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(asio REQUIRED)
find_package(llhttp REQUIRED)
find_package(spdlog REQUIRED)

add_subdirectory(ctask_lib)
add_subdirectory(test)

add_executable(ctask app/main.cpp)
target_include_directories(ctask PRIVATE ${CMAKE_SOURCE_DIR}/ctask_lib)
target_link_libraries(ctask PRIVATE ctask_lib)
