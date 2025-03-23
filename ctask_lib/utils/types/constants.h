#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

namespace ctask::utils::constants
{
    constexpr std::string JSON_CONTENT_TYPE{"application/json"};

    constexpr std::string CONTENT_LENGTH_HEADER{"Content-Length"};
    constexpr std::string CONTENT_TYPE_HEADER{"Content-Type"};

    constexpr std::string CONNECTION_HEADER{"Connection"};
    constexpr std::string KEEP_ALIVE_CONNECTION{"Keep-Alive"};
}

#endif //CONSTANTS_H
