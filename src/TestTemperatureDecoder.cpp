#include <array>
#include <cstdint>
#include "DecoderProtocol.hpp"
#include "SensorValue.hpp"
#include "TestTemperatureDecoder.hpp"

DecoderResult decodeTestTemperature(std::array<uint8_t, kMaxMessageBytes> const& msg)
{
    DecoderResult result;
    uint8_t const nrOfReadings{(uint8_t)(msg.at(2U) >> 4U)};
    for (uint8_t i{0U}; i < nrOfReadings; ++i)
    {
        double const tempCelsius{(double)(msg.at(3U + (std::size_t)(i)) >> 4U)};
        if (!result.push(SensorValue{tempCelsius, SensorType{"Temperature"}, SensorUnit{"Celsius"}}))
        {
            break;
        }
    }
    return result;
}
