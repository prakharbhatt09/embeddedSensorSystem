#include "HumidityDecoder.hpp"

DecoderResult decodeHumidity(std::array<uint8_t, kMaxMessageBytes> const& msg)
{
  DecoderResult result;
  uint8_t       humidityValue{ msg.at(2U) };
  double        humidityPercent{ static_cast<double>(humidityValue) };
  SensorValue   sensorValue{ humidityPercent, SensorType{ "Humidity" }, SensorUnit{ "Percentage" } };
  (void)result.push(sensorValue);
  return result;
}