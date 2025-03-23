#include "http_server.h"
#include <utils/misc/misc.h>
#include "network/http/parser/json/http_parser.h"
#include "network/http/response_serializer/json/response_serializer.h"
#include "logger.h"

#include <asio.hpp>

namespace ctask::service
{
    auto log{Logger::instance().getLogger()};

    using namespace asio;
    using namespace asio::ip;
    using namespace ctask::utils::misc;
    using namespace ctask::utils::constants;
    using namespace ctask::network::http::router;
    using namespace ctask::network::http::parser;
    using namespace ctask::network::http::response_serializer;

#ifdef IGNORE_ONE_INSTANCE_CREATION_POLICY
    std::shared_ptr<HttpServer> HttpServer::сreateService(io_service& ctx, HttpServerArgs serverArgs,
                                                          std::unique_ptr<IRouter> router)
    {
        return std::shared_ptr<HttpServer>(new HttpServer{
            ctx,
            std::move(serverArgs.address), serverArgs.port, serverArgs.threads, serverArgs.keepAliveSec,
            std::move(router)
        });
    }
#else
    std::shared_ptr<HttpServer> HttpServer::сreateService(io_service& ctx, HttpServerArgs serverArgs,
                                                          std::unique_ptr<IRouter> router)
    {
        std::shared_ptr<HttpServer> server{nullptr};

        static std::once_flag initHttpServerOnce;
        std::call_once(initHttpServerOnce, [&]()
        {
            server = std::shared_ptr<HttpServer>(new HttpServer{
                ctx, std::move(serverArgs.address), serverArgs.port, serverArgs.threads, serverArgs.keepAliveSec,
                std::move(router)
            });
        });
        return server;
    }
#endif

    HttpServer::HttpServer(io_service& ctx, std::string address, uint16_t port, size_t threads, uint16_t keepAliveSec,
                           std::unique_ptr<IRouter> router) : ctxRef_(ctx),
                                                              address_(std::move(address)), port_(port),
                                                              threads_(threads), keepAliveSec_(keepAliveSec),
                                                              threadPool_(threads_),
                                                              router_(std::move(router))
    {
        if (router_ == nullptr)
        {
            throw std::invalid_argument("Router is nullptr");
        }

        if (keepAliveSec_ == 0)
        {
            throw std::invalid_argument("Keep-Alive must be greater than zero");
        }
    }

