project(ctask_lib)

add_library(ctask_lib STATIC
        utils/types/types.h
        cli/cli_parser.h
        cli/cli_parser.cpp
        network/http/parser/i_http_parser.h
        network/http/parser/json/json_http_parser.cpp
        network/http/parser/json/json_http_parser.h
        utils/types/constants.h
)

target_include_directories(ctask_lib PRIVATE ${CMAKE_SOURCE_DIR}/ctask_lib)
target_compile_options(ctask_lib PRIVATE -fexperimental-library)
target_link_libraries(ctask_lib
        cxxopts::cxxopts
        nlohmann_json::nlohmann_json
        asio::asio
        llhttp::llhttp
)
