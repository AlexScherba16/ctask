#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include "utils/types/types.h"

namespace ctask::cli_parser
{
    /**
     * @class CliParser
     * @brief Command-line arguments parser.
     *
     * This class is responsible for parsing command-line arguments
     * and extracting necessary information for proper application configuration
     */
    class CliParser
    {
    public:
        // Boring cpp boilerplate and obvious descriptions =D
        CliParser() = delete;
        CliParser(const CliParser&) = delete;
        CliParser(CliParser&&) = delete;
        CliParser& operator=(const CliParser&) = delete;
        CliParser& operator=(CliParser&&) = delete;

        ~CliParser() = default;

        /**
        * @brief Constructs a CliParser with application info details.
        *
        * @param appName Name of the application.
        * @param appDescription Brief description of the application.
        */
        explicit CliParser(std::string appName, std::string appDescription);

        /**
         * @brief Parses command-line arguments.
         *
         * Parses argv parameter to CliArgs
         *
         * @param argc Number of command-line arguments.
         * @param argv Array of command-line arguments.
         * @return Parsed command-line arguments stored in a CliArgs structure.
         *
         * @throws if required parameters are missed or invalid.
         */
        utils::types::CliArgs parse(int argc, char** argv);

    private:
        std::string appName_;
        std::string appDescription_;
    };
}

#endif //CLI_PARSER_H
