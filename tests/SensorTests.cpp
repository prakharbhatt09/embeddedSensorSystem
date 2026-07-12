#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "SensorServer.hpp"
#include "TestObject.hpp"

TEST_CASE("Server decodes for a TempSensor")
{
    SensorServer server;
    server.initializeDecoders();

    std::array<uint8_t, kMaxMessageBytes> const msg{0U, 1U, 23U};
    server.Decoder(msg);

    REQUIRE(server.getDecodedCount() == 1U);
    CHECK(server.getDecodedValues().at(0U).getValue() == 23.00);
    CHECK(server.getDecodedValues().at(0U).getType() == "Temperature");
    CHECK(server.getDecodedValues().at(0U).getUnit() == "Celsius");
}

TEST_CASE("Server decodes for a MultiReadingTemperatureSensor")
{
    SensorServer server;
    server.initializeDecoders();

    std::array<uint8_t, kMaxMessageBytes> const msg{0U, 3U, 3U, 0U, 23U, 0U, 24U, 0U, 25U};
    server.Decoder(msg);

    REQUIRE(server.getDecodedCount() == 3U);
    CHECK(server.getDecodedValues().at(0U).getValue() == 23.00);
    CHECK(server.getDecodedValues().at(1U).getValue() == 24.00);
    CHECK(server.getDecodedValues().at(2U).getValue() == 25.00);
}

TEST_CASE("Server decodes for a TempSensor in Kelvin")
{
    SensorServer server;
    server.initializeDecoders();

    std::array<uint8_t, kMaxMessageBytes> const msg{0U, 2U, 1U, 44U}; /// 300 K = 27 C
    server.Decoder(msg);

    REQUIRE(server.getDecodedCount() == 1U);
    CHECK(server.getDecodedValues().at(0U).getValue() == 27.00);
    CHECK(server.getDecodedValues().at(0U).getType() == "Temperature");
    CHECK(server.getDecodedValues().at(0U).getUnit() == "Celsius");
}

TEST_CASE("Server decodes for Airpressure in Pascal")
{
    SensorServer server;
    server.initializeDecoders();

    std::array<uint8_t, kMaxMessageBytes> const msg{0U, 4U, 0U, 101U};
    server.Decoder(msg);

    REQUIRE(server.getDecodedCount() == 1U);
    CHECK(server.getDecodedValues().at(0U).getValue() == 101.0);
    CHECK(server.getDecodedValues().at(0U).getType() == "Airpressure");
    CHECK(server.getDecodedValues().at(0U).getUnit() == "Pascal");
}

TEST_CASE("Decode scenario for 2 MultiReadingTemperatureDecoder messages")
{
    SensorServer server;
    server.initializeDecoders();

    std::array<uint8_t, kMaxMessageBytes> const msg1{0U, 3U, 3U, 0U, 23U, 0U, 24U, 0U, 25U};
    std::array<uint8_t, kMaxMessageBytes> const msg2{0U, 3U, 3U, 0U, 26U, 0U, 27U, 0U, 28U};
    server.Decoder(msg1);
    server.Decoder(msg2);

    REQUIRE(server.getDecodedCount() == 6U);
    CHECK(server.getDecodedValues().at(0U).getValue() == 23.00);
    CHECK(server.getDecodedValues().at(1U).getValue() == 24.00);
    CHECK(server.getDecodedValues().at(2U).getValue() == 25.00);
    CHECK(server.getDecodedValues().at(3U).getValue() == 26.00);
    CHECK(server.getDecodedValues().at(4U).getValue() == 27.00);
    CHECK(server.getDecodedValues().at(5U).getValue() == 28.00);
}

TEST_CASE("Sensor disconnect scenario with test double")
{
    SensorServer server;
    TestSensor sensor;

    std::array<uint8_t, kMaxMessageBytes> const msg1{0U, 3U, 3U, 0U, 23U, 0U, 24U, 0U, 25U};
    std::array<uint8_t, kMaxMessageBytes> const msg2{0U, 3U, 3U, 0U, 26U, 0U, 27U, 0U, 28U};
    sensor.addDataToBuffer(msg1);
    sensor.addDataToBuffer(msg2);

    server.initializeDecoders();
    std::size_t const count{sensor.getBufferCount()};
    for (std::size_t i{0U}; i < count; ++i)
    {
        server.Decoder(sensor.getBuffer().at(i));
    }

    REQUIRE(server.getDecodedCount() == 6U);
    CHECK(server.getDecodedValues().at(0U).getValue() == 23.00);
    CHECK(server.getDecodedValues().at(1U).getValue() == 24.00);
    CHECK(server.getDecodedValues().at(2U).getValue() == 25.00);
    CHECK(server.getDecodedValues().at(3U).getValue() == 26.00);
    CHECK(server.getDecodedValues().at(4U).getValue() == 27.00);
    CHECK(server.getDecodedValues().at(5U).getValue() == 28.00);
}

