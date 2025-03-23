#include "telemetry/core/telemetry_storage.h"

#include <gtest/gtest.h>
#include <thread>

using namespace ctask::telemetry::core;
using namespace testing;

const auto GLOABL_EVENT_MODELS{
    std::vector<InteractionTimesEventModel>{
        {10, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
        {20, {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}},
        {30, {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}},
    }
};

TEST(TelemetryStorageTest, GetMissingEvent_ReturnEmpty)
{
    TelemetryStorage storage;
    auto result = storage.getEventInteractions("Nope, NotToday", 0, 1000);
    EXPECT_TRUE(result.empty());
}

TEST(TelemetryStorageTest, Store_SingleEvent_GetSingleRecord)
{
    TelemetryStorage storage;
    InteractionTimesEventModel model{
        .date = 1000,
        .values = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
    };

    storage.storeEvent("singleEvent", model);
    const auto actualValues{storage.getEventInteractions("singleEvent", 0, 2000)};

    ASSERT_EQ(actualValues.size(), 1);
    ASSERT_EQ(actualValues[0], model.values);
}

TEST(TelemetryStorageTest, Store_SingleEvents_WithDifferentNames)
{
    TelemetryStorage storage;

    storage.storeEvent("first", {10, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}});
    storage.storeEvent("second", {20, {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}});
    EXPECT_EQ(storage.getEventInteractions("first", 0, 100).size(), 1);
    EXPECT_EQ(storage.getEventInteractions("second", 0, 100).size(), 1);
}

TEST(TelemetryStorageTest, Store_SeveralEvents_WithSameNames_GetAllRange)
{
    TelemetryStorage storage;
    for (const auto& eventModel : GLOABL_EVENT_MODELS)
    {
        storage.storeEvent("first", eventModel);
    }
    // get all
    std::vector<InteractionTimesCollection> expectedInteractions{
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
        {3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    };
    const auto actualInteractions{storage.getEventInteractions("first", 0, 100)};
    ASSERT_EQ(actualInteractions, expectedInteractions);
}

TEST(TelemetryStorageTest, Store_SeveralEvents_WithSameNames_ScipFirstRecord_GetRange)
{
    TelemetryStorage storage;
    for (const auto& eventModel : GLOABL_EVENT_MODELS)
    {
        storage.storeEvent("first", eventModel);
    }

    // skip first
    std::vector<InteractionTimesCollection> expectedInteractions{
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
        {3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    };
    const auto actualInteractions{storage.getEventInteractions("first", 11, 100)};
    ASSERT_EQ(actualInteractions, expectedInteractions);
}

TEST(TelemetryStorageTest, Store_SeveralEvents_WithSameNames_ScipLastRecord_GetRange)
{
    TelemetryStorage storage;
    for (const auto& eventModel : GLOABL_EVENT_MODELS)
    {
        storage.storeEvent("first", eventModel);
    }

    // skip last
    std::vector<InteractionTimesCollection> expectedInteractions{
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    };
    const auto actualInteractions{storage.getEventInteractions("first", 0, 25)};
    ASSERT_EQ(actualInteractions, expectedInteractions);
}

TEST(TelemetryStorageTest, Store_SeveralEvents_WithSameNames_OnlyMiddleRecord_GetRange)
{
    TelemetryStorage storage;
    for (const auto& eventModel : GLOABL_EVENT_MODELS)
    {
        storage.storeEvent("first", eventModel);
    }

    {
        // middle
        std::vector<InteractionTimesCollection> expectedInteractions{
            {2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
        };
        const auto actualInteractions{storage.getEventInteractions("first", 15, 25)};
        ASSERT_EQ(actualInteractions, expectedInteractions);
    }
}

TEST(TelemetryStorageTest, Store_SeveralEvents_WithDifferentNames_InSeparateThreads)
{
    TelemetryStorage storage;

    struct Test
    {
        std::string eventName;
        InteractionTimesEventModel model;
    };

    std::vector<Test> firstPollerContent{
        {"first", 10},
        {"first", 20},
        {"first", 30},
        {"second", 40},
        {"second", 50},
        {"first", 60},
    };

    std::vector<Test> secondPollerContent{
        {"second", 70},
        {"first", 80},
        {"second", 90},
        {"first", 100},
        {"second", 110},
        {"second", 120},
    };

    auto firstPoller{
        std::jthread([&]()
        {
            for (const auto& poller : firstPollerContent)
            {
                storage.storeEvent(poller.eventName, poller.model);
            }
        })
    };
    auto secondPoller{
        std::jthread([&]()
        {
            for (const auto& poller : secondPollerContent)
            {
                storage.storeEvent(poller.eventName, poller.model);
            }
        })
    };

    firstPoller.join();
    secondPoller.join();

    EXPECT_EQ(storage.getEventInteractions("first", 0, 1000).size(), 6);
    EXPECT_EQ(storage.getEventInteractions("second", 0, 1000).size(), 6);
}
