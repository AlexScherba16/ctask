#ifndef ROUTER_H
#define ROUTER_H

#include "network/http/router/i_router.h"

namespace ctask::network::http::router
{
    namespace Types = utils::types;

    /**
     * @class HttpRouter
     * @brief HTTP router for handling GET and POST requests with support for parameterized paths.
     *
     * The router maps routes to handler functions and supports parameter extraction
     * similar to lightweight web frameworks.
     */
    class HttpRouter final : public IRouter
    {
    public:
        HttpRouter() = default;
        ~HttpRouter() override = default;

        /**
          * @brief Registers a handler for an HTTP GET path.
          *
          * @param path The path for which to register the handler.
          * @param handler The function to handle the request.
          *
          * @throws If path already registered
          */
        void addGet(Types::HttpPath path, Types::HttpHandlerFn handler) override;

        /**
          * @brief Registers a handler for an HTTP POST path.
          *
          * @param path The path for which to register the handler.
          * @param handler The function to handle the request.
          *
          * @throws If path already registered
          */
        void addPost(Types::HttpPath path, Types::HttpHandlerFn handler) override;


        /**
         * @brief Routes the request to the appropriate handler based on method and path.
         *
         * Passed by non-const reference, because we need to prepare parameters,
         * in case we're dealing with a parameterized path.
         *
         * Only the router knows the parameter template of a registered route.
         * Yeah, sure, technically it’s possible to resolve this on some third-party level,
         * but I *really* don’t want to allow this responsibility to leak into the handler.
         *
         * So we extract and inject parameters right here — where they belong.
         *
         * @param request Incoming HTTP request.
         * @return HttpResponse The result of the matched handler.
         */
        Types::HttpResponse route(Types::HttpRequest& request) noexcept override;

    private:
        using pathHandlerMap = std::unordered_map<Types::HttpPath, Types::HttpHandlerFn>;
        pathHandlerMap getHandlers_;
        pathHandlerMap postHandlers_;

        /**
         * @brief Registers a handler in the specified map.
         *
         * Also parses and stores parameter metadata if the path is parameterized.
         *
         * @param path The route path.
         * @param handler The request handler function.
         * @param map The map (GET or POST) where the handler is stored.
         *
         * @throws If path already registered
         */
        void registerHandler_(Types::HttpPath path, Types::HttpHandlerFn handler,
                              pathHandlerMap& map);

        /**
         * @brief Internal routing logic for a given method.
         *
         * Attempts to match the request path to a registered handler.
         * If the route is parameterized, parameters are extracted.
         *
         * @param request The request to route.
         * @param handlersMap The map of handlers (GET or POST).
         * @return HttpResponse The result of the matched handler.
         */
        Types::HttpResponse processRouting_(Types::HttpRequest& request, pathHandlerMap& handlersMap);


        // parameter positions and helper methods to operate with parameterized pathes,
        // inspired by this approach 👇in more simple manner, of course =)
        // https://github.com/pistacheio/pistache/blob/ea6b7dd0dffdf0dbce1e87dfbbde9d94fc7f2851/src/server/router.cc#L526

        /**
         * @struct ParameterMetaData
         * @brief Metadata describing a single path parameter.
         */
        struct ParameterMetaData
        {
            Types::ParameterName name{};
            size_t positionInPath{0};
        };

        /**
         * @struct RouteParameterInfo
         * @brief Information about a registered parameterized route.
         */
        struct RouteParameterInfo
        {
            Types::HttpPath parameterizedPath;
            std::vector<ParameterMetaData> parametersMeta;
        };

        std::vector<RouteParameterInfo> pathParametersInfo_;

        /**
         * @brief Converts a concrete route into a parameterized path template.
         *
         * Example: "/path/{event}/more_data/{data}" → "/path/{}/more_data/{}"
         *
         * Uses a simple and straightforward approach to recognize parameterized routes
         * like "/path/{event}" and convert them into a generalized form like "/path/{}".
         *
         * It's not the most efficient or flexible solution based on tries out there, but it's
         * dead simple to implement and gets the job done :)
         *
         * The resulting template (e.g., "/path/{}") becomes the key used to look up
         * the appropriate handler for real paths like "/path/open", "/path/close", etc.
         *
         * @param path The original path string (may contain parameters like {id}).
         * @return HttpPath The normalized path template.
         */
        Types::HttpPath buildParameterizedPathTemplate_(std::string_view path) const;

        /**
         * @brief Parses parameter values from the actual path.
         *
         * Matches path segments against metadata to extract values.
         *
         * @param path The actual request path.
         * @param paramMeta Parameter positions and names.
         * @return Map of parameter names to values.
         */
        std::unordered_map<Types::ParameterName, Types::ParameterValue> parseParameters_(std::string_view path,
            const std::vector<ParameterMetaData>&
            paramMeta)
        const;

        /**
         * @brief Parses parameter names from a parameterized route.
         *
         * Example: "/user/{id}/post/{postId}" → [ {name="id", positionInPath=1}, {name="postId", positionInPath=3} ]
         *
         * @param path The route containing parameter placeholders.
         * @return Vector of parameter metadata.
         */
        std::vector<ParameterMetaData> parseParameterNames_(std::string_view path) const;
    };
}

#endif //ROUTER_H
