#ifndef TELEMETRY_MISC_H
#define TELEMETRY_MISC_H

#include <array>
#include <string>
#include <vector>
#include <numeric>
#include <cstdint>
#include <stdexcept>

namespace ctask::telemetry::core
{
    /**
     * @brief General scrapyard for telemetry, Human friendly aliases, helpers, types
     *
     */

    constexpr uint8_t INTERACTION_TIMES_LEN{10};

    using EventDateType = uint64_t;
    using InteractionTimeType = int32_t;

    using InteractionTimesCollection = std::array<InteractionTimeType, INTERACTION_TIMES_LEN>;

    using EventName = std::string;

    enum class TimeUnit
    {
        Seconds,
        Milliseconds
    };

    inline TimeUnit parseTimeUnit(const std::string& str)
    {
        if (str == "seconds")
        {
            return TimeUnit::Seconds;
        }
        if (str == "milliseconds")
        {
            return TimeUnit::Milliseconds;
        }

        throw std::invalid_argument("Invalid time unit");
    };

    inline double calculateMeanPathLength(
        const std::vector<InteractionTimesCollection>& interactions,
        TimeUnit unit = TimeUnit::Seconds
    )
    {
        if (interactions.empty())
        {
            return 0.0;
        }

        uint64_t totalTime{0};
        for (const auto& interaction : interactions)
        {
            totalTime += std::accumulate(interaction.begin(), interaction.end(), 0);
        }

        double mean = static_cast<double>(totalTime) / interactions.size();
        return unit == TimeUnit::Milliseconds ? mean * 1000.0 : mean;
    }
}

#endif //TELEMETRY_MISC_H
