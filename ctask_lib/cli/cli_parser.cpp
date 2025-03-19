#include "cli_parser.h"

#include <cxxopts.hpp>

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
            throw std::invalid_argument("config file path is required. Use --config or -c to specify it");
        }

        return {};
    }
}
