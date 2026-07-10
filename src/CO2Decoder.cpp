#include "CO2Decoder.hpp"
#include "SensorValue.hpp"

DecoderResult decodeCO2(std::array<uint8_t, kMaxMessageBytes> const& msg)
{
  DecoderResult result;
  uint16_t      co2Value{ readBigEndian<uint16_t>(msg, 2U) };
  SensorValue   sensorVal{ (double)(co2Value), SensorType{ "CO2" }, SensorUnit{ "ppm" } };
  (void)result.push(sensorVal);
  return result;
}