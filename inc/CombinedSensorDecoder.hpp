#ifndef COMBINEDSENSORDECODER_HPP
#define COMBINEDSENSORDECODER_HPP
#include <array>
#include <cstdint>
#include "DecoderProtocol.hpp"

DecoderResult decodeCombinedSensor(std::array<uint8_t, kMaxMessageBytes> const& msg);

#endif
