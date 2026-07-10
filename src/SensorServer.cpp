#include "SensorServer.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

void SensorServer::initializeDecoders()
{
  decoderTable.at((std::size_t)(MessageType::TemperatureCelsius))      = &decodeTemperatureInCelsius;
  decoderTable.at((std::size_t)(MessageType::TemperatureKelvin))       = &decodeTemperatureInKelvin;
  decoderTable.at((std::size_t)(MessageType::MultiReadingTemperature)) = &decodeMultiReadingTemperature;
  decoderTable.at((std::size_t)(MessageType::AirpressurePascal))       = &decodeAirpressureInPascal;
  decoderTable.at((std::size_t)(MessageType::CombinedSensor))          = &decodeCombinedSensor;
  decoderTable.at((std::size_t)(MessageType::TestTemperature))         = &decodeTestTemperature;
}

std::size_t SensorServer::getMessageLength(std::array<uint8_t, kMaxMessageBytes> const& msg) const
{
  uint16_t const messageType{ readBigEndian<uint16_t>(msg, 0U) };
  switch (messageType)
  {
    case (uint16_t)(MessageType::TemperatureCelsius):
      return kTempCelsiusLength;
    case (uint16_t)(MessageType::TemperatureKelvin):
      return kTempKelvinLength;
    case (uint16_t)(MessageType::MultiReadingTemperature):
      return kBytesPerMultiTemperatureReading * (std::size_t)(msg.at(2U)) + kMultiReadingTemperatureHeaderBytes;
    case (uint16_t)(MessageType::AirpressurePascal):
      return kAirpressureLength;
    default:
      return 0U;
  }
}

void SensorServer::decodeBuffer()
{
  while (bufferCount > 2U)
  {
    std::array<uint8_t, kMaxMessageBytes> msgBuf{};
    std::size_t const                     peekLen{ std::min(bufferCount, kMaxMessageBytes) };

    for (std::size_t i{ 0U }; i < peekLen; ++i)
    {
      msgBuf.at(i) = buffer.at(i);
    }

    std::size_t const msgLen{ getMessageLength(msgBuf) };
    if (((msgLen == 0U) || (bufferCount < msgLen)) || (msgLen > kMaxMessageBytes))
    {
      break;
    }

    Decoder(msgBuf);

    for (std::size_t i{ 0U }; i < bufferCount - msgLen; ++i)
    {
      buffer.at(i) = buffer.at(i + msgLen);
    }
    bufferCount -= msgLen;
  }
}

void SensorServer::Decoder(std::array<uint8_t, kMaxMessageBytes> const& encodedMessage)
{
  /// Step 1: read which kind of message this is.
  uint16_t const messageType{ readBigEndian<uint16_t>(encodedMessage, 0U) };
  if (messageType >= kDecoderTableSize)
  {
    return;
  }

  /// Step 2: look up the decoder registered for that message type.
  DecodeFn const chosenDecoder{ decoderTable.at(messageType) };
  if (chosenDecoder == nullptr)
  {
    return;
  }

  /// Step 3: run it to get this message's readings.
  DecoderResult const result{ chosenDecoder(encodedMessage) };

  /// Step 4: append those readings to the server's running total.
  std::size_t const count{ result.size() };
  for (std::size_t i{ 0U }; i < count; ++i)
  {
    if (!decodedValues.push(result.at(i)))
    {
      break;
    }
  }
}

DecodedValues const& SensorServer::getDecodedValues() const
{
  return decodedValues;
}

std::size_t SensorServer::getDecodedCount() const noexcept
{
  return decodedValues.size();
}

void SensorServer::addDecodedValue(SensorValue const& value)
{
  (void)decodedValues.push(value);
}
