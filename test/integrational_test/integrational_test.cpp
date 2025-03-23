#ifdef IGNORE_ONE_INSTANCE_CREATION_POLICY

#include "service/http_server/http_server.h"
#include "utils/types/types.h"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>


#include "telemetry/core/models.h"
#include "telemetry/dto/dto.h"
#include "telemetry/core/telemetry_storage.h"

#include "network/http/router/router_builder.h"
#include "telemetry/api/routes.h"

#include <random>

#include <latch>
#include "helper.h"

using namespace ctask::service;
using namespace ctask::telemetry::api;
using namespace ctask::network::http::router;
using namespace ctask::telemetry;

using namespace nlohmann;
using namespace testing;

std::string requestGenerator(
    std::string method, std::string path, std::string address,
    std::string port, std::string payload)
{
    std::stringstream ss;
    ss << method << " " << path << " " << " HTTP/1.1\r\n"
        << "Host: " << address << ":" << port << "\r\n"
        << "Content-Type: application/json\r\n"
        << "Content-Length: " << payload.size() << "\r\n";
    ss << "\r\n" << payload;
    return ss.str();
}

TEST(WholeApp, RunApp)
{
    io_context serverCtx;
    const HttpServerArgs args{"127.0.0.1", 8080, 4, 2};

    RouterBuilder routBuilder;
    TelemetryRoutes::registerRoutes(routBuilder,
                                    std::make_shared<core::TelemetryStorage>());
    auto server{
        HttpServer::сreateService(serverCtx,
                                  args,
                                  routBuilder.build())
    };

    const std::vector<std::string> postEvents{
        "/paths/open",
        "/paths/register",
        "/paths/click",
        "/paths/select",
        "/paths/close",
        "/paths/delete",
    };

    struct PostEventData
    {
        std::string path;
        uint64_t date{};
        std::array<uint32_t, 10> values{};
    };


    const uint32_t dataLen{25000};
    const uint32_t postThreads{2};

    std::latch latch{postThreads}; // + getThreads};

    std::vector<std::vector<PostEventData>> postEventsDataSet{};
    postEventsDataSet.reserve(postThreads);

    for (uint32_t i = 0; i < postThreads; ++i)
    {
        std::vector<PostEventData> threadEventData{};
        threadEventData.reserve(dataLen);

        std::mt19937 gen;
        std::uniform_int_distribution<uint32_t> eventNameDistribution(0, postEvents.size() - 1);
        std::uniform_int_distribution<uint32_t> timestampDistibution(0, 10 * 1'000'000);
        uint32_t startTimestamp{i * dataLen};

        for (uint32_t j = startTimestamp; j < startTimestamp + dataLen; ++j)
        {
            std::array<uint32_t, 10> values{};
            values.fill(j);
            threadEventData.emplace_back(postEvents[eventNameDistribution(gen)], timestampDistibution(gen), values);
        }
        postEventsDataSet.emplace_back(std::move(threadEventData));
    }

    auto awaiter = [&]()
    {
        latch.wait();
        serverCtx.stop();
    };

    io_context ctx;
    auto postThreadRunner = [&](std::vector<PostEventData>& postEvents)
    {
        tcp::resolver resolver(ctx);
        tcp::socket socket(ctx);
        auto endpoint = resolver.resolve(args.address, std::to_string(args.port));

        try
        {
            error_code ec;
            for (int i{0}; i < 10; ++i)
            {
                connect(socket, endpoint, ec);
                if (!ec)
                {
                    break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            for (const auto& postEvent : postEvents)
            {
                json event;
                event["values"] = postEvent.values;
                event["date"] = postEvent.date;

                auto postEventJson{event.dump()};
                postEventJson = requestGenerator("POST", postEvent.path, args.address, std::to_string(args.port),
                                                 postEventJson, {{"Connection", "Keep-Alive"}});

                write(socket, buffer(postEventJson.data(), postEventJson.size()));

                std::string response;
                char container[1024];

                size_t len = socket.read_some(buffer(container), ec);
                response.append(container, len);
                // socket.close();
            }
            socket.close();
            latch.count_down();
        }
        catch (const std::exception& e)
        {
            FAIL() << e.what() << std::endl;
        }
    };


    std::vector<std::jthread> posters{};
    posters.reserve(postThreads);
    for (int i = 0; i < postThreads; ++i)
    {
        posters.emplace_back(postThreadRunner, std::ref(postEventsDataSet[i]));
    }

    std::jthread awiterThread{awaiter};

    auto START = std::chrono::steady_clock::now();
    server->start();
    auto END = std::chrono::steady_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::seconds>(END - START);
    std::cout << "DURATION : " << duration.count() << " sec" << std::endl;


    for (auto& p : posters)
    {
        p.join();
    }
    awiterThread.join();
}

#endif
