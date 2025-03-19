project(test)

enable_testing()

add_executable(test test_main.cpp
        cli_test/cli_test.cpp
)

target_include_directories(test PRIVATE ${CMAKE_SOURCE_DIR}/ctask_lib)
target_compile_options(test PRIVATE -fexperimental-library)
target_link_libraries(test PRIVATE ctask_lib GTest::gtest GTest::gtest_main)
