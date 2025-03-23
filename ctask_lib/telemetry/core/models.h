#ifndef TELEMETRY_MODEL_H
#define TELEMETRY_MODEL_H

#include "misc.h"

#include <array>

namespace ctask::telemetry::core
{
    /**
     * @struct InteractionTimesEventModel
     * @brief Validated model for storing interaction times events.
     *
     * Ensures correctness and is ready for processing.
     */
    struct InteractionTimesEventModel
    {
        EventDateType date{};
        std::array<InteractionTimeType, INTERACTION_TIMES_LEN> values{};
    };

    /**
     * @struct MeanLengthQueryModel
     * @brief Validated model for querying mean interaction length.
     *
     * Ensures correctness and is ready for processing.
     */
    struct MeanLengthQueryModel
    {
        TimeUnit resultUnit;
        uint64_t startTimestamp;
        uint64_t endTimestamp;
    };
}

#endif //TELEMETRY_MODEL_H
