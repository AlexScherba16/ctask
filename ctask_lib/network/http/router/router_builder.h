#ifndef BUILDER_H
#define BUILDER_H

#include "network/http/router/i_router.h"

namespace ctask::network::http::router
{
    /**
     * @class RouterBuilder
     * @brief Builder for configuring and creating an HTTP router. Thanks to Golang =)
     *
     * Allows fluent registration of GET/POST routes, then produces a fully constructed router.
     *
     * This is the only single way to create a Router instance —
     * to ensure all routes are registered before the router is used.
     * Keeps construction controlled, centralized, manageable, clean, fancy and etc related to best practice stuff.
     */
    class RouterBuilder final
    {
    public:
        RouterBuilder() = default;
        ~RouterBuilder() = default;
        RouterBuilder(const RouterBuilder&) = delete;
        RouterBuilder& operator=(const RouterBuilder&) = delete;
        RouterBuilder(RouterBuilder&&) = delete;
        RouterBuilder& operator=(RouterBuilder&&) = delete;

        /**
         * @brief Registers a handler for an HTTP GET path.
         *
         * @param path The path for which to register the handler.
         * @param handler The function to handle the request.
         * @return RouterBuilder& instance.
         */
        RouterBuilder& registerGet(Types::HttpPath path, Types::HttpHandlerFn handler);

        /**
         * @brief Registers a handler for an HTTP POST path.
         *
         * @param path The path for which to register the handler.
         * @param handler The function to handle the request.
         * @return RouterBuilder& instance.
         */
        RouterBuilder& registerPost(Types::HttpPath path, Types::HttpHandlerFn handler);

        /**
         *@brief Builds and returns a fully configured router.
         *
         * After calling this, the builder has fulfilled its purpose.
         * The returned router is ready to use.
         *
         * @return std::unique_ptr<IRouter> The constructed router.
         *
         * @throws If path already registered
         */
        std::unique_ptr<IRouter> build();

    private:
        std::unordered_map<Types::HttpPath, Types::HttpHandlerFn> getHandlers_;
        std::unordered_map<Types::HttpPath, Types::HttpHandlerFn> postHandlers_;
    };
}

#endif //BUILDER_H
