#include "telemetry_storage.h"

#include <mutex>
#include <shared_mutex>

namespace ctask::telemetry::core
{
    void TelemetryStorage::storeEvent(const std::string& eventName, InteractionTimesEventModel event)
    {
        EventEntriesSortedByTimestamp* tmp{nullptr};
        {
            std::shared_lock lock(mutex_);

            // at this point nobody is able to write in eventEntries_
            // let's try to find proper entry
            auto it{eventEntries_.find(eventName)};
            if (it != eventEntries_.end())
            {
                // store entry pointer
                tmp = &it->second;
            }
        }

        // if entry exists, lock it and store event data
        if (tmp != nullptr)
        {
            {
                // at this point nobody is able to modify eventEntry, store new data
                std::unique_lock lock(tmp->entryMutex);
                tmp->data.emplace(std::move(event.date), std::move(event.values));
                return;
            }
        }

        // brand new event comes, lock map and store event
        {
            std::unique_lock lock(mutex_);

            // this weird way to emplace new event, thanks to mutex;
            auto [it, _]{eventEntries_.try_emplace(eventName)};
            it->second.data.emplace(std::move(event.date), std::move(event.values));
        }
    }

    std::vector<InteractionTimesCollection> TelemetryStorage::getEventInteractions(
        const std::string& eventName, uint64_t from,
        uint64_t to)
    {
        EventEntriesSortedByTimestamp* tmp{nullptr};
        {
            std::shared_lock lock(mutex_);
            auto it{eventEntries_.find(eventName)};
            if (it == eventEntries_.end())
            {
                return {};
            }
            tmp = &it->second;
        }
        std::vector<InteractionTimesCollection> result{};
        result.reserve(INTERACTION_TIMES_LEN);

        {
            // nobody cant modify entry during reading
            std::shared_lock lock(tmp->entryMutex);
            auto lowerIt{tmp->data.lower_bound(from)};
            auto upperIt{tmp->data.upper_bound(to)};
            for (; lowerIt != upperIt; ++lowerIt)
            {
                result.emplace_back(lowerIt->second);
            }
        }

        return result;
    }
}
