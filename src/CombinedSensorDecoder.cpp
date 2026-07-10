#include <array>
#include <cstdint>
#include "CombinedSensorDecoder.hpp"
#include "DecoderProtocol.hpp"
#include "SensorValue.hpp"

namespace
{
constexpr uint16_t temperatureFlag{0x0001U};
constexpr uint16_t pressureFlag{0x0004U};
} /// namespace

DecoderResult decodeCombinedSensor(std::array<uint8_t, kMaxMessageBytes> const& msg)
{
    DecoderResult result;
    std::size_t idx{2U};
    while ((idx + 1U) < kMaxMessageBytes)
    {
        uint16_t const messageFlag{readBigEndianU16(msg, idx)};
        idx += 2U;
        if (messageFlag == 0U)
        {
            break;
        }
        if ((messageFlag & temperatureFlag) != 0U)
        {
            double const tempCelsius{(double)(msg.at(idx))};
            if (!result.push(SensorValue{tempCelsius, SensorType{"Temperature"}, SensorUnit{"Celsius"}}))
            {
                break;
            }
            ++idx;
        }
        if ((messageFlag & pressureFlag) != 0U)
        {
            uint16_t const pressure{readBigEndianU16(msg, idx)};
            if (!result.push(SensorValue{(double)(pressure), SensorType{"Airpressure"}, SensorUnit{"Pascal"}}))
            {
                break;
            }
            idx += 2U;
        }
    }
    return result;
}
