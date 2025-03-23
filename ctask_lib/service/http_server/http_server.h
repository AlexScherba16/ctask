#ifndef SERVER_H
#define SERVER_H

#include "service/i_service.h"
#include "utils/types/types.h"
#include "network/http/router/i_router.h"

#include <asio.hpp>

namespace ctask::service
{
    namespace Types = utils::types;
    namespace Router = network::http::router;


    /**
     * @brief Asynchronous HTTP server.
     *
     * Implements IService, meaning it follows a lifecycle with start/stop.
     * Uses ASIO for async networking and an IRouter for request handling.
     *
     * Highlights:
     * - Fully async, no blocking calls.
     * - Uses IRouter for request dispatching (keeps routing separate).
     * - Designed for scalability (thread pool, keep-alive, coroutine-based handlers).
     * - Inspired by 👇
     *   https://www.youtube.com/watch?v=0i_pFZSijZc
     */
    class HttpServer final : public IService
    {
    public:
        HttpServer() = delete;

        /**
         * @brief Starts the HTTP server.
         *
         * Runs the ASIO event loop, accepting and handling incoming requests.
         *
         * @throws If can't create endpoint
         */
        void start() override;

        /**
         * @brief Stops the HTTP server.
         *
         * Gracefully stops the ASIO event loop.
         */
        void stop() override;
        ~HttpServer() override;

        /**
         * @brief Factory method to create an HttpServer instance.
         *
         * Encapsulates creation logic to ensure correct resource initialization,
         * creates only one instance of HttpServer.
         *
         * @param ctx ASIO IO context.
         * @param serverArgs Configuration arguments (address, port, etc.).
         * @param router Pointer to an IRouter for request handling.
         * @return std::shared_ptr<HttpServer> Instance of HttpServer.
         *
         * @throws If invalid parameters were passed
         */
        static std::shared_ptr<HttpServer> сreateService(asio::io_service& ctx,
                                                         Types::HttpServerArgs serverArgs,
                                                         std::unique_ptr<Router::IRouter> router);

    private:
        /**
         * @brief Private constructor
         *
         * @param ctx ASIO IO context reference.
         * @param address Server address.
         * @param port Server port.
         * @param threads Number of worker threads.
         * @param keepAliveSec Keep-alive timeout in seconds.
         * @param router Router pointer.
         */
        explicit HttpServer(asio::io_service& ctx,
                            std::string address, uint16_t port, size_t threads, uint16_t keepAliveSec,
                            std::unique_ptr<Router::IRouter> router);

        std::reference_wrapper<asio::io_service> ctxRef_;
        std::string address_{0};
        asio::ip::port_type port_{0};
        size_t threads_{0};
        uint16_t keepAliveSec_{0};
        asio::thread_pool threadPool_;
        std::unique_ptr<Router::IRouter> router_{nullptr};

        /**
         * @brief Handles new incoming connections asynchronously.
         *
         * Uses coroutine-based approach (`awaitable<void>`) for efficient async execution.
         *
         * @param ctx IO context reference.
         * @param address Server address.
         * @param port Server port.
         */
        asio::awaitable<void> connectionHandler_(asio::io_context& ctx, const std::string& address,
                                                 asio::ip::port_type port);

        /**
         * @brief Client's session runner.
         *
         * Reads request, delegates to IRouter instance and writes response.
         *
         * @param socket Pointer to the client socket.
         */
        asio::awaitable<void> clientSession_(std::shared_ptr<asio::ip::tcp::socket> socket);
    };
}

#endif //SERVER_H
