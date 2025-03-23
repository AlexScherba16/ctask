#ifndef MOCK_ROUTER_H
#define MOCK_ROUTER_H

#include <gmock/gmock.h>
#include "network/http/router/i_router.h"

namespace ctask::network::http::router
{
    class MockRouter : public IRouter
    {
    public:
        MOCK_METHOD(void, addGet, (HttpPath path, HttpHandlerFn handler), (override));
        MOCK_METHOD(void, addPost, (HttpPath path, HttpHandlerFn handler), (override));
        MOCK_METHOD(HttpResponse, route, (HttpRequest& request), (noexcept, override));
    };
}

#endif //MOCK_ROUTER_H
