#include "router_builder.h"
#include "network/http/router/custom/router.h"

namespace ctask::network::http::router
{
    using namespace utils::types;

    RouterBuilder& RouterBuilder::registerGet(HttpPath path, HttpHandlerFn handler)
    {
        getHandlers_.emplace(std::move(path), std::move(handler));
        return *this;
    }

    RouterBuilder& RouterBuilder::registerPost(HttpPath path, HttpHandlerFn handler)
    {
        postHandlers_.emplace(std::move(path), std::move(handler));
        return *this;
    }

    std::unique_ptr<IRouter> RouterBuilder::build()
    {
        std::unique_ptr<HttpRouter> router(new HttpRouter());
        for (const auto& [path, handler] : getHandlers_)
        {
            router->addGet(path, handler);
        }
        for (const auto& [path, handler] : postHandlers_)
        {
            router->addPost(path, handler);
        }
        return router;
    }
}
