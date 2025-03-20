#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <unordered_map>

namespace ctask::utils::types
{
    /**
    * @struct CliArgs
    * @brief Structure for storing command-line arguments.
    *
    * This structure is used to store application configurations.
    */
    struct CliArgs
    {
    };

    // Some of these structures might seem excessive, but I added them to keep
    // the code independent of external libraries.
    // Plus I really don’t want to pull in something heavy like Boost just to use an HTTP parser.

    /**
     * @brief Represents an HTTP request method.
     *
     * This enum defines common HTTP methods. It includes an UNKNOWN_METHOD
     * to handle unexpected or unsupported methods.
     */
    enum class HttpMethod : uint8_t
    {
        UNKNOWN_METHOD = 0,
        GET_METHOD,
        POST_METHOD,
    };

    /**
     * @brief Represents an HTTP header field name.
     *
     * Defined as a string alias for clarity.
     */
    using HeaderField = std::string;

    /**
     * @brief Represents an HTTP header field value.
     *
     * Defined as a string alias for clarity.
     */
    using HeaderValue = std::string;

    /**
     * @brief Represents an HTTP request.
     *
     * This structure holds all necessary information about an HTTP request,
     * including the method, URL, version, headers, and body.
     */
    struct HttpRequest
    {
        HttpMethod method{HttpMethod::UNKNOWN_METHOD};
        std::string url{};
        std::string version{};
        std::unordered_map<HeaderField, HeaderValue> headers{};
        std::string body{};
    };
}

#endif //TYPES_H
