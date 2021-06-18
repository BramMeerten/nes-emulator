#include "gtest/gtest.h"

#include "system.h"

class CpuAdcTwosComplementTest : public ::testing::Test
{
protected:
  System system;
};

unsigned char overflow(unsigned char status) {
  return (status & 0b0100'0000) >> 6;
}

unsigned char carry(unsigned char status) {
  return status & 0b0000'0001;
}

TEST_F(CpuAdcTwosComplementTest, One)
{
  // LDA #01;    1+1=2;  C=0, V=0
  // ADC #01;
  unsigned char data[5] = {0xa9, 0x01, 0x69, 0x01, 0x00};

  // when
  system.insertDisk(data, 5);

  // then
  EXPECT_EQ(system.cpu.getA(), 0x02);
  EXPECT_EQ(carry(system.cpu.getStatus()), 0);
  EXPECT_EQ(overflow(system.cpu.getStatus()), 0);
}

TEST_F(CpuAdcTwosComplementTest, Two)
{
  // LDA #01;    1 + -1 = 0;  C=1, V=0
  // ADC #FF;
  unsigned char data[5] = {0xa9, 0x01, 0x69, 0xff, 0x00};

  // when
  system.insertDisk(data, 5);

  // then
  EXPECT_EQ(system.cpu.getA(), 0);
  EXPECT_EQ(carry(system.cpu.getStatus()), 1);
  EXPECT_EQ(overflow(system.cpu.getStatus()), 0);
}

TEST_F(CpuAdcTwosComplementTest, Three)
{
  // LDA #7F;    127 + 1 = 128;  C=0, V=1
  // ADC #01;
  unsigned char data[5] = {0xa9, 0x7f, 0x69, 0x01, 0x00};

  // when
  system.insertDisk(data, 5);

  // then
  EXPECT_EQ(system.cpu.getA(), 128); // 128 (0x80) is -128 in two's complement -> overflow
  EXPECT_EQ(carry(system.cpu.getStatus()), 0);
  EXPECT_EQ(overflow(system.cpu.getStatus()), 1);
}

TEST_F(CpuAdcTwosComplementTest, Four)
{
  // LDA #80;    -128 + -1 = -129,  C=1, V=1
  // ADC #FF;
  unsigned char data[5] = {0xa9, 0x80, 0x69, 0xff, 0x00};

  // when
  system.insertDisk(data, 5);

  // then
  EXPECT_EQ(system.cpu.getA(), 127); // +127 instead of -129 -> overflow
  EXPECT_EQ(overflow(system.cpu.getStatus()), 1);
  EXPECT_EQ(carry(system.cpu.getStatus()), 1);
}

TEST_F(CpuAdcTwosComplementTest, Five)
{
  // SEC;        63 + 64 + 1 = 128,   C=0, V=1
  // LDA #3F; 
  // ADC #40;
  unsigned char data[6] = {0x38, 0xa9, 0x3f, 0x69, 0x40, 0x00};

  // when
  system.insertDisk(data, 6);

  // then
  EXPECT_EQ(system.cpu.getA(), 128); // 128 (0x80) is -128 in two's complement -> overflow
  EXPECT_EQ(carry(system.cpu.getStatus()), 0);
  EXPECT_EQ(overflow(system.cpu.getStatus()), 1);
}