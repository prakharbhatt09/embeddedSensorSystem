#ifndef CO2DECODER_HPP
#define CO2DECODER_HPP

#include "DecoderProtocol.hpp"
#include <array>
#include <cstdint>

constexpr std::size_t kCO2DecoderLength{ 4U };

DecoderResult decodeCO2(std::array<uint8_t, kMaxMessageBytes> const& msg);

#endif // CO2DECODER_HPP