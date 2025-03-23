#ifndef HELPER_H
#define HELPER_H

#include "utils/types/types.h"

#include <sstream>

struct Header
{
    ctask::utils::types::HttpHeaderField field;
    ctask::utils::types::HttpHeaderValue value;
};

inline std::string requestGenerator(
    std::string_view method, std::string_view path, std::string_view address,
    std::string_view port, std::string_view payload, const std::vector<Header>& headers = {})
{
    std::stringstream ss;
    ss << method << " " << path << " " << " HTTP/1.1\r\n"
        << "Host: " << address << ":" << port << "\r\n"
        << "Content-Type: application/json\r\n"
        << "Content-Length: " << payload.size() << "\r\n";

    for (const auto& header : headers)
    {
        ss << header.field << ": " << header.value << "\r\n";
    }

    ss << "\r\n" << payload;
    return ss.str();
};


#endif //HELPER_H
