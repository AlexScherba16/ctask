#ifndef MOCK_ROUTER_H
#define MOCK_ROUTER_H

#include <gmock/gmock.h>
#include "network/http/router/i_router.h"

namespace ctask::network::http::router
{
    namespace Types = utils::types;
    class MockRouter : public IRouter
    {
    public:
        MOCK_METHOD(void, addGet, (Types::HttpPath path, Types::HttpHandlerFn handler), (override));
        MOCK_METHOD(void, addPost, (Types::HttpPath path, Types::HttpHandlerFn handler), (override));
        MOCK_METHOD(Types::HttpResponse, route, (Types::HttpRequest& request), (noexcept, override));
    };
}

#endif //MOCK_ROUTER_H
