project(ctask_lib)

add_library(ctask_lib STATIC
        utils/types/types.h
        cli/cli_parser.h
        cli/cli_parser.cpp
        service/i_service.h
        service/http_server/http_server.cpp
        service/http_server/http_server.h
        network/http/parser/i_http_parser.h
        network/http/parser/json/http_parser.cpp
        network/http/parser/json/http_parser.h
        utils/types/constants.h
        network/http/response_serializer/json/response_serializer.cpp
        network/http/response_serializer/json/response_serializer.h
        network/http/router/i_router.h
        network/http/router/custom/router.cpp
        network/http/router/custom/router.h
        network/http/router/router_builder.h
        network/http/router/router_builder.cpp
        network/http/router/router_builder.h
        telemetry/dto/dto.h
        telemetry/core/models.h
        telemetry/core/telemetry_storage.cpp
        telemetry/core/telemetry_storage.h
        telemetry/core/misc.h
        telemetry/api/routes.cpp
        telemetry/api/routes.h
        logger.h
)

target_include_directories(ctask_lib PRIVATE ${CMAKE_SOURCE_DIR}/ctask_lib)
target_compile_options(ctask_lib PRIVATE -fexperimental-library)
target_link_libraries(ctask_lib
        cxxopts::cxxopts
        nlohmann_json::nlohmann_json
        asio::asio
        llhttp::llhttp
        spdlog::spdlog
)
