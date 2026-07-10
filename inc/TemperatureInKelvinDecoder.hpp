#ifndef TEMPERATUREINKELVINDECODER_HPP
#define TEMPERATUREINKELVINDECODER_HPP
#include <array>
#include <cstdint>
#include "DecoderProtocol.hpp"

constexpr std::size_t kTempKelvinLength{4U};

DecoderResult decodeTemperatureInKelvin(std::array<uint8_t, kMaxMessageBytes> const& msg);

#endif
