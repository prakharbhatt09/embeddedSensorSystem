#ifndef HUMIDITYDECODER_HPP
#define HUMIDITYDECODER_HPP

#include "DecoderProtocol.hpp"
#include <array>
#include <cstdint>

constexpr std::size_t kHumidityDecoderLength{ 3U };

DecoderResult decodeHumidity(std::array<uint8_t, kMaxMessageBytes> const& msg);

#endif // HUMIDITYDECODER_HPP