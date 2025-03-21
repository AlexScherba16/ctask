#ifndef JSON_HTTP_RESPONSE_SERIALIZER_H
#define JSON_HTTP_RESPONSE_SERIALIZER_H

#include "network/http/response_serializer/i_response_serializer.h"

namespace ctask::network::http::response_serializer
{
    /**
     * @class JsonHttpResponseSerializer
     * @brief JSON body HTTP response serializer (JSON-style body, basic headers).
     *
     * Serializes HttpResponseMeta into a string
     * with status line, headers, and body.
     */
    class JsonHttpResponseSerializer final : public IResponseSerializer
    {
    public:
        ~JsonHttpResponseSerializer() override = default;

        /**
         * @brief Serializes the response to raw HTTP format.
         * Includes status line, headers, and body.
         * Body is assumed to be pre-formatted (JSON).
         *
         * @param response Full response to serialize.
         * @return std::string Raw HTTP response string.
         */
        std::string serialize(const HttpResponseMeta& response) override;
    };
}
#endif //JSON_HTTP_RESPONSE_SERIALIZER_H
