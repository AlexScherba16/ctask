#include "cli/cli_parser.h"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <fstream>

using namespace testing;
using namespace nlohmann;
using namespace ctask::cli_parser;
using namespace ctask::utils::types;

TEST(CliParserTest, ParseMissingParameter_ThrowsException)
{
    const char* argv[] = {"test"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    CliParser parser("", "");
    ASSERT_THROW(parser.parse(argc, const_cast<char**>(argv)), std::invalid_argument);
}

TEST(CliParserTest, ParseValidInputParameter_InvalidConfigFilePath_ThrowsException)
{
    const std::string expectedPath{"path/to/something_expected_here.json"};
    const char* argv[] = {"test", "--config", expectedPath.c_str()};
    int argc = sizeof(argv) / sizeof(argv[0]);

    CliParser parser("", "");
    ASSERT_THROW(parser.parse(argc, const_cast<char**>(argv)), std::runtime_error);
}

TEST(CliParserTest, ParseInvalidInputParameter_ThrowsException)
{
    const std::string expectedPath{"path/to/something_expected_here.json"};
    const char* argv[] = {"test", "--INVALID_PARAMETER", expectedPath.c_str()};
    int argc = sizeof(argv) / sizeof(argv[0]);

    CliParser parser("", "");

    // assert any, to prevent high framework code dependency
    ASSERT_ANY_THROW(parser.parse(argc, const_cast<char**>(argv)));
}

TEST(CliParserTest, ParseValidInputParameter_ValidConfig)
{
    std::string data = json::object(
        {
            {"server", {{"address", "127.0.0.1"}, {"port", 8080}, {"keepAliveSec", 5}},},
            {"logger", {{"level", "debug"},},}
        }
    ).dump();

    char tempFileName[] = "/tmp/jsonXXXXXX";
    int fd = mkstemp(tempFileName);
    if (fd == -1)
    {
        throw std::runtime_error("Failed to create tmp json file");
    }

    std::ofstream file;
    file.open(tempFileName);
    if (!file)
    {
        throw std::runtime_error("Failed to open tmp json file");
    }
    file << data << "\n";
    file.close();

    const std::string expectedPath{tempFileName};
    const char* argv[] = {"test", "--config", expectedPath.c_str()};
    int argc = sizeof(argv) / sizeof(argv[0]);

    CliParser parser("", "");
    CliArgs result{};
    ASSERT_NO_THROW(result = parser.parse(argc, const_cast<char**>(argv)));
    ASSERT_EQ(result.serverArgs.address, "127.0.0.1");
    ASSERT_EQ(result.serverArgs.port, 8080);
    ASSERT_EQ(result.serverArgs.threads, 0);
    ASSERT_EQ(result.serverArgs.keepAliveSec, 5);
    ASSERT_EQ(result.loggerArgs.level, "debug");
}
