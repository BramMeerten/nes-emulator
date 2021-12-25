#include "gtest/gtest.h"

#include "bus.h"
#include "cpu/cpu.h"
#include "ppu/ppu.h"

class PpuRegisterTest : public ::testing::Test
{
public:
  PpuRegisterTest() {
    Ppu ppu;
    bus = new Bus(&ppu);
  }

  ~PpuRegisterTest() 
  {
    delete bus;
  }

protected:
  Bus *bus;
};

TEST_F(PpuRegisterTest, write_address)
{
  // given
  bus->write_8(0x2006, 0x40);
  bus->write_8(0x2006, 0x02);
  bus->write_8(0x2007, 0x18);
  
  bus->write_8(0x2006, 0x41);
  bus->write_8(0x2006, 0x14);
  bus->write_8(0x2007, 0x21);

  // when
  bus->write_8(0x2006, 0x40);
  bus->write_8(0x2006, 0x02);
  unsigned char read1 = bus->read(0x2007);
  unsigned char read2 = bus->read(0x2007);

  // then
  EXPECT_NE(read1, 0x18);
  EXPECT_EQ(read2, 0x18);
}
