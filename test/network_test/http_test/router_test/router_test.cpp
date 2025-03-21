#include "network/http/router/router.h"

#include <gtest/gtest.h>

using namespace ctask::network::http::router;
using namespace testing;

TEST(RouterTest, CreateRouter_RegisterSamePath_GetHandler_ThrowsException)
{
    std::vector<std::string> pathes{
        "/get_path",
        "/get_path/{event}",
    };

    for (const auto& path : pathes)
    {
        HttpRouter router;
        EXPECT_NO_THROW(router.addGet(path, [&](const HttpRequest& req) { return HttpResponse{}; }));
        EXPECT_THROW(router.addGet(path, [&](const HttpRequest& req) { int a {123}; return HttpResponse{}; }),
                     std::invalid_argument);
    }
}

TEST(RouterTest, CreateRouter_RegisterSamePath_PostHandler_ThrowsException)
{
    std::vector<std::string> pathes{
        "/post_path",
        "/post_path/{event}",
    };

    for (const auto& path : pathes)
    {
        HttpRouter router;
        EXPECT_NO_THROW(router.addPost(path, [&](const HttpRequest& req) { return HttpResponse{}; }));
        EXPECT_THROW(router.addPost(path, [&](const HttpRequest& req) { int a {123}; return HttpResponse{}; }),
                     std::invalid_argument);
    }
}

TEST(RouterTest, Routing_UnregisteredHandler_ReturnsNotFoundStatus)
{
    HttpRouter router;
    std::vector<HttpRequest> requests{
        {HttpMethod::GET_METHOD, "/path"},
        {HttpMethod::POST_METHOD, "/path"},
        {HttpMethod::UNKNOWN_METHOD, "/path"},
    };
    for (auto& request : requests)
    {
        ASSERT_EQ(router.route(request).code, HttpStatusCode::HTTP_STATUS_NOT_FOUND);
    }
}

TEST(RouterTest, Routing_ThrownExceptionByHandler_ReturnsInternalServerErrorStatus)
{
    std::vector<std::string> pathes{
        "/path",
        "/path/{event}",
    };

    auto throwHandler{
        [&](const HttpRequest& req)
        {
            throw std::runtime_error("Exploded");
            return HttpResponse{};
        }
    };
    for (const auto& path : pathes)
    {
        HttpRouter router;
        EXPECT_NO_THROW(router.addPost(path, throwHandler));
        EXPECT_NO_THROW(router.addGet(path, throwHandler));

        {
            HttpRequest postReq{HttpMethod::POST_METHOD, path};
            auto response{router.route(postReq)};
            ASSERT_EQ(response.code, HttpStatusCode::HTTP_STATUS_INTERNAL_SERVER_ERROR);
        }
        {
            HttpRequest getReq{HttpMethod::GET_METHOD, path};
            auto response{router.route(getReq)};
            ASSERT_EQ(response.code, HttpStatusCode::HTTP_STATUS_INTERNAL_SERVER_ERROR);
        }
    }
}

TEST(RouterTest, Routing_HandleUnsupportedMethod_ReturnsNotImplemented)
{
    std::vector<std::string> pathes{
        "/path",
        "/path/{event}",
    };

    for (const auto& path : pathes)
    {
        HttpRouter router;
        HttpRequest request{HttpMethod::UNKNOWN_METHOD, path};
        EXPECT_NO_THROW(router.addPost(path, [&](const HttpRequest& req) { return HttpResponse{}; }));
        EXPECT_NO_THROW(router.addGet(path, [&](const HttpRequest& req) { return HttpResponse{}; }));
        auto response{router.route(request)};
        ASSERT_EQ(response.code, HttpStatusCode::HTTP_STATUS_NOT_IMPLEMENTED);
    }
}

TEST(RouterTest, Route_ParseParameterizedRequest)
{
    std::string parameterizedGetPath{"/path/{get_event}/get/Hello/{get_time}/{get_date}"};
    std::string parameterizedPostPath{"/path/{post_event}/P0St_something/{post_time}/{post_date}"};

    std::unordered_map<ParameterName, ParameterValue> expectedGetParams{
        {"get_event", "GET_EVENT_PARAM"},
        {"get_time", "GET_TIME_PARAM"},
        {"get_date", "GET_DATE_PARAM"},
    };

    std::unordered_map<ParameterName, ParameterValue> expectedPostParams{
        {"post_event", "POST_EVENT_PARAM"},
        {"post_time", "POST_TIME_PARAM"},
        {"post_date", "POST_DATE_PARAM"},
    };

    std::unordered_map<ParameterName, ParameterValue> actualGetParams{};
    std::unordered_map<ParameterName, ParameterValue> actualPostParams{};

    HttpRouter router;
    auto getHandler{
        [&](const HttpRequest& req)
        {
            actualGetParams = req.parameters;
            return HttpResponse{HttpStatusCode::HTTP_STATUS_OK};
        }
    };

    auto postHandler{
        [&](const HttpRequest& req)
        {
            actualPostParams = req.parameters;
            return HttpResponse{HttpStatusCode::HTTP_STATUS_OK};
        }
    };

    EXPECT_NO_THROW(router.addGet(parameterizedGetPath, getHandler));
    EXPECT_NO_THROW(router.addPost(parameterizedPostPath, postHandler));

    {
        HttpRequest getRequest{HttpMethod::GET_METHOD, "/path/GET_EVENT_PARAM/get/Hello/GET_TIME_PARAM/GET_DATE_PARAM"};
        auto response{router.route(getRequest)};
        ASSERT_EQ(actualGetParams, expectedGetParams);
        ASSERT_EQ(response.code, HttpStatusCode::HTTP_STATUS_OK);
    }
    {
        HttpRequest postRequest{
            HttpMethod::POST_METHOD, "/path/POST_EVENT_PARAM/P0St_something/POST_TIME_PARAM/POST_DATE_PARAM"
        };
        auto response{router.route(postRequest)};
        ASSERT_EQ(actualPostParams, expectedPostParams);
        ASSERT_EQ(response.code, HttpStatusCode::HTTP_STATUS_OK);
    }
}

struct RouterTestSuite
{
    std::string path{""};
    uint16_t actualCalls{0};
    uint16_t expectedCalls{0};
};

TEST(RouterTest, CreateRouter_RegisterPostHandlers)
{
    HttpRouter router;

    std::vector<RouterTestSuite> postRouteSuites{
        {"/path/{event}", 0, 1},
        {"/path", 0, 1},
        {"/", 0, 1},
    };

    for (auto& postSuite : postRouteSuites)
    {
        router.addPost(postSuite.path, [&](const HttpRequest& req)
        {
            ++postSuite.actualCalls;
            return HttpResponse{};
        });
    }

    for (auto& postSuite : postRouteSuites)
    {
        HttpRequest req{HttpMethod::POST_METHOD, postSuite.path};
        router.route(req);
        EXPECT_EQ(postSuite.actualCalls, postSuite.expectedCalls);
    }
}
