#pragma once

#include "../Devices/LPS25HB.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../Peripherals/I2C/I2CBase.hpp"


struct I2CMock : public Peripherals::I2CBase
{
    MOCK_METHOD(void, Write, (const uint16_t deviceAddress, const uint16_t memoryAddress, const uint8_t value, const uint32_t timeOut), (override));
    MOCK_METHOD(bool, Read, (const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer, const uint32_t timeout), (override));
};

class LPS25HBFixture : public ::testing::Test
{
public:
    I2CMock i2cMock;
    Device::LPS25HB lps25hb{ i2cMock };
};

TEST_F(LPS25HBFixture, ReadTemperature_Success)
{
    static constexpr uint16_t devAddr = 0xBA;
    static constexpr uint16_t tempOutLowReg = 0x2B;
    
    uint8_t mockBuffer[2] = { 0x00, 0x00 };
    EXPECT_CALL(i2cMock, Read(devAddr, tempOutLowReg | 0x80,
                              testing::Truly([&](std::span<uint8_t> s) {
                                  return s.size() == 2 && s[0] == mockBuffer[0] && s[1] == mockBuffer[1];
                              })
                              , 1000))
        .WillOnce(testing::Return(true));
    ASSERT_TRUE(lps25hb.ReadTemperature().has_value());
}
