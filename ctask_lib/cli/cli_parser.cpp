#include "cli_parser.h"

#include <cxxopts.hpp>
#include <fstream>
#include <nlohmann/json.hpp>

namespace ctask::cli_parser
{
    using namespace ctask::utils::types;

    CliParser::CliParser(std::string appName, std::string appDescription) :
        appName_(std::move(appName)), appDescription_(std::move(appDescription))
    {
    }

    CliArgs CliParser::parse(int argc, char** argv)
    {
        cxxopts::Options options(appName_, appDescription_);
        options.add_options()
            ("c,config", "config file path", cxxopts::value<std::string>());

        auto result = options.parse(argc, argv);
        if (!result.count("config"))
        {
            throw std::invalid_argument("Config file path is required. Use --config or -c to specify it");
        }

        std::ifstream configFile(result["config"].as<std::string>());
        if (!configFile.is_open())
        {
            throw std::runtime_error("Can't open config file");
        }
        std::stringstream ss;
        ss << configFile.rdbuf();
        configFile.close();

        auto config{nlohmann::json::parse(ss.str())};
        return {
            {
                config["server"]["address"].get<std::string>(),
                config["server"]["port"].get<std::uint16_t>(),
                0,
                config["server"]["keepAliveSec"].get<std::uint16_t>(),
            },
            {
                config["logger"]["level"].get<std::string>(),
            }
        };
    }
}
