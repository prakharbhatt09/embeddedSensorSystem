#include <array>
#include <cstdint>
#include "DecoderProtocol.hpp"
#include "MultiReadingTemperatureDecoder.hpp"
#include "SensorValue.hpp"

DecoderResult decodeMultiReadingTemperature(std::array<uint8_t, kMaxMessageBytes> const& msg)
{
    DecoderResult result;
    uint8_t const nrOfReadings{msg.at(2U)};
    for (uint8_t i{0U}; i < nrOfReadings; ++i)
    {
        std::size_t const idx{kMultiReadingTemperatureHeaderBytes + kBytesPerMultiTemperatureReading * (std::size_t)(i)};
        uint16_t const tempCelsius{readBigEndianU16(msg, idx)};
        if (!result.push(SensorValue{(double)(tempCelsius), SensorType{"Temperature"}, SensorUnit{"Celsius"}}))
        {
            break;
        }
    }
    return result;
}