#ifdef IGNORE_ONE_INSTANCE_CREATION_POLICY

#include "service/http_server/http_server.h"
#include "mock/mock_router.h"
#include "utils/types/types.h"
#include "helper.h"

#include <gtest/gtest.h>

using namespace testing;
using namespace asio;
using namespace asio::ip;
using namespace ctask::service;
using namespace ctask::utils::types;
using namespace ctask::network::http::router;


using ClientSession = std::function<void(tcp::socket s)>;

void clientRoutine(io_context& ctx,
                   std::string address, std::string port,
                   ClientSession clientSession)
{
    try
    {
        tcp::resolver resolver(ctx);
        tcp::socket socket(ctx);
        auto endpoint = resolver.resolve(address, port);

        error_code ec;
        for (int i{0}; i < 10; ++i)
        {
            connect(socket, endpoint);
            if (!ec)
            {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        if (ec)
        {
            throw std::runtime_error(std::format("No more attempts to connect, error : {}", ec.message()));
        }
        clientSession(std::move(socket));
    }
    catch (const std::exception& e)
    {
        GTEST_FAIL() << e.what() << std::endl;
    }
}

TEST(HandlingRequestServerTest, HandleRequest_WithInvalidPaiload)
{
    io_service serverCtx;
    const HttpServerArgs args{"127.0.0.1", 8080, 4, 2};
    auto router{std::make_unique<MockRouter>()};

    HttpResponse expectedResponse{HttpStatusCode::HTTP_STATUS_OK, "Hello from test server"};
    auto server = HttpServer::сreateService(serverCtx, args, std::move(router));

    auto clientSession = [&](tcp::socket s)
    {
        std::string request{"Definitely not a request"};
        write(s, buffer(request.data(), request.size()));

        error_code ec;
        std::array<char, 1024> container{};

        s.read_some(buffer(container), ec);
        s.close();
        serverCtx.stop();
    };

    io_context clientCtx;
    std::jthread clientThread(clientRoutine, std::ref(clientCtx), args.address, std::to_string(args.port),
                              clientSession);
    EXPECT_NO_THROW(server->start());
    clientThread.join();
}

TEST(HandlingRequestServerTest, HandleRequest_WithKeepAliveHeader_AndValidPayload)
{
    io_service serverCtx;
    const HttpServerArgs args{"127.0.0.1", 8080, 4, 1};
    HttpResponse expectedResponse{HttpStatusCode::HTTP_STATUS_OK, "Hello from test server"};
    auto router{std::make_unique<MockRouter>()};
    EXPECT_CALL(*router, route(_)).WillOnce(Return(expectedResponse));

    auto server = HttpServer::сreateService(serverCtx, args, std::move(router));
    auto clientSession = [&](tcp::socket s)
    {
        auto request{
            requestGenerator("GET", "/", args.address, std::to_string(args.port),
                             R"({"test": keep_alive_connection})", {{"Connection", "Keep-Alive"}})
        };

        error_code ec;
        auto size = write(s, buffer(request.data(), request.size()), ec);
        if (ec)
        {
            FAIL() << ec.message();
        }

        std::array<char, 1024> container{};
        s.read_some(buffer(container), ec);
        if (ec)
        {
            FAIL() << ec.message();
        }

        // wait until keep-alive expired
        std::this_thread::sleep_for(std::chrono::seconds(args.keepAliveSec * 2));

        // write several times to detect closed socket
        size = 0;
        for (int i = 0; i < 10; ++i)
        {
            size = write(s, buffer(request.data(), request.size()), ec);
            if (ec || size == 0)
            {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        ASSERT_TRUE(ec);

        s.close();
        serverCtx.stop();
    };

    io_context clientCtx;
    std::jthread clientThread(clientRoutine, std::ref(clientCtx), args.address, std::to_string(args.port),
                              clientSession);
    EXPECT_NO_THROW(server->start());
    clientThread.join();
}

TEST(HandlingRequestServerTest, HandleRequest_WithCloseConnectionHeader_ValidPayload)
{
    io_service serverCtx;
    const HttpServerArgs args{"127.0.0.1", 8080, 4, 1};
    HttpResponse expectedResponse{HttpStatusCode::HTTP_STATUS_OK, "Hello from test server"};
    auto router{std::make_unique<MockRouter>()};
    EXPECT_CALL(*router, route(_)).WillOnce(Return(expectedResponse));

    auto server = HttpServer::сreateService(serverCtx, args, std::move(router));
    auto clientSession = [&](tcp::socket s)
    {
        auto request{
            requestGenerator("GET", "/", args.address, std::to_string(args.port),
                             R"({"test": keep_alive_connection})", {{"Connection", "Close"}})
        };

        error_code ec;
        auto size = write(s, buffer(request.data(), request.size()), ec);
        if (ec)
        {
            FAIL() << ec.message();
        }

        std::array<char, 1024> container{};
        s.read_some(buffer(container), ec);
        if (ec)
        {
            FAIL() << ec.message();
        }

        // wait until keep-alive expired
        std::this_thread::sleep_for(std::chrono::seconds(args.keepAliveSec * 2));

        // write several times to detect closed socket
        size = 0;
        for (int i = 0; i < 10; ++i)
        {
            size = write(s, buffer(request.data(), request.size()), ec);
            if (ec || size == 0)
            {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        ASSERT_TRUE(ec);

        s.close();
        serverCtx.stop();
    };

    io_context clientCtx;
    std::jthread clientThread(clientRoutine, std::ref(clientCtx), args.address, std::to_string(args.port),
                              clientSession);
    EXPECT_NO_THROW(server->start());
    clientThread.join();
}

#endif
