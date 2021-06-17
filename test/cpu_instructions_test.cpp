#include "gtest/gtest.h"

#include "system.h"

class CpuTest : public ::testing::Test
{
protected:
  System system;
};

TEST_F(CpuTest, LdaSetsRegisterA)
{
  // given
  unsigned char data[3] = {0xa9, 0b0100'0000, 0x00}; // LDA #40

  // when
  system.insertDisk(data, 3);

  // then
  EXPECT_EQ(system.cpu.getA(), 0x40);
  EXPECT_EQ(system.cpu.getStatus(), 0);
}

TEST_F(CpuTest, LdaSetsZeroFlag)
{ 
  // given
  unsigned char data[3] = {0xa9, 0x00, 0x00}; // LDA 00

  // when
  system.insertDisk(data, 3);

  // then
  EXPECT_EQ(system.cpu.getA(), 0);
  EXPECT_EQ(system.cpu.getStatus(), 0b0000'0010);
}

TEST_F(CpuTest, LdaSetsNegativeFlag)
{
  // given
  unsigned char data[3] = {0xa9, 0b1100'0000, 0x00}; // LDA c0

  // when
  system.insertDisk(data, 3);

  // then
  EXPECT_EQ(system.cpu.getA(), 0xc0);
  EXPECT_EQ(system.cpu.getStatus(), 0b1000'0000);
}

TEST_F(CpuTest, TaxSetsXRegister)
{
  // given
  unsigned char data[4] = {0xa9, 0x80, 0xaa, 0x00}; // LDA #80; TAX;

  // when
  system.insertDisk(data, 4);

  // then
  EXPECT_EQ(system.cpu.getX(), 0x80);
  EXPECT_EQ(system.cpu.getStatus(), 0b1000'0000);
}

TEST_F(CpuTest, InxIncreasesTheXRegister)
{
  // given
  unsigned char data[5] = {0xa2, 0x80, 0xe8, 0x00}; // LDX #80; INX

  // when
  system.insertDisk(data, 5);

  // then
  EXPECT_EQ(system.cpu.getX(), 0x81);
  EXPECT_EQ(system.cpu.getStatus(), 0b1000'0000);
}

TEST_F(CpuTest, LdxSetsRegisterX)
{
  // given
  unsigned char data[3] = {0xa2, 0x40, 0x00}; // LDX #40

  // when
  system.insertDisk(data, 3);

  // then
  EXPECT_EQ(system.cpu.getX(), 0x40);
}

TEST_F(CpuTest, LdySetsRegisterY)
{
  // given
  unsigned char data[3] = {0xa0, 0x40, 0x00}; // LDY #40

  // when
  system.insertDisk(data, 3);

  // then
  EXPECT_EQ(system.cpu.getY(), 0x40);
}

TEST_F(CpuTest, And)
{
  // given
  unsigned char data[5] = {0xa9, 0b0011'0011, 0x29, 0b1010'0001, 0x00}; // LDA #33; AND #a1;

  // when
  system.insertDisk(data, 5);

  // then
  EXPECT_EQ(system.cpu.getA(), 0b0010'0001);
}

TEST_F(CpuTest, Adc)
{
  // given
  unsigned char data[5] = {0xa9, 0x33, 0x69, 0x12, 0x00}; // LDA #33; ADC #12;

  // when
  system.insertDisk(data, 5);

  // then
  EXPECT_EQ(system.cpu.getA(), 0x45);
  EXPECT_EQ(system.cpu.getStatus(), 0b0000'0000); // carry = 0
}

TEST_F(CpuTest, Adc_set_carry)
{
  // given
  unsigned char data[5] = {0xa9, 0xff, 0x69, 0xff, 0x00}; // LDA #ff; ADC #ff;

  // when
  system.insertDisk(data, 5);

  // then
  EXPECT_EQ(system.cpu.getA(), 0xfe); // #ff + #ff = #1FE 
  EXPECT_EQ(system.cpu.getStatus(), 0b1000'0001); // carry = 1
}

TEST_F(CpuTest, Adc_adds_carry)
{
  // given
  unsigned char data[9] = {0xa9, 0xff, 0x69, 0xff, 0xa9, 0xff, 0x69, 0xff, 0x00}; // LDA #ff; ADC #ff; LDA #ff; ADC #ff;

  // when
  system.insertDisk(data, 9);

  // then
  EXPECT_EQ(system.cpu.getA(), 0xff); // #ff + #ff + #01 = #1FF 
  EXPECT_EQ(system.cpu.getStatus(), 0b1000'0001); // carry = 1
}

TEST_F(CpuTest, Sec)
{
  // given
  unsigned char data[2] = {0x38, 0x00}; // SEC;

  // when
  system.insertDisk(data, 2);

  // then
  EXPECT_EQ(system.cpu.getStatus(), 0b0000'0001); // carry = 1
}