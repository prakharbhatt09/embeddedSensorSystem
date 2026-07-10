#ifndef DECODERPROTOCOL_HPP
#define DECODERPROTOCOL_HPP
#include <array>
#include <cstddef>
#include <cstdint>
#include "FixedCapacityArray.hpp"
#include "SensorValue.hpp"

constexpr std::size_t kMaxMessageBytes{32U};

/// The wire-format message-type tag read out of every message's first two
/// bytes -- named here so SensorServer's decoder table and message-length
/// lookup don't reference raw 1-6 literals with no indication of which
/// decoder each number actually selects.
enum class MessageType : std::uint16_t
{
    TemperatureCelsius      = 1U,
    TemperatureKelvin       = 2U,
    MultiReadingTemperature = 3U,
    AirpressurePascal       = 4U,
    CombinedSensor          = 5U,
    TestTemperature         = 6U,
};

using DecoderResult = FixedCapacityArray<SensorValue, 16U>;

/// Every decoder reads its 16-bit fields as big-endian (high byte first) out
/// of the message buffer at some offset -- shared here instead of being
/// reimplemented at each call site.
inline uint16_t readBigEndianU16(std::array<uint8_t, kMaxMessageBytes> const& msg, std::size_t idx)
{
    return (uint16_t)((msg.at(idx) << 8U) | msg.at(idx + 1U));
}

#endif
