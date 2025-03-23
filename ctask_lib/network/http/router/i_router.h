#ifndef I_ROUTER_H
#define I_ROUTER_H

#include "utils/types/types.h"

namespace ctask::network::http::router
{
    namespace Types = utils::types;

    /**
     * @interface IRouter
     * @brief Interface for an HTTP router.
     *
     * Allows registering route handlers and dispatching requests.
     * Basic and minimal — just GET, POST, and route().
     *
     * Created for flexibility and testability — to avoid tight coupling with specific implementations
     * or frameworks. Not a rocket science 🚀
     */
    class IRouter
    {
    public:
        IRouter() = default;
        virtual ~IRouter() = default;

        IRouter(const IRouter&) = delete;
        IRouter& operator=(const IRouter&) = delete;

        IRouter(IRouter&& other) = delete;
        IRouter& operator=(IRouter&& other) = delete;

        /**
         * @brief Registers a handler for an HTTP GET route.
         *
         * @param path The route path (e.g., "/path")
         * @param handler Function to handle the request.
         */
        virtual void addGet(Types::HttpPath path, Types::HttpHandlerFn handler) = 0;

        /**
         * @brief Registers a handler for an HTTP POST route.
         *
         * @param path The route path (e.g., "/path/{event}")
         * @param handler Function to handle the request.
         */
        virtual void addPost(Types::HttpPath path, Types::HttpHandlerFn handler) = 0;

        /**
         * @brief Routes an incoming request to the appropriate handler.
         *
         * @param request The incoming HTTP request.
         * @return HttpResponse The result from the matched handler.
         */
        virtual Types::HttpResponse route(Types::HttpRequest& request) noexcept = 0;
    };
}

#endif //I_ROUTER_H
