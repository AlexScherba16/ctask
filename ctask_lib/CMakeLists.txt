project(ctask_lib)

add_library(ctask_lib STATIC
        utils/types/types.h
        cli/cli_parser.h
        cli/cli_parser.cpp
)

target_include_directories(ctask_lib PRIVATE ${CMAKE_SOURCE_DIR}/ctask_lib)
target_link_libraries(ctask_lib
        cxxopts::cxxopts
        nlohmann_json::nlohmann_json
)
