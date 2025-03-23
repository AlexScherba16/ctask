#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async.h>

/**
 * @class Logger
 * @brief Global logger entity
 *
 * This class provides a logging utility.
 */
class Logger
{
public:
    /**
     * @brief Global logger entry point
     *
     * @returns Logger instance
     */
    static Logger& instance()
    {
        static Logger logger;
        return logger;
    }

    std::shared_ptr<spdlog::logger> getLogger() { return logger_; }

private:
    Logger()
    {
        spdlog::init_thread_pool(8192, 1);
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        logger_ = std::make_shared<spdlog::async_logger>(
            "global_logger", consoleSink, spdlog::thread_pool(),
            spdlog::async_overflow_policy::block);

        // Format: [2025-03-20 12:34:56] [info] [Thread ID: 1234] Message
        logger_->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] [Thread ID: %t] %v");

        spdlog::register_logger(logger_);
        spdlog::set_default_logger(logger_);
        spdlog::set_level(spdlog::level::debug);
    }

    std::shared_ptr<spdlog::logger> logger_;
};
#endif //LOGGER_H
