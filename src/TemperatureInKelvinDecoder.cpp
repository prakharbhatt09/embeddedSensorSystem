#include <array>
#include <cstdint>
#include "DecoderProtocol.hpp"
#include "SensorValue.hpp"
#include "TemperatureInKelvinDecoder.hpp"

DecoderResult decodeTemperatureInKelvin(std::array<uint8_t, kMaxMessageBytes> const& msg)
{
    DecoderResult result;
    uint16_t const tempKelvin{readBigEndian<uint16_t>(msg, 2U)};
    double const tempCelsius{(double)(tempKelvin) - 273.0};
    (void)result.push(SensorValue{tempCelsius, SensorType{"Temperature"}, SensorUnit{"Celsius"}});
    return result;
}
