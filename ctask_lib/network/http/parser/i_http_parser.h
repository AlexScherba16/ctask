#ifndef I_PARSER_H
#define I_PARSER_H

#include "utils/types/types.h"

namespace ctask::network::http::parser
{
    using namespace ctask::utils::types;

    /**
     * @interface IHttpParser
     * @brief Interface for an HTTP request parser.
     *
     * This interface defines a contract for parsing raw HTTP request strings
     * into structured HttpRequest.
     */
    class IHttpParser
    {
    public:
        IHttpParser() = default;
        IHttpParser(const IHttpParser&) = delete;
        IHttpParser(IHttpParser&&) = delete;
        IHttpParser& operator=(const IHttpParser&) = delete;
        IHttpParser& operator=(IHttpParser&&) = delete;
        virtual ~IHttpParser() = default;

        /**
         * @brief Parses a raw HTTP request string into an HttpRequest object.
         *
         * This method should be implemented by subclasses to provide
         * the actual parsing logic.
         *
         * @param rawRequest The raw HTTP request as a string view.
         * @return HttpRequest A parsed representation of the request.
         */
        virtual HttpRequest parseRequest(std::string_view rawRequest) = 0;
    };
}
#endif //I_PARSER_H
