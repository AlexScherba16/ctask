#ifndef TELEMETRY_ROUTES_H
#define TELEMETRY_ROUTES_H

#include <memory>

namespace ctask::telemetry::core
{
    class TelemetryStorage;
}

namespace ctask::network::http::router
{
    class RouterBuilder;
}

namespace ctask::telemetry::api
{
    namespace Router = network::http::router;

    /**
     * @class TelemetryRoutes
     * @brief Static utility class for registering telemetry-related HTTP routes.
     *
     * Provides a method to register API endpoints for telemetry interactions.
     * Routes are registered via RouterBuilder, with handlers interacting
     * with TelemetryStorage.
     *
     * This class is non-instantiable and non-copyable.
     */
    class TelemetryRoutes
    {
    public:
        TelemetryRoutes() = delete;
        TelemetryRoutes(const TelemetryRoutes&) = delete;
        TelemetryRoutes(TelemetryRoutes&&) = delete;
        TelemetryRoutes& operator=(const TelemetryRoutes&) = delete;
        TelemetryRoutes& operator=(TelemetryRoutes&&) = delete;
        ~TelemetryRoutes() = delete;

        /**
         * @brief Registers telemetry-related routes.
         *
         * Integrates telemetry storage with the HTTP router by defining
         * API endpoints and associating them with appropriate handlers.
         *
         * @param builder Router builder instance for route registration.
         * @param storage Shared pointer to the telemetry storage instance.
         */
        static void registerRoutes(Router::RouterBuilder& builder,
                                   std::shared_ptr<core::TelemetryStorage> storage);
    };
}

#endif //TELEMETRY_ROUTES_H
