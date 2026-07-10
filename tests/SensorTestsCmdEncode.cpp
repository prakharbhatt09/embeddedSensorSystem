#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "CommandEncoder.hpp"
#include "DeviceCommand.hpp"

TEST_CASE("Encode start command")
{
    CommandEncoder startCommand{};
    EncodedCommand const encodedData{startCommand.encodeCmd(DeviceCommand::Start)};
    REQUIRE(encodedData.size() == 1U);
    CHECK(encodedData.at(0U) == 1U);
}

TEST_CASE("Encode stop command")
{
    CommandEncoder stopCommand{};
    EncodedCommand const encodedData{stopCommand.encodeCmd(DeviceCommand::Stop)};
    REQUIRE(encodedData.size() == 1U);
    CHECK(encodedData.at(0U) == 2U);
}

TEST_CASE("Encode reset command")
{
    CommandEncoder resetCommand{};
    EncodedCommand const encodedData{resetCommand.encodeCmd(DeviceCommand::Reset)};
    REQUIRE(encodedData.size() == 1U);
    CHECK(encodedData.at(0U) == 3U);
}
