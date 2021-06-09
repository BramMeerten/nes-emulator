#include "gtest/gtest.h"

#include "system.h"

// The fixture for testing class Foo.
class CpuTest : public ::testing::Test
{
protected:
  System system;
};

TEST_F(CpuTest, LdaSetsRegisterA)
{
  // given
  unsigned char data[3] = {0xa9, 0b0100'0000, 0x00}; // LDA 0x40

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
  EXPECT_EQ(system.cpu.getStatus(), 0b0100'0000);
}