#include "gtest/gtest.h"

#include "system.h"

// The fixture for testing class Foo.
class CpuTest : public ::testing::Test
{
protected:
  System system;
};

TEST_F(CpuTest, LdaImmediateSetsRegisterA)
{
  // given
  unsigned char data[3] = {0xa9, 0b0100'0000, 0x00}; // LDA #40

  // when
  system.insertDisk(data, 3);

  // then
  EXPECT_EQ(system.cpu.getA(), 0x40);
  EXPECT_EQ(system.cpu.getStatus(), 0);
}

TEST_F(CpuTest, LdaZeroPageSetsRegisterA)
{
  // given
  unsigned char value[1] = {9};
  system.memory.write(0x05, value, 1);
  unsigned char data[3] = {0xa5, 0x05, 0x00}; // LDA $05

  // when
  system.insertDisk(data, 3);

  // then
  EXPECT_EQ(system.cpu.getA(), value[0]);
}

TEST_F(CpuTest, LdaZeroPageXSetsRegisterA)
{
  // given
  unsigned char value[1] = {9};
  system.memory.write(0x85, value, 1);
  unsigned char data[6] = {0xa9, 0x80, 0xaa, 0xb5, 0x05, 0x00}; // LDA #80; TAX; LDA $05,X  TODO: replace with LDX #80

  // when
  system.insertDisk(data, 6);

  // then
  EXPECT_EQ(system.cpu.getA(), value[0]);
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
  EXPECT_EQ(system.cpu.getStatus(), 0b0100'0000);
}

TEST_F(CpuTest, TaxSetsXRegister)
{
  // given
  unsigned char data[4] = {0xa9, 0x80, 0xaa, 0x00}; // LDA #80; TAX

  // when
  system.insertDisk(data, 4);

  // then
  EXPECT_EQ(system.cpu.getX(), 0x80);
  EXPECT_EQ(system.cpu.getStatus(), 0b0100'0000);
}

TEST_F(CpuTest, InxIncreasesTheXRegister)
{
  // given
  unsigned char data[5] = {0xa9, 0x80, 0xaa, 0xe8, 0x00}; // LDA #80; TAX; INX  TODO: replace with LDX #80

  // when
  system.insertDisk(data, 5);

  // then
  EXPECT_EQ(system.cpu.getX(), 0x81);
  EXPECT_EQ(system.cpu.getStatus(), 0b0100'0000);
}
