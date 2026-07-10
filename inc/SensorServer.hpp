#ifndef SENSORSERVER_HPP
#define SENSORSERVER_HPP
#include <array>
#include <cstddef>
#include <cstdint>
#include "AirpressureInPascalDecoder.hpp"
#include "CombinedSensorDecoder.hpp"
#include "DecoderProtocol.hpp"
#include "MultiReadingTemperatureDecoder.hpp"
#include "SensorValue.hpp"
#include "TemperatureInCelsiusDecoder.hpp"
#include "TemperatureInKelvinDecoder.hpp"
#include "TestTemperatureDecoder.hpp"

constexpr std::size_t kMaxTotalDecodedValues{64U};

/// All decoded SensorValues across every message this server has processed.
///
/// Exposed as the FixedCapacityArray itself (bounds-checked at(), and
/// size() tied to what's actually been written) rather than the previous
/// std::array<SensorValue, kMaxTotalDecodedValues> plus a separate
/// getDecodedCount() -- that pairing let a caller range-for over the raw
/// array and read default-constructed junk past the real data, since nothing
/// tied the two together.
///
/// In C++20, std::span<SensorValue const> would be an even lighter-weight
/// return type here: a non-owning (pointer, size) view that doesn't bake the
/// kMaxTotalDecodedValues capacity into the return type at all. That would
/// need FixedCapacityArray to grow a data() accessor to build the span from,
/// which it doesn't have today.
using DecodedValues = FixedCapacityArray<SensorValue, kMaxTotalDecodedValues>;

class SensorServer final
{
  public:
    SensorServer() = default;

    void initializeDecoders();
    void Decoder(std::array<uint8_t, kMaxMessageBytes> const& encodedMessage);
    DecodedValues const& getDecodedValues() const;
    std::size_t getDecodedCount() const noexcept;
    void addDecodedValue(SensorValue const& value);
    void decodeBuffer();
    std::size_t getMessageLength(std::array<uint8_t, kMaxMessageBytes> const& msg) const;

    template<std::size_t N>
    void receiveDataFromSensor(std::array<uint8_t, N> const& data)
    {
        for (std::size_t i{0U}; (i < N) && (bufferCount < kServerBufferCapacity); ++i)
        {
            buffer.at(bufferCount) = data.at(i);
            ++bufferCount;
        }
    }

  private:
    /// A table of plain function pointers, one indirection per call, no
    /// per-object vtable pointer and no decoder instances to store at all --
    /// every decoder is a stateless free function.
    using DecodeFn = DecoderResult (*)(std::array<uint8_t, kMaxMessageBytes> const&);

    static constexpr std::size_t kServerBufferCapacity{256U};
    static constexpr std::size_t kDecoderTableSize{7U};

    std::array<DecodeFn, kDecoderTableSize>    decoderTable{};
    DecodedValues                              decodedValues{};
    std::array<uint8_t, kServerBufferCapacity> buffer{};
    std::size_t                                bufferCount{0U};
};

#endif