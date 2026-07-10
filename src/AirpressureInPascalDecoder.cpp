#include <array>
#include <cstdint>
#include "AirpressureInPascalDecoder.hpp"
#include "DecoderProtocol.hpp"
#include "SensorValue.hpp"

DecoderResult decodeAirpressureInPascal(std::array<uint8_t, kMaxMessageBytes> const& msg)
{
    DecoderResult result;
    uint16_t const pressure{readBigEndianU16(msg, 2U)};
    (void)result.push(SensorValue{(double)(pressure), SensorType{"Airpressure"}, SensorUnit{"Pascal"}});
    return result;

}
