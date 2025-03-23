#ifndef TELEMETRY_STORAGE_H
#define TELEMETRY_STORAGE_H

#include "models.h"

#include <map>
#include <shared_mutex>
#include <unordered_map>

namespace ctask::telemetry::core
{
    /**
     * @class TelemetryStorage
     * @brief Thread-safe in-memory telemetry storage.
     *
     * Designed to store and retrieve telemetry interaction events efficiently.
     * Uses std::unordered_map for quick event name lookup and
     * std::map to keep event entries sorted by timestamp.
     */
    class TelemetryStorage
    {
    public:
        TelemetryStorage() = default;
        ~TelemetryStorage() = default;
        TelemetryStorage(const TelemetryStorage&) = delete;
        TelemetryStorage& operator=(const TelemetryStorage&) = delete;
        TelemetryStorage(TelemetryStorage&&) = delete;
        TelemetryStorage& operator=(TelemetryStorage&&) = delete;

        /**
         * @brief Stores a telemetry event.
         *
         * Stores an interaction event under the given event name.
         * Uses mutex_ to allow unique ownership in case of storing brand-new event.
         *
         * @param eventName The name of the event.
         * @param event The event data.
         */
        void storeEvent(const std::string& eventName, InteractionTimesEventModel event);

        /**
         * @brief Retrieves telemetry events in a given time range.
         *
         * Returns a collection of interaction events for a specific event name
         * that occurred within the specified timestamp range.
         *
         * @param eventName The name of the event.
         * @param from The start timestamp (inclusive).
         * @param to The end timestamp (inclusive).
         * @return Interactions collection.
         */
        std::vector<InteractionTimesCollection> getEventInteractions(const std::string& eventName,
                                                                     uint64_t from,
                                                                     uint64_t to);

    private:
        /**
         * @struct EventEntriesSortedByTimestamp
         * @brief Internal structure for storing event data sorted by timestamp.
         *
         * Uses std::map to maintain timestamp ordering for fast range queries.
         * Read/write access is controlled with std::shared_mutex to allow
         * concurrent reads and serialized writes.
         */
        struct EventEntriesSortedByTimestamp
        {
            std::map<EventDateType, InteractionTimesCollection> data;
            std::shared_mutex entryMutex;
        };

        std::shared_mutex mutex_;
        std::unordered_map<EventName, EventEntriesSortedByTimestamp> eventEntries_;
    };
}

#endif //TELEMETRY_STORAGE_H
