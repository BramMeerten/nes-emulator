#include <algorithm>
#include <iostream>

#include "gtest/gtest.h"

#include "cpu/cpu.h"

// The fixture for testing class Foo.
class CpuTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
  }

  Cpu cpu;
};

TEST_F(CpuTest, LdaSetsRegisterA)
{
  // given
  unsigned char data[3] = {0xa9, 0b0100'0000, 0x00}; // LDA 0x40
  cpu.load(data, 3);

  // when
  cpu.run();

  // then
  EXPECT_EQ(cpu.a, 0x40);
  EXPECT_EQ(cpu.status, 0);
}

TEST_F(CpuTest, LdaSetsZeroFlag)
{ 
  // given
  unsigned char data[3] = {0xa9, 0x00, 0x00}; // LDA 00
  cpu.load(data, 3);

  // when
  cpu.run();

  // then
  EXPECT_EQ(cpu.a, 0);
  EXPECT_EQ(cpu.status, 0b0000'0010);
}

TEST_F(CpuTest, LdaSetsNegativeFlag)
{
  // given
  unsigned char data[3] = {0xa9, 0b1100'0000, 0x00}; // LDA c0
  cpu.load(data, 3);

  // when
  cpu.run();

  // then
  EXPECT_EQ(cpu.a, 0xc0);
  EXPECT_EQ(cpu.status, 0b0100'0000);
}