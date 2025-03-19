#include "cli/cli_parser.h"

#include <iostream>

int main(int argc, char** argv)
{
    using namespace ctask::cli_parser;

    try
    {
        const auto args{CliParser{"сtask", "Callstack coding task"}.parse(argc, argv)};
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
