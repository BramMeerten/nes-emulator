#include "gtest/gtest.h"

#include "memory.h"

// The fixture for testing class Foo.
class MemoryTest : public ::testing::Test
{
protected:
  Memory memory;
};

TEST_F(MemoryTest, MemoryReadsTwoByteValue)
{
  // given
  unsigned char data[2] = {0x34, 0x12};
  memory.write(0, data, 2);

  // when
  unsigned short value = memory.read_16(0);

  // then
  EXPECT_EQ(value, 0x1234);
}

TEST_F(MemoryTest, MemoryWritesTwoByteValue)
{
  // given
  unsigned short data = 0x1234;

  // when
  memory.write_16(0, data);

  // then
  EXPECT_EQ(memory.read(0), 0x34);
  EXPECT_EQ(memory.read(1), 0x12);
  EXPECT_EQ(memory.read_16(0), 0x1234);
}