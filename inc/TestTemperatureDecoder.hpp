#ifndef TESTTEMPERATUREDECODER_HPP
#define TESTTEMPERATUREDECODER_HPP
#include <array>
#include <cstdint>
#include "DecoderProtocol.hpp"

DecoderResult decodeTestTemperature(std::array<uint8_t, kMaxMessageBytes> const& msg);

#endif
