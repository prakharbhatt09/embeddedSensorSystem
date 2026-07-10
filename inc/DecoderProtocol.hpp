#ifndef DECODERPROTOCOL_HPP
#define DECODERPROTOCOL_HPP
#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>
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
    Humidity = 7U,
    CO2 = 8U,
};

using DecoderResult = FixedCapacityArray<SensorValue, 16U>;

/// Every decoder reads its multi-byte fields as big-endian (high byte first)
/// out of the message buffer at some offset -- shared here instead of being
/// reimplemented at each call site. T must be an unsigned integer type: a
/// signed T would make `value << 8U` undefined behavior the moment the
/// shift touches the sign bit.
template<typename T>
inline auto readBigEndian(std::array<uint8_t, kMaxMessageBytes> const& msg, std::size_t idx) -> T
{
    static_assert(std::is_unsigned<T>::value,
                  "readBigEndian<T> requires T to be an unsigned integer type");
    T value{0};
    for (std::size_t i{0U}; i < sizeof(T); ++i)
    {
        value = (T)((value << 8U) | (T)(msg.at(idx + i)));
    }
    return value;
}

#endif
