#include "CommandEncoder.hpp"
#include "DeviceCommand.hpp"

EncodedCommand CommandEncoder::encodeCmd(DeviceCommand command)
{
    EncodedCommand encodedData;
    switch (command)
    {
        case DeviceCommand::Start:
            (void)encodedData.push(0x01U);
            break;
        case DeviceCommand::Stop:
            (void)encodedData.push(0x02U);
            break;
        case DeviceCommand::Reset:
            (void)encodedData.push(0x03U);
            break;
        default:
            break;
    }
    return encodedData;
}
