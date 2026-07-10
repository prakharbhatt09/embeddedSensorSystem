#include "TestObject.hpp"

std::array<uint8_t, kMaxMessageBytes> TestSensorV4::encode() const
{
    std::array<uint8_t, kMaxMessageBytes> encodedMessage{};
    encodedMessage.at(0U) = static_cast<uint8_t>(kMessageType >> 8U);
    encodedMessage.at(1U) = static_cast<uint8_t>(kMessageType & 0x00FFU);
    encodedMessage.at(2U) = static_cast<uint8_t>((getNrOfSensorReadings() << 4U) | (getRfa() & 0x0FU));
    for (uint8_t i{0U}; i < getNrOfSensorReadings(); ++i)
    {
        encodedMessage.at(3U + static_cast<std::size_t>(i)) =
            static_cast<uint8_t>((sensorReadings.at(i) << 4U) | (getRfa() & 0x0FU));
    }
    return encodedMessage;
}
