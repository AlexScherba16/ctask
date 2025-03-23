#include "http_parser.h"
#include "utils/misc/misc.h"

#include <format>

namespace ctask::network::http::parser
{
    using namespace utils::types;
    using namespace ctask::utils;

    JsonHttpParser::JsonHttpParser()
    {
        llhttp_settings_init(&settings_);

        // set status parsing handlers
        settings_.on_method = onMethod_;
        settings_.on_url = onUrl_;
        settings_.on_version = onVersion_;

        // set headers parsing handlers
        settings_.on_header_field = onHeaderField_;
        settings_.on_header_value = onHeaderValue_;
        settings_.on_header_value_complete = onHeaderValueComplete_;

        // set body parsing handlers
        settings_.on_body = onBody_;

        llhttp_init(&parser_, HTTP_REQUEST, &settings_);
    }

    HttpRequest JsonHttpParser::parseRequest(std::string_view rawRequest)
    {
        // I have no idea why, but without initialization, the next parsing process returns
        // an empty request structure... well, to avoid this, let's prepare deferred initialization.
        // Maybe llhttp_reset isn't needed, but hey — if we can initialize something,
        // we should reset something too, lol =D
        misc::Defer rechargeParserOnExit{
            [this]()
            {
                llhttp_reset(&parser_);
                llhttp_init(&parser_, HTTP_REQUEST, &settings_);
            }
        };

        if (rawRequest.empty())
        {
            throw std::invalid_argument("Empty request string");
        }

        // prepare helper contained structure for parsing process
        HttpRequestParsingState helper;
        parser_.data = &helper;

        // run parsing
        auto err{llhttp_execute(&parser_, rawRequest.data(), rawRequest.size())};
        if (err != HPE_OK)
        {
            throw std::runtime_error(std::format("Invalid request string : \"{}\", err : \"{}\"", rawRequest,
                                                 llhttp_errno_name(err)));
        }

        // final validations
        for (auto& throwIfInvalid : validators_)
        {
            throwIfInvalid(helper.request);
        }

        // Done, fresh request is ready to use
        return helper.request;
    }

    // callback handlers implementation
    int JsonHttpParser::onMethod_(llhttp_t* parser, const char* at, size_t length)
    {
        auto state{static_cast<HttpRequestParsingState*>(parser->data)};
        state->request.method = misc::httpMethodFromString(std::string_view(at, length));
        return HPE_OK;
    }

    int JsonHttpParser::onUrl_(llhttp_t* parser, const char* at, size_t length)
    {
        auto state{static_cast<HttpRequestParsingState*>(parser->data)};
        state->request.path.assign(at, length);
        return HPE_OK;
    }

    int JsonHttpParser::onVersion_(llhttp_t* parser, const char* at, size_t length)
    {
        auto state{static_cast<HttpRequestParsingState*>(parser->data)};
        state->request.version.assign(at, length);
        return HPE_OK;
    }

    int JsonHttpParser::onHeaderField_(llhttp_t* parser, const char* at, size_t length)
    {
        auto state{static_cast<HttpRequestParsingState*>(parser->data)};
        state->currentHeaderField.assign(at, length);
        return HPE_OK;
    }

    int JsonHttpParser::onHeaderValue_(llhttp_t* parser, const char* at, size_t length)
    {
        auto state{static_cast<HttpRequestParsingState*>(parser->data)};
        state->currentHeaderValue.assign(at, length);
        return HPE_OK;
    }

    int JsonHttpParser::onHeaderValueComplete_(llhttp_t* parser)
    {
        auto state{static_cast<HttpRequestParsingState*>(parser->data)};
        if (state->currentHeaderField.empty() || state->currentHeaderValue.empty())
        {
            return HPE_INVALID_HEADER_TOKEN;
        }
        state->request.headers.emplace(std::move(state->currentHeaderField), std::move(state->currentHeaderValue));
        return HPE_OK;
    }

    int JsonHttpParser::onBody_(llhttp_t* parser, const char* at, size_t length)
    {
        auto state{static_cast<HttpRequestParsingState*>(parser->data)};
        state->request.body.assign(at, length);
        return HPE_OK;
    }
}
