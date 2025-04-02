#include "router.h"

#include <format>
#include <sstream>

namespace ctask::network::http::router
{
    using namespace utils::types;

    void HttpRouter::addGet(HttpPath path, HttpHandlerFn handler)
    {
        registerHandler_(std::move(path), std::move(handler), getHandlers_);
    }

    void HttpRouter::addPost(HttpPath path, HttpHandlerFn handler)
    {
        registerHandler_(std::move(path), std::move(handler), postHandlers_);
    }

    HttpResponse HttpRouter::route(HttpRequest& request) noexcept
    {
        try
        {
            switch (request.method)
            {
            case HttpMethod::GET_METHOD:
                {
                    return processRouting_(request, getHandlers_);
                }
            case HttpMethod::POST_METHOD:
                {
                    return processRouting_(request, postHandlers_);
                }
            default:
                return HttpResponse{HttpStatusCode::HTTP_STATUS_NOT_IMPLEMENTED, "Method is not implemented"};
            }
        }
        catch (const std::exception& e)
        {
            return HttpResponse{HttpStatusCode::HTTP_STATUS_INTERNAL_SERVER_ERROR, e.what()};
        }
    }

    void HttpRouter::registerHandler_(HttpPath path, HttpHandlerFn handler, pathHandlerMap& map)
    {
        if (path.find('{') == std::string::npos)
        {
            // direct path
            auto it{map.find(path)};
            if (it != map.end())
            {
                throw std::invalid_argument(std::format("{}, handler already registered", path));
            }
            map.emplace_hint(it, std::move(path), std::move(handler));
        }
        else
        {
            // parameterized path
            auto parameterNamesWithPositions{parseParameterNames_(path)};
            auto pathTemplate{buildParameterizedPathTemplate_(path)};
            auto it{map.find(pathTemplate)};
            if (it != map.end())
            {
                throw std::invalid_argument(std::format("{}, handler already registered", path));
            }

            map.emplace_hint(it, std::move(pathTemplate), std::move(handler));

            RouteParameterInfo info{std::move(path), std::move(parameterNamesWithPositions)};
            pathParametersInfo_.emplace_back(std::move(info));
        }
    }

    HttpResponse HttpRouter::processRouting_(HttpRequest& request, pathHandlerMap& handlersMap)
    {
        // try to find direct path and handle request
        if (auto it{handlersMap.find(request.path)}; it != handlersMap.end())
        {
            return it->second(request);
        }

        // try to find parameterizedPath and handle request
        for (const auto& [parameterizedPath, paramsMeta] : pathParametersInfo_)
        {
            auto pathTemplate{buildParameterizedPathTemplate_(parameterizedPath)};

            if (auto it = handlersMap.find(pathTemplate); it != handlersMap.end())
            {
                auto parametersMap{parseParameters_(request.path, paramsMeta)};
                request.parameters = std::move(parametersMap);
                return it->second(request);
            }
        }

        return HttpResponse{
            HttpStatusCode::HTTP_STATUS_NOT_FOUND, std::format("Path is not found : {}", request.path)
        };
    }

    std::vector<HttpRouter::ParameterMetaData> HttpRouter::parseParameterNames_(std::string_view path) const
    {
        size_t pathEndPos{0};
        size_t pathStartPos{0};
        size_t parameterPosition{0};
        std::vector<ParameterMetaData> result{};

        while (pathStartPos < path.size())
        {
            pathEndPos = path.find('/', pathStartPos);
            if (pathEndPos == std::string::npos)
            {
                pathEndPos = path.size();
            }
            if (pathEndPos != pathStartPos)
            {
                auto part{path.substr(pathStartPos, pathEndPos - pathStartPos)};

                // parameter name detected
                if (part.front() == '{' && part.back() == '}')
                {
                    ParameterMetaData nameData{std::string{part.substr(1, part.size() - 2)}, parameterPosition};
                    result.emplace_back(std::move(nameData));
                }
                ++parameterPosition;
            }

            pathStartPos = pathEndPos + 1;
        }

        return result;
    }

    HttpPath HttpRouter::buildParameterizedPathTemplate_(std::string_view path) const
    {
        size_t pathEndPos{0};
        size_t pathStartPos{0};
        std::stringstream ss;

        while (pathStartPos < path.size())
        {
            pathEndPos = path.find('/', pathStartPos);
            if (pathEndPos == std::string::npos)
            {
                pathEndPos = path.size();
            }
            if (pathEndPos != pathStartPos)
            {
                auto part{path.substr(pathStartPos, pathEndPos - pathStartPos)};
                ss << ((part.front() == '{' && part.back() == '}') ? "{}" : part);
                if (pathEndPos == path.size())
                {
                    break;
                }
            }
            ss << "/";
            pathStartPos = pathEndPos + 1;
        }
        return ss.str();
    }

    std::unordered_map<ParameterName, ParameterValue> HttpRouter::parseParameters_(
        std::string_view path, const std::vector<ParameterMetaData>& paramMeta) const
    {
        std::vector<std::string_view> pathParts;
        pathParts.reserve(16);

        // split path, collect parts
        {
            size_t startPos{0};
            size_t endPos{0};
            while (startPos < path.size())
            {
                endPos = path.find('/', startPos);
                if (endPos == std::string::npos)
                {
                    endPos = path.size();
                }

                if (endPos != startPos)
                {
                    pathParts.emplace_back(path.substr(startPos, endPos - startPos));
                }
                startPos = endPos + 1;
            }
        }

        std::unordered_map<ParameterName, ParameterValue> result;
        for (const auto& [parameterName, position] : paramMeta)
        {
            if (position >= pathParts.size())
            {
                break;
            }
            result[parameterName] = pathParts[position];
        }
        return result;
    }
}
