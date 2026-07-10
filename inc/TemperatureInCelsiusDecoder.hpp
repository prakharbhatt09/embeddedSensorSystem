#ifndef TEMPERATUREINCELSIUSDECODER_HPP
#define TEMPERATUREINCELSIUSDECODER_HPP
#include <array>
#include <cstdint>
#include "DecoderProtocol.hpp"

constexpr std::size_t kTempCelsiusLength{3U};

DecoderResult decodeTemperatureInCelsius(std::array<uint8_t, kMaxMessageBytes> const& msg);

#endif
