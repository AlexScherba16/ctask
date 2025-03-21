#ifdef IGNORE_ONE_INSTANCE_CREATION_POLICY

#include "service/http_server/http_server.h"
#include "mock/mock_router.h"

#include <gtest/gtest.h>

using namespace ctask::service;
using namespace testing;

TEST(ShutdownServerTest, RunServer_Shutdown_BySignal)
{
    io_service io;
    HttpServerArgs args{"127.0.0.1", 8080, 4, 2};
    auto router{std::make_unique<MockRouter>()};
    auto server = HttpServer::сreateService(io, args,
                                            std::move(router));

    signal_set signals(io, SIGINT);
    signals.async_wait([&](const asio::error_code& ec, int signal)
    {
        EXPECT_FALSE(ec);
        EXPECT_EQ(signal, SIGINT);
        server->stop();
    });

    std::atomic_bool stopped{false};
    std::jthread signalThread([&]()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        kill(getpid(), SIGINT);
    });

    std::jthread controller([&]()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        if (stopped.load(std::memory_order::acquire))
        {
            return;
        }
        FAIL() << "Signal timeout failure";
    });

    EXPECT_NO_THROW(server->start());
    stopped.store(true, std::memory_order::release);

    signalThread.join();
    controller.join();
}

TEST(ShutdownServerTest, RunServer_ShutdownBy_StoppingIoContext)
{
    io_service io;
    HttpServerArgs args{"127.0.0.1", 8080, 4,2};
    auto router{std::make_unique<MockRouter>()};
    auto server = HttpServer::сreateService(io, args, std::move(router));

    std::atomic_bool stopped{false};
    std::jthread contextStopThread([&]()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        io.stop();
    });

    std::jthread controller([&]()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        if (stopped.load(std::memory_order::acquire))
        {
            return;
        }
        FAIL() << "Context timeout failure";
    });

    EXPECT_NO_THROW(server->start());
    stopped.store(true, std::memory_order::release);

    contextStopThread.join();
    controller.join();
}

#endif
