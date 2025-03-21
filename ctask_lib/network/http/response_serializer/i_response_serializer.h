#ifndef I_RESPONSE_SERIALIZER_H
#define I_RESPONSE_SERIALIZER_H

#include "utils/types/types.h"

namespace ctask::network::http::response_serializer
{
    namespace Types = utils::types;

    /**
     * @interface IResponseSerializer
     * @brief Interface for HTTP response serialization.
     *
     * Converts HttpResponseMeta data into a raw HTTP string
     * ready to be sent over the wire.
     */
    class IResponseSerializer
    {
    public:
        IResponseSerializer() = default;
        IResponseSerializer(const IResponseSerializer&) = delete;
        IResponseSerializer(IResponseSerializer&&) = delete;
        IResponseSerializer& operator=(const IResponseSerializer&) = delete;
        IResponseSerializer& operator=(IResponseSerializer&&) = delete;
        virtual ~IResponseSerializer() = default;

        /**
         * @brief Serializes the HTTP response into a raw string.
         *
         * @param response Full response metadata (status, headers, body).
         * @return serialized Raw HTTP response (ready to send to client).
         */
        virtual std::string serialize(const Types::HttpResponseMeta& response) = 0;
    };
}
#endif //I_RESPONSE_SERIALIZER_H
