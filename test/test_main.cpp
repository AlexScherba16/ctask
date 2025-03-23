#include <gtest/gtest.h>

#include "logger.h"

int main(int argc, char** argv)
{
    auto log{Logger::instance().getLogger()};
    log->set_level(spdlog::level::off);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
