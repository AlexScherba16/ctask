#include "routes.h"
#include "telemetry/dto/dto.h"
#include "utils/types/types.h"
#include "telemetry/core/misc.h"
#include "telemetry/core/models.h"
#include "telemetry/core/telemetry_storage.h"
#include "network/http/router/router_builder.h"

#include <nlohmann/json.hpp>

#include "logger.h"

namespace ctask::telemetry::api
{
    using namespace nlohmann;
    using namespace ctask::telemetry;
    using namespace ctask::utils::types;
    using namespace ctask::network::http::router;

    auto log{Logger::instance().getLogger()};

    void TelemetryRoutes::registerRoutes(RouterBuilder& builder, std::shared_ptr<core::TelemetryStorage> storage)
    {
        builder.registerGet("/paths/{event}/meanLength", [storage](const HttpRequest& req)
        {
            try
            {
                log->debug(std::format("Handle path : {}, body : {}", req.path, req.body));

                auto meanLenDto{json::parse(req.body).get<dto::MeanLengthQueryDto>()};
                auto it{req.parameters.find("event")};
                if (it == req.parameters.end())
                {
                    return HttpResponse{
                        HttpStatusCode::HTTP_STATUS_BAD_REQUEST,
                        json{{"error", "No event name"}}.dump()
                    };
                }

                core::MeanLengthQueryModel model{
                    core::parseTimeUnit(meanLenDto.resultUnit),
                    meanLenDto.startTimestamp.value_or(0),
                    meanLenDto.endTimestamp.value_or(std::numeric_limits<uint64_t>::max())
                };

                auto interactions = storage->getEventInteractions(
                    it->second,
                    model.startTimestamp,
                    model.endTimestamp
                );

                double mean{calculateMeanPathLength(interactions, model.resultUnit)};
                return HttpResponse{HttpStatusCode::HTTP_STATUS_OK, json{{"mean", mean}}.dump()};
            }
            catch (const std::exception& e)
            {
                log->error("Handler error, path : {}, error : {}", req.path, e.what());
                return HttpResponse{
                    HttpStatusCode::HTTP_STATUS_BAD_REQUEST, json{{"error", e.what()}}.dump()
                };
            }
        });

        builder.registerPost("/paths/{event}", [storage](const HttpRequest& req)
        {
            try
            {
                log->debug(std::format("Handle path : {}, body : {}", req.path, req.body));
                auto eventDto{json::parse(req.body).get<dto::InteractionTimesEventDto>()};

                if (eventDto.values.size() != core::INTERACTION_TIMES_LEN)
                {
                    return HttpResponse{
                        HttpStatusCode::HTTP_STATUS_BAD_REQUEST,
                        json{{"error", "Invalid values len"}}.dump()
                    };
                }

                auto it{req.parameters.find("event")};
                if (it == req.parameters.end())
                {
                    return HttpResponse{
                        HttpStatusCode::HTTP_STATUS_BAD_REQUEST,
                        json{{"error", "No event name"}}.dump()
                    };
                }

                core::InteractionTimesEventModel model{};
                model.date = eventDto.date;
                std::copy(eventDto.values.begin(), eventDto.values.end(), model.values.begin());

                storage->storeEvent(it->second, std::move(model));
                return HttpResponse{HttpStatusCode::HTTP_STATUS_OK};
            }
            catch (const std::exception& e)
            {
                log->error("Handler error, path : {}, error : {}", req.path, e.what());
                return HttpResponse{
                    HttpStatusCode::HTTP_STATUS_BAD_REQUEST,
                    json{{"error", e.what()}}.dump()
                };
            }
        });
    }
}
