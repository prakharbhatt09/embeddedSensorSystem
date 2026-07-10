#ifndef COMMANDENCODER_HPP
#define COMMANDENCODER_HPP
#include <cstdint>
#include "DeviceCommand.hpp"
#include "FixedCapacityArray.hpp"

using EncodedCommand = FixedCapacityArray<uint8_t, 4U>;

class CommandEncoder final
{
  public:
    EncodedCommand encodeCmd(DeviceCommand command);
};

#endif
