#include "cli/cli_parser.h"
#include "telemetry/api/routes.h"
#include "service/http_server/http_server.h"
#include "telemetry/core/telemetry_storage.h"
#include "network/http/router/router_builder.h"

#include "logger.h"

#include <asio.hpp>

int main(int argc, char** argv)
{
    using namespace ctask::cli_parser;

    namespace Router = ctask::network::http::router;
    namespace Service = ctask::service;

    namespace TelemetryApi = ctask::telemetry::api;
    namespace TelemetryCore = ctask::telemetry::core;

    namespace Types = ctask::utils::types;

    auto log{Logger::instance().getLogger()};
    try
    {
        auto args{CliParser{"сtask", "Callstack coding task"}.parse(argc, argv)};
        {
            auto threadCount{static_cast<uint16_t>(std::thread::hardware_concurrency())};
            threadCount = threadCount == 0 ? 4 : threadCount;

            // 1 - is current, and 1 is logger
            threadCount -= 2;
            args.serverArgs.threads = threadCount;
            log->set_level(spdlog::level::from_str(args.loggerArgs.level));
        }

        Router::RouterBuilder routBuilder;
        TelemetryApi::TelemetryRoutes::registerRoutes(routBuilder,
                                                      std::make_shared<TelemetryCore::TelemetryStorage>());

        asio::io_service ctx;
        auto server{
            Service::HttpServer::сreateService(ctx,
                                               std::move(args.serverArgs),
                                               routBuilder.build())
        };

        asio::signal_set signals(ctx, SIGINT);
        signals.async_wait([&](const asio::error_code& ec, int signal)
        {
            log->info(std::format("Shutdown server, signal : {}", signal));
            server->stop();
        });

        server->start();
        log->info("See Ya 👋");
    }
    catch (const std::exception& e)
    {
        log->error("Exception occured: {}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
