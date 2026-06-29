#pragma once

#include "../Devices/LPS25HB.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../Peripherals/I2C/I2CBase.hpp"


struct I2CMock : public Peripherals::I2CBase
{
    MOCK_METHOD(Peripherals::I2CResult, Write, (const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer), (override));
    MOCK_METHOD(Peripherals::I2CResult, Read, (const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer), (override));
};

class LPS25HBFixture : public ::testing::Test
{
public:
    I2CMock i2cMock;
    Device::LPS25HB lps25hb{ i2cMock };
    static constexpr uint16_t devAddr = 0xBA;
    static constexpr uint16_t AUTO_INCREMENT = 0x80;
};

TEST_F(LPS25HBFixture, ReadTemperature_Success)
{
    static constexpr uint16_t tempOutLowReg = 0x2B;
    
    uint8_t mockBuffer[2] = { 0x00, 0x00 };
    EXPECT_CALL(i2cMock, Read(devAddr, tempOutLowReg | AUTO_INCREMENT,
                              testing::Truly([&](std::span<uint8_t> s) {
                                  return s.size() == 2 && s[0] == mockBuffer[0] && s[1] == mockBuffer[1];
                              })))
        .WillOnce(testing::Return(Peripherals::I2CResult::Success));
    ASSERT_TRUE(lps25hb.ReadTemperature().has_value());
}

TEST_F(LPS25HBFixture, ReadPressure_Success)
{
    static constexpr uint16_t pressOutXLReg = 0x28;
    uint8_t mockBuffer[3] = { 0, 0, 0};
    EXPECT_CALL(i2cMock, Read(devAddr, pressOutXLReg | AUTO_INCREMENT,
                              testing::Truly([&](std::span<uint8_t> s) {
                                    return s.size() == 3 &&
                                           s[0] == mockBuffer[0] &&
                                           s[1] == mockBuffer[1] &&
                                           s[2] == mockBuffer[2];
                                })))
                .WillOnce(testing::Return(Peripherals::I2CResult::Success));

    ASSERT_TRUE(lps25hb.ReadPressure().has_value());
}
