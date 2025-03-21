#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <functional>
#include <unordered_map>

namespace ctask::utils::types
{
    /**
    * @struct HttpServerArgs
    * @brief Arguments required to configure and run the HTTP server.
    *
    * Just the basics — nothing fancy.
    * Address to bind, port to listen on, and number of threads to handle requests.
    */
    struct HttpServerArgs
    {
        std::string address;
        uint16_t port;
        size_t threads;
    };

    /**
    * @struct CliArgs
    * @brief Structure for storing command-line arguments.
    *
    * This structure is used to store application configurations.
    */
    struct CliArgs
    {
        HttpServerArgs serverArgs{};
    };

    // Some of these structures might seem excessive, but I added them to keep
    // the code independent of external libraries.
    // Plus I really don’t want to pull in something heavy like Boost just to use an HTTP parser.

    /**
     * @enum HttpMethod
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
     * @brief Human friendly aliases.
     */
    using HttpPath = std::string;
    using HttpHeaderField = std::string;
    using HttpHeaderValue = std::string;
    using ParameterName = std::string;
    using ParameterValue = std::string;

    /**
     * @struct HttpRequest
     * @brief Represents an HTTP request.
     *
     * This structure holds all necessary information about an HTTP request,
     * including the method, path, version, headers, and body.
     */
    struct HttpRequest
    {
        HttpMethod method{HttpMethod::UNKNOWN_METHOD};
        std::string path{};
        std::string version{};
        std::unordered_map<HttpHeaderField, HttpHeaderValue> headers{};
        std::unordered_map<ParameterName, ParameterValue> parameters{};
        std::string body{};
    };

    /**
     * @enum HttpStatusCode
     * @brief Common HTTP status codes.
     */
    enum class HttpStatusCode : uint16_t
    {
        HTTP_STATUS_OK = 200,
        HTTP_STATUS_BAD_REQUEST = 400,
        HTTP_STATUS_NOT_FOUND = 404,
        HTTP_STATUS_INTERNAL_SERVER_ERROR = 500,
        HTTP_STATUS_NOT_IMPLEMENTED = 501,
    };

    /**
     * @struct HttpResponse
     * @brief Final HTTP response to be sent to the client.
     *
     * Will be serialized into raw HTTP format and written to the socket.
     * Minimal on purpose — just status and body for now.
     */
    struct HttpResponse
    {
        HttpStatusCode code{HttpStatusCode::HTTP_STATUS_OK};
        std::string message{""};
    };

    /**
     * @brief Lambda alias for handling HTTP requests.
     */
    using HttpHandlerFn = std::function<HttpResponse (const HttpRequest&)>;
}

#endif //TYPES_H
