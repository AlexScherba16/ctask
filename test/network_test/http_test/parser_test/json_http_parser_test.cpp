#include "network/http/parser/json/http_parser.h"

#include <gtest/gtest.h>

using namespace ctask::network::http::parser;
using namespace testing;

TEST(JsonHttpParserTest, CreateJsonParser_NoThrowsException)
{
    ASSERT_NO_THROW(JsonHttpParser {});
}

TEST(JsonHttpParserTest, ParseRequest_EmptyHttpRequestString_ThrowsException)
{
    auto parser{JsonHttpParser{}};
    ASSERT_THROW(parser.parseRequest(""), std::invalid_argument);
}

TEST(JsonHttpParserTest, ParseRequest_InvalidHttpRequestString_ThrowsException)
{
    auto parser{JsonHttpParser{}};
    std::vector<std::string> invalidRequests{
        "INVALID_METHOD ", // invalid method
        "GET ", // no url
        "GET / ", // no version
        "GET / HTTP/1.1", // no headers
        "GET / HTTP/1.1\r\nHost: 123.4.5.6:7890", // invalid header

        // no body
        "POST / HTTP/1.1\r\nHost: 123.4.5.6:7890\r\nContent-Type: application/json\r\nContent-Length: 123\r\n\r\n",

        // invalid content len
        "POST / HTTP/1.1\r\nHost: 123.4.5.6:7890\r\nContent-Type: application/json\r\nContent-Length: 123\r\n\r\n{}",

        // invalid content type
        "POST / HTTP/1.1\r\nHost: 123.4.5.6:7890\r\nContent-Type: NOT-JSON-CONTENT-TYPE\r\nContent-Length: 2\r\n\r\n{}",
    };

    for (const auto& request : invalidRequests)
    {
        ASSERT_THROW(parser.parseRequest(request), std::runtime_error);
    }
}

TEST(JsonHttpParserTest, ParseRequest_ValidHttpRequestString)
{
    auto parser{JsonHttpParser{}};
    std::vector<std::string> validRequests{
        "GET / HTTP/1.1\r\nHost: 123.4.5.6:7890\r\n\r\n",
        "POST / HTTP/1.1\r\nHost: 123.4.5.6:7890\r\nContent-Type: application/json\r\nContent-Length: 2\r\n\r\n{}",
    };

    for (const auto& request : validRequests)
    {
        ASSERT_NO_THROW(parser.parseRequest(request));
    }
}