TEST_CASE("Server decodes for a CombinedSensor")
{
    SensorServer server;
    server.initializeDecoders();

    std::array<uint8_t, kMaxMessageBytes> const msg{0U, 5U, 0U, 1U, 23U, 0U, 4U, 0U, 101U};
    server.Decoder(msg);

    REQUIRE(server.getDecodedCount() == 2U);
    CHECK(server.getDecodedValues().at(0U).getValue() == 23.00);
    CHECK(server.getDecodedValues().at(0U).getType() == "Temperature");
    CHECK(server.getDecodedValues().at(1U).getValue() == 101.0);
    CHECK(server.getDecodedValues().at(1U).getType() == "Airpressure");
}

TEST_CASE("Buffer test: 1 temp sensor reading across two receives")
{
    SensorServer server;
    server.initializeDecoders();

    std::array<uint8_t, 2U> const message1{0U, 1U};
    std::array<uint8_t, 1U> const message2{23U};

    server.receiveDataFromSensor(message1);
    server.decodeBuffer();
    REQUIRE(server.getDecodedCount() == 0U);

    server.receiveDataFromSensor(message2);
    server.decodeBuffer();
    REQUIRE(server.getDecodedCount() == 1U);
    CHECK(server.getDecodedValues().at(0U).getValue() == 23.00);
}

TEST_CASE("Buffer test: 4 temp sensor readings across fragmented receives")
{
    SensorServer server;
    server.initializeDecoders();

    std::array<uint8_t, 2U> const m1{0U, 1U};
    std::array<uint8_t, 1U> const m2{23U};
    std::array<uint8_t, 3U> const m3{0U, 1U, 24U};
    std::array<uint8_t, 1U> const m4{0U};
    std::array<uint8_t, 2U> const m5{1U, 25U};
    std::array<uint8_t, 3U> const m6{0U, 1U, 26U};

    server.receiveDataFromSensor(m1); server.decodeBuffer();
    REQUIRE(server.getDecodedCount() == 0U);

    server.receiveDataFromSensor(m2); server.decodeBuffer();
    REQUIRE(server.getDecodedCount() == 1U);

    server.receiveDataFromSensor(m3); server.decodeBuffer();
    REQUIRE(server.getDecodedCount() == 2U);

    server.receiveDataFromSensor(m4); server.decodeBuffer();
    REQUIRE(server.getDecodedCount() == 2U);

    server.receiveDataFromSensor(m5); server.decodeBuffer();
    REQUIRE(server.getDecodedCount() == 3U);

    server.receiveDataFromSensor(m6); server.decodeBuffer();
    REQUIRE(server.getDecodedCount() == 4U);

    CHECK(server.getDecodedValues().at(0U).getValue() == 23.00);
    CHECK(server.getDecodedValues().at(1U).getValue() == 24.00);
    CHECK(server.getDecodedValues().at(2U).getValue() == 25.00);
    CHECK(server.getDecodedValues().at(3U).getValue() == 26.00);
}

TEST_CASE("Buffer test: partial MultiReading message then completion")
{
    SensorServer server;
    server.initializeDecoders();

    std::array<uint8_t, 9U> const m1{0U, 3U, 6U, 0U, 23U, 0U, 24U, 0U, 25U};
    std::array<uint8_t, 6U> const m2{0U, 26U, 0U, 27U, 0U, 28U};

    server.receiveDataFromSensor(m1);
    server.decodeBuffer();
    REQUIRE(server.getDecodedCount() == 0U);

    server.receiveDataFromSensor(m2);
    server.decodeBuffer();
    REQUIRE(server.getDecodedCount() == 6U);
    CHECK(server.getDecodedValues().at(0U).getValue() == 23.00);
    CHECK(server.getDecodedValues().at(5U).getValue() == 28.00);
}

TEST_CASE("TestSensorV4 encoding and TestTemperatureDecoder decoding")
{
    TestSensorV4 tempSensor;
    tempSensor.setNrOfSensorReadings(4U);
    tempSensor.setSensorReadingInVector(10U);
    tempSensor.setSensorReadingInVector(10U);
    tempSensor.setSensorReadingInVector(10U);
    tempSensor.setSensorReadingInVector(10U);

    std::array<uint8_t, kMaxMessageBytes> const encodedStream{tempSensor.encode()};

    CHECK(encodedStream.at(0U) == 0U);
    CHECK(encodedStream.at(1U) == 6U);
    CHECK(static_cast<uint16_t>(encodedStream.at(2U)) == 64U);
    CHECK(static_cast<uint16_t>(encodedStream.at(3U)) == 160U);

    SensorServer server;
    server.initializeDecoders();
    server.Decoder(encodedStream);

    REQUIRE(server.getDecodedCount() == 4U);
    CHECK(server.getDecodedValues().at(0U).getValue() == 10.0);
    CHECK(server.getDecodedValues().at(1U).getValue() == 10.0);
    CHECK(server.getDecodedValues().at(2U).getValue() == 10.0);
    CHECK(server.getDecodedValues().at(3U).getValue() == 10.0);
}
