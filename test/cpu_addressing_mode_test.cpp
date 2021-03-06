#include "gtest/gtest.h"

#include "bus.h"
#include "cpu/cpu.h"

class CpuAddressingModeTest : public ::testing::Test
{
public:
  CpuAddressingModeTest() {
    bus = new Bus();
    cpu = new Cpu(bus);
  }

  ~CpuAddressingModeTest() 
  {
    delete cpu;
    delete bus;
  }
protected:
  Bus *bus;
  Cpu *cpu;

  void readData(unsigned char *data, int length)
  {
    bus->readData(data, length);
    bus->write_16(bus->RESET_VECTOR_ADDR, 0x8000);
    cpu->run();
  }
};

const unsigned char EXPECTED_RESULT = 0x40;

TEST_F(CpuAddressingModeTest, Immediate)
{
  // given
  unsigned char data[3] = {0xa9, 0x40, 0x00}; // LDA #40

  // when
  readData(data, 3);

  // then
  EXPECT_EQ(cpu->getA(), 0x40);
}

TEST_F(CpuAddressingModeTest, ZeroPage)
{
  // given
  bus->write_8(0x05, EXPECTED_RESULT);
  unsigned char data[3] = {0xa5, 0x05, 0x00}; // LDA $05

  // when
  readData(data, 3);

  // then
  EXPECT_EQ(cpu->getA(), EXPECTED_RESULT);
}

TEST_F(CpuAddressingModeTest, ZeroPageX)
{
  // given
  bus->write_8(0x85, EXPECTED_RESULT);
  unsigned char data[6] = {0xa2, 0x80, 0xb5, 0x05, 0x00}; // LDX #80; LDA $05,X 

  // when
  readData(data, 6);

  // then
  EXPECT_EQ(cpu->getA(), EXPECTED_RESULT);
}

TEST_F(CpuAddressingModeTest, ZeroPageX_Wraps)
{
  // given
  bus->write_8(0x02, EXPECTED_RESULT);
  unsigned char data[6] = {0xa2, 0xfe, 0xb5, 0x04, 0x00}; // LDX #fe; LDA $04,X 

  // when
  readData(data, 6);

  // then
  EXPECT_EQ(cpu->getA(), EXPECTED_RESULT);
}

TEST_F(CpuAddressingModeTest, ZeroPageY)
{
  // given
  bus->write_8(0x85, EXPECTED_RESULT);
  unsigned char data[6] = {0xa0, 0x80, 0xb6, 0x05, 0x00}; // LDY #80; LDX $05,Y 

  // when
  readData(data, 6);

  // then
  EXPECT_EQ(cpu->getX(), EXPECTED_RESULT);
}

TEST_F(CpuAddressingModeTest, ZeroPageY_Wraps)
{
  // given
  bus->write_8(0x00, EXPECTED_RESULT);
  unsigned char data[6] = {0xa0, 0xff, 0xb6, 0x01, 0x00}; // LDY #ff; LDX $01,Y 

  // when
  readData(data, 6);

  // then
  EXPECT_EQ(cpu->getX(), EXPECTED_RESULT);
}

TEST_F(CpuAddressingModeTest, Absolute)
{
  // given
  bus->write_8(0x1234, EXPECTED_RESULT);
  unsigned char data[4] = {0xad, 0x34, 0x12, 0x00}; // LDA $1234;

  // when
  readData(data, 4);

  // then
  EXPECT_EQ(cpu->getA(), EXPECTED_RESULT);
}

TEST_F(CpuAddressingModeTest, AbsoluteX)
{
  // given
  bus->write_8(0x1294, EXPECTED_RESULT);
  unsigned char data[6] = {0xa2, 0x60, 0xbd, 0x34, 0x12, 0x00}; // LDX #60, LDA $1234,X;

  // when
  readData(data, 6);

  // then
  EXPECT_EQ(cpu->getA(), EXPECTED_RESULT);
}

TEST_F(CpuAddressingModeTest, AbsoluteY)
{
  // given
  bus->write_8(0x1294, EXPECTED_RESULT);
  unsigned char data[6] = {0xa0, 0x60, 0xb9, 0x34, 0x12, 0x00}; // LDY #60, LDA $1234,Y;

  // when
  readData(data, 6);

  // then
  EXPECT_EQ(cpu->getA(), EXPECTED_RESULT);
}

TEST_F(CpuAddressingModeTest, Indirect)
{
  // given
  unsigned char jumpInstructions[3] = {0xa9, EXPECTED_RESULT, 0x00}; // LDA 
  bus->write_16(0xccbb, 0xddff);
  bus->write(0xddff, jumpInstructions, 3);
  unsigned char data[4] = {0x6c, 0xbb, 0xcc, 0x00}; // JMP ($1234);

  // when
  readData(data, 4);

  // then
  EXPECT_EQ(cpu->getA(), EXPECTED_RESULT);
}

TEST_F(CpuAddressingModeTest, IndexedIndirect)
{
  // given
  bus->write_16(0x00bc, 0xddff);
  bus->write_8(0xddff, EXPECTED_RESULT);
  unsigned char data[5] = {0xa2, 0x01, 0xa1, 0xbb, 0x00}; // LDX #01; LDA ($bb,X);

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getA(), EXPECTED_RESULT);
}

TEST_F(CpuAddressingModeTest, IndirectIndexed)
{
  // given
  bus->write_16(0x00bb, 0xddee);
  bus->write_8(0xddef, EXPECTED_RESULT);
  unsigned char data[5] = {0xa0, 0x01, 0xb1, 0xbb, 0x00}; // LDY #01; LDA ($bb),Y;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getA(), EXPECTED_RESULT);
}