    void HttpServer::start()
    {
        std::promise<std::exception_ptr> errorPromise;
        auto errorFuture{errorPromise.get_future()};

        auto listen{connectionHandler_(ctxRef_.get(), address_, port_)};
        co_spawn(ctxRef_.get(), std::move(listen), [&](std::exception_ptr eptr)
        {
            // this running in separate thread
            if (eptr)
            {
                // store exception and shutdown execution context
                errorPromise.set_exception(eptr);
                ctxRef_.get().stop();
            }
        });

        for (size_t i = 0; i < threads_; ++i)
        {
            post(threadPool_, [&]() { ctxRef_.get().run(); });
        }

        threadPool_.join();

        // well, according to mentions in Internet, ChatGPT and finally Marc Gregoire's book,
        // there is no more fancy way how to validate future.
        if (errorFuture.valid() && errorFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
        {
            std::rethrow_exception(errorFuture.get());
        }
    }

    void HttpServer::stop()
    {
        ctxRef_.get().stop();
    }

    HttpServer::~HttpServer()
    {
        stop();
    }

    awaitable<void> HttpServer::clientSession_(std::shared_ptr<tcp::socket> socket)
    {
        log->info("Handle new client");

        std::array<char, 2048> readBuffer{};
        Defer closeSocketOnExit{
            [socket]()
            {
                log->info("Close client's session");
                if (socket->is_open())
                {
                    socket->close();
                }
            }
        };

        auto keepAliveTimer = std::make_shared<steady_timer>(ctxRef_.get());

        // since it's not possible to reset the timer obviously
        // create lambda to reset it and spin up new async waiting ¯\_(ツ)_/¯
        auto resetTimer = [socket, keepAliveTimer, alive = keepAliveSec_]()
        {
            keepAliveTimer->expires_after(std::chrono::seconds(alive));
            keepAliveTimer->async_wait([socket, keepAliveTimer](const error_code& er)
            {
                if (er == error::operation_aborted)
                {
                    return;
                }

                if (!er)
                {
                    log->warn("Keep-alive expired, close socket");
                    socket->close();
                }
            });
        };

        // subscribe time resetter in event loop
        post(ctxRef_.get(), resetTimer);

        // stop timer in case the connection was closed by client
        Defer cancelTimerOnExit{
            [&keepAliveTimer]()
            {
                keepAliveTimer->cancel();
            }
        };

        for (;;)
        {
            JsonHttpParser parser;
            JsonHttpResponseSerializer responseGenerator;

            resetTimer();
            error_code ec;

            // for the sake of simplicity in this test assignment, we assume that the entire request
            // (headers + body) is received in a single read.
            // In a real-world server, we would need to handle buffering, chunked transfer encoding, etc.
            auto size = co_await socket->
                async_read_some(buffer(readBuffer.data(), readBuffer.size()),
                                redirect_error(use_awaitable, ec));

            if (ec)
            {
                if (ec == error::eof)
                {
                    log->debug("Socket closed");
                    co_return;
                }
                log->error("Reading socket error : {}", ec.message());
                co_return;
            }

            size = std::min(size, readBuffer.size());
            if (size == 0)
            {
                log->debug("End of session");
                co_return;
            }

            HttpRequest request;
            bool validRequest{true};
            std::string erroMessage{};
            try
            {
                request = std::move(parser.parseRequest(std::string_view{readBuffer.data(), size}));
            }
            catch (const std::exception& e)
            {
                // can't use async in try/catch
                validRequest = false;
                erroMessage = e.what();
                log->error("Parsing request error : {}", erroMessage);
            }

            if (!validRequest)
            {
                HttpResponseMeta badRequestResp{{HttpStatusCode::HTTP_STATUS_BAD_REQUEST, erroMessage}, "1.1"};
                auto serialized{responseGenerator.serialize(badRequestResp)};
                co_await socket->async_write_some(buffer(serialized),
                                                  redirect_error(use_awaitable, ec));

                if (ec)
                {
                    log->error("Send invalid request, write response error : {}", ec.message());
                }
                co_return;
            }

            resetTimer();

            HttpResponseMeta responseMeta{std::move(router_->route(request)), std::move(request.version)};
            auto serialized{responseGenerator.serialize(responseMeta)};
            co_await socket->async_write_some(buffer(serialized),
                                              redirect_error(use_awaitable, ec));
            if (ec)
            {
                log->error("Send response error : {}", ec.message());
                co_return;
            }

            auto it{request.headers.find(CONNECTION_HEADER)};
            if (it == request.headers.end() || it->second != KEEP_ALIVE_CONNECTION)
            {
                co_return;
            }
        }
    }

    awaitable<void> HttpServer::connectionHandler_(io_context& ctx, const std::string& address, port_type port)
    {
        tcp::endpoint endpoint(address::from_string(address), port);
        tcp::acceptor acceptor{ctx};

        acceptor.open(endpoint.protocol());
        acceptor.set_option(socket_base::reuse_address(true));
        acceptor.bind(endpoint);
        acceptor.listen(socket_base::max_listen_connections);

        log->info("Listening on {}:{}", address, port);
        for (;;)
        {
            try
            {
                // wait for a client's connection
                tcp::socket socket = co_await acceptor.async_accept(use_awaitable);

                // client is connected, run client's session coroutine
                co_spawn(ctx, clientSession_(std::make_shared<tcp::socket>(std::move(socket))), detached);
            }
            catch (const std::exception& e)
            {
                log->error("Connection handler error : {}", e.what());
            }
        }
    }
}
