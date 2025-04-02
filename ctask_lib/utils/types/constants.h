#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

namespace ctask::utils::constants
{
    constexpr const char* JSON_CONTENT_TYPE{"application/json"};

    constexpr const char* CONTENT_LENGTH_HEADER{"Content-Length"};
    constexpr const char* CONTENT_TYPE_HEADER{"Content-Type"};

    constexpr const char* CONNECTION_HEADER{"Connection"};
    constexpr const char* KEEP_ALIVE_CONNECTION{"Keep-Alive"};
}

#endif //CONSTANTS_H
