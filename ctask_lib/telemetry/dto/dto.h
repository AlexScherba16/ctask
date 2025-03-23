#ifndef TELEMETRY_DTO_H
#define TELEMETRY_DTO_H

#include "telemetry/core/misc.h"

#include <nlohmann/json.hpp>
#include <optional>

namespace ctask::telemetry::dto
{
    /**
     * @struct InteractionTimesEventDto
     * @brief DTO for interaction times events.
     *
     * Represents raw data received in an HTTP request before validation.
     * Contains a date and a std::vector of interaction times.
     */
    struct InteractionTimesEventDto
    {
        core::EventDateType date{};
        std::vector<core::InteractionTimeType> values{};
    };

    inline void from_json(const nlohmann::json& j, InteractionTimesEventDto& dto)
    {
        j.at("date").get_to(dto.date);
        j.at("values").get_to(dto.values);
    }

    /**
     * @brief DTO for querying mean interaction length.
     *
     * Represents query parameters as received in an HTTP request.
     * Includes an optional time range and a unit of measurement.
     */
    struct MeanLengthQueryDto
    {
        std::string resultUnit; // "seconds" or "milliseconds"
        std::optional<uint64_t> startTimestamp;
        std::optional<uint64_t> endTimestamp;
    };

    inline void from_json(const nlohmann::json& j, dto::MeanLengthQueryDto& dto)
    {
        j.at("resultUnit").get_to(dto.resultUnit);
        if (j.contains("startTimestamp"))
            dto.startTimestamp = j.at("startTimestamp").get<uint64_t>();

        if (j.contains("endTimestamp"))
            dto.endTimestamp = j.at("endTimestamp").get<uint64_t>();
    }
}

#endif //TELEMETRY_DTO_H
