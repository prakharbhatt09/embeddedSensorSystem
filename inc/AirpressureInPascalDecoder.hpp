#ifndef AIRPRESSUREINPASCALDECODER_HPP
#define AIRPRESSUREINPASCALDECODER_HPP
#include <array>
#include <cstdint>
#include "DecoderProtocol.hpp"

constexpr std::size_t kAirpressureLength{4U};

DecoderResult decodeAirpressureInPascal(std::array<uint8_t, kMaxMessageBytes> const& msg);

#endif
