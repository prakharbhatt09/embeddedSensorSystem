#ifndef MULTIREADINGTEMPERATUREDECODER_HPP
#define MULTIREADINGTEMPERATUREDECODER_HPP
#include <array>
#include <cstddef>
#include <cstdint>
#include "DecoderProtocol.hpp"

constexpr std::size_t kMultiReadingTemperatureHeaderBytes{3U};   /// messageType(2) + count(1)
constexpr std::size_t kBytesPerMultiTemperatureReading{2U};

DecoderResult decodeMultiReadingTemperature(std::array<uint8_t, kMaxMessageBytes> const& msg);

#endif
