#include "network/http/response_serializer/json/response_serializer.h"
#include "utils/types/types.h"

#include <gtest/gtest.h>

using namespace ctask::network::http::response_serializer;
using namespace ctask::utils::types;
using namespace testing;


bool contains(const std::string& str, const std::string& substr)
{
    return str.find(substr) != std::string::npos;
}

TEST(JsonHttpResponseSerializerTest, GenerateResponse_Base)
{
    HttpResponse response{HttpStatusCode::HTTP_STATUS_OK, R"({"message":"Hello, world!"})"};
    HttpResponseMeta meta{response, "1.1"};
    JsonHttpResponseSerializer generator;

    auto serialized{generator.serialize(meta)};
    EXPECT_TRUE(contains(serialized, "HTTP/1.1 200 OK"));
    EXPECT_TRUE(contains(serialized, "Content-Type: application/json"));
    EXPECT_TRUE(contains(serialized, "Content-Length: 27")); // длина JSON
    EXPECT_TRUE(contains(serialized, response.message));
}

TEST(JsonHttpResponseSerializerTest, GenerateResponse_WithCustomHeaders)
{
    HttpResponse response{
        HttpStatusCode::HTTP_STATUS_OK, R"({"message":"Hello, world!"})",
        {
            {"CustomHeader", "CustomValue"},
            {"AdvancedHeader", "DavancedValue"},
            {"LOL-HE_ader", "No-Comments"},
        }
    };
    HttpResponseMeta meta{response, "1.1"};
    JsonHttpResponseSerializer generator;

    std::string serialized = generator.serialize(meta);
    EXPECT_TRUE(contains(serialized, "CustomHeader: CustomValue"));
    EXPECT_TRUE(contains(serialized, "AdvancedHeader: DavancedValue"));
    EXPECT_TRUE(contains(serialized, "LOL-HE_ader: No-Comments"));
    EXPECT_TRUE(contains(serialized, response.message));
}

TEST(JsonHttpResponseSerializerTest, GenerateResponse_WithHttpVersion_2_0)
{
    HttpResponse response{HttpStatusCode::HTTP_STATUS_OK};
    HttpResponseMeta meta{response, "2.0"};
    JsonHttpResponseSerializer generator;

    auto serialized{generator.serialize(meta)};
    EXPECT_TRUE(contains(serialized, "HTTP/2.0 200 OK"));
    EXPECT_TRUE(contains(serialized, response.message));
}

TEST(JsonHttpResponseSerializerTest, GenerateResponse_WithNotOkCode)
{
    HttpResponseMeta meta{{HttpStatusCode::HTTP_STATUS_INTERNAL_SERVER_ERROR}};
    JsonHttpResponseSerializer generator;
    std::string serialized = generator.serialize(meta);
    EXPECT_TRUE(contains(serialized, "HTTP/ 500 INTERNAL_SERVER_ERROR"));
}

TEST(JsonHttpResponseSerializerTest, GenerateResponse_WithEmptyPayload)
{
    HttpResponseMeta meta{};
    JsonHttpResponseSerializer generator;
    auto serialized{generator.serialize(meta)};
    EXPECT_TRUE(contains(serialized, "{}"));
}
