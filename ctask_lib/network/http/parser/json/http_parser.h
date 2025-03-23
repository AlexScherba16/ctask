#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "network/http/parser/i_http_parser.h"
#include "utils/types/constants.h"

#include <llhttp.h>
#include <functional>

namespace ctask::network::http::parser
{
    namespace Types = utils::types;
    namespace Constants = utils::constants;

    /**
     * @class JsonHttpParser
     * @brief JSON-based HTTP request parser.
     *
     * This class provides an implementation of IHttpParser that uses the llhttp library
     * to parse raw HTTP request strings into structured HttpRequest objects.
     */
    class JsonHttpParser final : public IHttpParser
    {
    public:
        ~JsonHttpParser() override = default;

        /**
         * @brief Constructs a JsonHttpParser instance
         *
         * perform parser initialization
         */
        JsonHttpParser();

        /**
         * @brief Parses a raw HTTP request into an HttpRequest object.
         *
         * @param rawRequest The raw HTTP request as a string view.
         * @return HttpRequest A structured representation of the request.
         */
        Types::HttpRequest parseRequest(std::string_view rawRequest) override;

    private:
        /**
         * @struct HttpRequestParsingState
         * @brief Internal state used during HTTP request parsing.
         * Using as a helper structure to utilize http headers parsing process.
         *
         * @throws If post parsing request validation failed
         */
        struct HttpRequestParsingState
        {
            Types::HttpRequest request;
            Types::HttpHeaderField currentHeaderField;
            Types::HttpHeaderField currentHeaderValue;
        };

        // third-party http parser and its settings
        llhttp_t parser_;
        llhttp_settings_t settings_;

        /**
         * @brief Callback function for handling the HTTP method.
         *
         * @param parser Pointer to the llhttp parser instance.
         * @param at Pointer to the method string.
         * @param length Length of the method string.
         * @return llhttp status code.
         */
        static int onMethod_(llhttp_t* parser, const char* at, size_t length);

        /**
         * @brief Callback function for handling the URL.
         *
         * @param parser Pointer to the llhttp parser instance.
         * @param at Pointer to the URL string.
         * @param length Length of the URL string.
         * @return llhttp status code.
         */
        static int onUrl_(llhttp_t* parser, const char* at, size_t length);

        /**
         * @brief Callback function for handling the HTTP version.
         *
         * @param parser Pointer to the llhttp parser instance.
         * @param at Pointer to the version string.
         * @param length Length of the version string.
         * @return llhttp status code.
         */
        static int onVersion_(llhttp_t* parser, const char* at, size_t length);

        /**
         * @brief Callback function for handling an HTTP header field.
         *
         * @param parser Pointer to the llhttp parser instance.
         * @param at Pointer to the header field string.
         * @param length Length of the header field string.
         * @return llhttp status code.
         */
        static int onHeaderField_(llhttp_t* parser, const char* at, size_t length);

        /**
         * @brief Callback function for handling an HTTP header value.
         *
         * @param parser Pointer to the llhttp parser instance.
         * @param at Pointer to the header value string.
         * @param length Length of the header value string.
         * @return llhttp status code.
         */
        static int onHeaderValue_(llhttp_t* parser, const char* at, size_t length);

        /**
         * @brief Callback function triggered when a header value is fully parsed.
         *
         * @param parser Pointer to the llhttp parser instance.
         * @return llhttp status code.
         */
        static int onHeaderValueComplete_(llhttp_t* parser);

        /**
         * @brief Callback function for handling the HTTP body.
         *
         * @param parser Pointer to the llhttp parser instance.
         * @param at Pointer to the body string.
         * @param length Length of the body string.
         * @return llhttp status code.
         */
        static int onBody_(llhttp_t* parser, const char* at, size_t length);

        /**
         * @brief Collection of validation functions for parsed HTTP requests.
         *
         * Each function verifies a specific requirement of the HTTP request and
         * throws a runtime error if validation fails.
         */
        using throwIfEmpty = std::function<void(const Types::HttpRequest& r)>;

        std::vector<throwIfEmpty> validators_{
            // validate PATH
            [](const Types::HttpRequest& r) { if (r.path.empty()) throw std::runtime_error("Empty path"); },

            // validate version
            [](const Types::HttpRequest& r) { if (r.version.empty()) throw std::runtime_error("Empty version"); },

            // validate headers
            [](const Types::HttpRequest& r) { if (r.headers.empty()) throw std::runtime_error("Empty headers"); },

            // validate body
            [](const Types::HttpRequest& r)
            {
                if (r.method != Types::HttpMethod::GET_METHOD && r.body.empty()) throw std::runtime_error("Empty body");
            },

            // validate content length header; expected content length and actual length of body
            [](const Types::HttpRequest& r)
            {
                if (r.method == Types::HttpMethod::GET_METHOD || r.method == Types::HttpMethod::UNKNOWN_METHOD) return;
                const auto it{r.headers.find(Constants::CONTENT_LENGTH_HEADER)};
                if (it == r.headers.end()) throw std::runtime_error("Empty content length");
                auto contentLen{static_cast<size_t>(std::stoll(it.operator->()->second))};
                if (contentLen != r.body.size()) throw std::runtime_error("Content length mismatch");
            },

            // validate content type header
            [](const Types::HttpRequest& r)
            {
                if (r.method == Types::HttpMethod::GET_METHOD || r.method == Types::HttpMethod::UNKNOWN_METHOD) return;
                const auto it{r.headers.find(Constants::CONTENT_TYPE_HEADER)};
                if (it == r.headers.end()) throw std::runtime_error("Empty content type");
                if (it->second != Constants::JSON_CONTENT_TYPE)
                    throw std::runtime_error(
                        "Invalid content type, application/json is expected");
            },
        };
    };
}

#endif //JSON_PARSER_H
