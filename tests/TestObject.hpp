#ifndef TESTOBJECT_HPP
#define TESTOBJECT_HPP
#include <array>
#include <cstddef>
#include <cstdint>
#include "DecoderProtocol.hpp"

class ITestSensor
{
  public:
    virtual void addDataToBuffer(std::array<uint8_t, kMaxMessageBytes> const& data) = 0;
    virtual std::array<std::array<uint8_t, kMaxMessageBytes>, 16U> const& getBuffer() const = 0;
    virtual std::size_t getBufferCount() const noexcept = 0;
    virtual ~ITestSensor() = default;
};

class TestSensor final : public ITestSensor
{
  public:
    TestSensor() = default;

    void addDataToBuffer(std::array<uint8_t, kMaxMessageBytes> const& data) override
    {
        internalBuffer.at(messageCount) = data;
        ++messageCount;
    }

    std::array<std::array<uint8_t, kMaxMessageBytes>, 16U> const& getBuffer() const override
    {
        return internalBuffer;
    }

    std::size_t getBufferCount() const noexcept override { return messageCount; }

  private:
    std::array<std::array<uint8_t, kMaxMessageBytes>, 16U> internalBuffer{};
    std::size_t messageCount{0U};
};

class TestSensorV4 final : public ITestSensor
{
  public:
    TestSensorV4() = default;

    std::array<uint8_t, kMaxMessageBytes> encode() const;

    void addDataToBuffer(std::array<uint8_t, kMaxMessageBytes> const& data) override
    {
        internalBuffer.at(messageCount) = data;
        ++messageCount;
    }

    std::array<std::array<uint8_t, kMaxMessageBytes>, 16U> const& getBuffer() const override
    {
        return internalBuffer;
    }

    std::size_t getBufferCount() const noexcept override { return messageCount; }

    uint8_t getNrOfSensorReadings() const noexcept { return nrOfSensorReadings; }
    uint8_t getRfa() const noexcept { return rfa; }

    void setNrOfSensorReadings(uint8_t number) noexcept { nrOfSensorReadings = number; }
    void setSensorReadingInVector(uint8_t reading)
    {
        sensorReadings.at(readingCount) = reading;
        ++readingCount;
    }

  private:
    static constexpr uint16_t kMessageType{0x0006U};
    uint8_t nrOfSensorReadings{0U};
    uint8_t rfa{0U};
    std::array<uint8_t, 16U> sensorReadings{};
    std::size_t readingCount{0U};
    std::array<std::array<uint8_t, kMaxMessageBytes>, 16U> internalBuffer{};
    std::size_t messageCount{0U};
};

#endif
