#include <array>
#include <cstdint>
#include "DecoderProtocol.hpp"
#include "SensorValue.hpp"
#include "TemperatureInCelsiusDecoder.hpp"

DecoderResult decodeTemperatureInCelsius(std::array<uint8_t, kMaxMessageBytes> const& msg)
{
    DecoderResult result;
    double const tempCelsius{(double)(msg.at(2U))};
    (void)result.push(SensorValue{tempCelsius, SensorType{"Temperature"}, SensorUnit{"Celsius"}});
    return result;
}
