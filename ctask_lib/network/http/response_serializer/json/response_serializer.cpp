#include "response_serializer.h"
#include "utils/misc/misc.h"

#include <nlohmann/json.hpp>

#include <format>
#include <sstream>

namespace ctask::network::http::response_serializer
{
    using namespace nlohmann;
    using namespace ctask::utils::misc;

    std::string JsonHttpResponseSerializer::serialize(const HttpResponseMeta& response)
    {
        std::stringstream ss;
        auto message{response.payload.message};
        if (message.empty())
        {
            message = json::object().dump();
        }

        // required response part
        ss << std::format("HTTP/{} ", response.protocolVersion) << httpStatusCodeToString(response.payload.code) << " "
            << httpStatusName(response.payload.code) << "\r\n"
            << "Content-Type: application/json\r\n"
            << "Content-Length: " << message.size() << "\r\n";

        // add user defined headers
        for (const auto& header : response.payload.headers)
        {
            ss << header.first << ": " << header.second << "\r\n";
        }

        // payload and serialize result
        ss << "\r\n" << message;
        return ss.str();
    }
}
