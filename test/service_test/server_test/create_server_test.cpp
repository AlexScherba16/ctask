#include "service/http_server/http_server.h"
#include "mock/mock_router.h"

#include <gtest/gtest.h>

using namespace ctask::service;
using namespace testing;

#ifdef IGNORE_ONE_INSTANCE_CREATION_POLICY

TEST(CreateServerTest, CreateServer_WithNullptrRouter_ThrowsException)
{
    io_service io;
    HttpServerArgs args{};
    ASSERT_THROW(HttpServer::сreateService(io, args, nullptr), std::invalid_argument);
}

TEST(CreateServerTest, CreateServer_WithZeroedKeepAlive_ThrowsException)
{
    io_service io;
    HttpServerArgs args{};
    auto router{std::make_unique<MockRouter>()};
    ASSERT_THROW(HttpServer::сreateService(io, args, std::move(router)), std::invalid_argument);
}

TEST(CreateServerTest, CreateServer_Run_InvalidAddress_Run_ThrowsException)
{
    std::vector<std::string> invalidAddresses{
        "", "garbadge", "999.999.999.999"
    };

    for (const auto& address : invalidAddresses)
    {
        io_service io;
        HttpServerArgs args{address, 123, 4, 1};
        auto router{std::make_unique<MockRouter>()};

        auto server = HttpServer::сreateService(io, args, std::move(router));
        ASSERT_THROW(server->start(), std::runtime_error);
    }
}

TEST(CreateServerTest, CreateServer_Run_InvalidPort_ThrowsException)
{
    io_service io;
    HttpServerArgs args{"127.0.0.1", 123, 4,2};
    auto router{std::make_unique<MockRouter>()};

    auto server = HttpServer::сreateService(io, args, std::move(router));
    EXPECT_THROW(server->start(), std::runtime_error);
}

#else

TEST(ServerTest, CreateServer_Twice_SecondTimeReturnsNullptr)
{
    io_service io;
    {
        auto router{std::make_unique<MockRouter>()};
        HttpServerArgs args{"127.0.0.1", 228, 4,2};
        auto server = HttpServer::сreateService(io, args, std::move(router));
        ASSERT_NE(server, nullptr);
    }
    {
        auto router{std::make_unique<MockRouter>()};
        HttpServerArgs args{"127.0.0.1", 228, 4, 2};
        auto server = HttpServer::сreateService(io, args, std::move(router));
        ASSERT_EQ(server, nullptr);
    }
}

#endif
