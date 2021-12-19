#include "gtest/gtest.h"

#include "bus.h"
#include "cpu/cpu.h"

class CpuTwosComplementTest : public ::testing::Test
{
public:
  CpuTwosComplementTest() {
    bus = new Bus();
    cpu = new Cpu(bus);
  }

  ~CpuTwosComplementTest() 
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

unsigned char overflow(unsigned char status) {
  return (status & 0b0100'0000) >> 6;
}

unsigned char carry(unsigned char status) {
  return status & 0b0000'0001;
}

TEST_F(CpuTwosComplementTest, ADC_One)
{
  // LDA #01;    1+1=2;  C=0, V=0
  // ADC #01;
  unsigned char data[5] = {0xa9, 0x01, 0x69, 0x01, 0x00};

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getA(), 0x02);
  EXPECT_EQ(carry(cpu->getStatus()), 0);
  EXPECT_EQ(overflow(cpu->getStatus()), 0);
}

TEST_F(CpuTwosComplementTest, ADC_Two)
{
  // LDA #01;    1 + -1 = 0;  C=1, V=0
  // ADC #FF;
  unsigned char data[5] = {0xa9, 0x01, 0x69, 0xff, 0x00};

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getA(), 0);
  EXPECT_EQ(carry(cpu->getStatus()), 1);
  EXPECT_EQ(overflow(cpu->getStatus()), 0);
}

TEST_F(CpuTwosComplementTest, ADC_Three)
{
  // LDA #7F;    127 + 1 = 128;  C=0, V=1
  // ADC #01;
  unsigned char data[5] = {0xa9, 0x7f, 0x69, 0x01, 0x00};

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getA(), 128); // 128 (0x80) is -128 in two's complement -> overflow
  EXPECT_EQ(carry(cpu->getStatus()), 0);
  EXPECT_EQ(overflow(cpu->getStatus()), 1);
}

TEST_F(CpuTwosComplementTest, ADC_Four)
{
  // LDA #80;    -128 + -1 = -129,  C=1, V=1
  // ADC #FF;
  unsigned char data[5] = {0xa9, 0x80, 0x69, 0xff, 0x00};

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getA(), 127); // +127 instead of -129 -> overflow
  EXPECT_EQ(overflow(cpu->getStatus()), 1);
  EXPECT_EQ(carry(cpu->getStatus()), 1);
}

TEST_F(CpuTwosComplementTest, ADC_Five)
{
  // SEC;        63 + 64 + 1 = 128,   C=0, V=1
  // LDA #3F; 
  // ADC #40;
  unsigned char data[6] = {0x38, 0xa9, 0x3f, 0x69, 0x40, 0x00};

  // when
  readData(data, 6);

  // then
  EXPECT_EQ(cpu->getA(), 128); // 128 (0x80) is -128 in two's complement -> overflow
  EXPECT_EQ(carry(cpu->getStatus()), 0);
  EXPECT_EQ(overflow(cpu->getStatus()), 1);
}

// ==============
//   SBC tests
// ==============
// 1: pos - pos | no carry
// 2: pos - pos | carry
// 3: pos - neg | no carry
// 4: pos - neg | carry
// 5: neg - pos | no carry
// 6: neg - pos | carry
// 7: neg - neg | no carry
// 8: neg - neg | carry

TEST_F(CpuTwosComplementTest, SBC_One)
{
  // LDA #00; = 0                0 - 127 - 1 (carry is 0) = -128;  C=0, V=0
  // SBC #7f; = 127
  unsigned char data[5] = {0xa9, 0x00, 0xe9, 0x7f, 0x00};

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getA(), 0x80); // -128
  EXPECT_EQ(carry(cpu->getStatus()), 0);
  EXPECT_EQ(overflow(cpu->getStatus()), 0);
}

TEST_F(CpuTwosComplementTest, SBC_Two)
{
  // LDA #01; CMP #00; carry=1
  // LDA #00; = 0                 0 - 127 - 0 (carry is 1) = -127;  C=0, V=0
  // SBC #7f; = 127
  unsigned char data[9] = {0xa9, 0x01, 0xc9, 0x00, 0xa9, 0x00, 0xe9, 0x7f, 0x00};

  // when
  readData(data, 9);

  // then
  EXPECT_EQ(cpu->getA(), 0x81); // -127
  EXPECT_EQ(carry(cpu->getStatus()), 0);
  EXPECT_EQ(overflow(cpu->getStatus()), 0);
}

TEST_F(CpuTwosComplementTest, SBC_THREE)
{
  // LDA #7f; = 127               127 - -3 - 1 (carry is 0) = 129;  C=0, V=1
  // SBC #fd; = -3
  unsigned char data[5] = {0xa9, 0x7f, 0xe9, 0xfd, 0x00};

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getA(), 0x81); // -127 instead of 129
  EXPECT_EQ(carry(cpu->getStatus()), 0);
  EXPECT_EQ(overflow(cpu->getStatus()), 1);
}

TEST_F(CpuTwosComplementTest, SBC_Four)
{
  // LDA #01; CMP #00; carry=1
  // LDA #7f; = 127               127 - -3 - 0 (carry is 1) = 130;  C=0, V=1
  // SBC #fd; = -3
  unsigned char data[9] = {0xa9, 0x01, 0xc9, 0x00, 0xa9, 0x7f, 0xe9, 0xfd, 0x00};

  // when
  readData(data, 9);

  // then
  EXPECT_EQ(cpu->getA(), 0x82); // -126 instead of 130
  EXPECT_EQ(carry(cpu->getStatus()), 0);
  EXPECT_EQ(overflow(cpu->getStatus()), 1);
}

TEST_F(CpuTwosComplementTest, SBC_FIVE)
{
  // LDA #92; = -110               -110 - 120 - 1 (carry is 0) = -231;  C=1, V=1
  // SBC #78; = 120
  unsigned char data[5] = {0xa9, 0x92, 0xe9, 0x78, 0x00};

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getA(), 0x19); // 25 instead of -231
  EXPECT_EQ(carry(cpu->getStatus()), 1);
  EXPECT_EQ(overflow(cpu->getStatus()), 1);
}

TEST_F(CpuTwosComplementTest, SBC_SIX)
{
  // LDA #01; CMP #00; carry=1
  // LDA #92; = -110               -110 - 120 - 0 (carry is 1) = -230;  C=1, V=1
  // SBC #78; = 120
  unsigned char data[9] = {0xa9, 0x01, 0xc9, 0x00, 0xa9, 0x92, 0xe9, 0x78, 0x00};

  // when
  readData(data, 9);

  // then
  EXPECT_EQ(cpu->getA(), 0x1a); // 26 instead of -230
  EXPECT_EQ(carry(cpu->getStatus()), 1);
  EXPECT_EQ(overflow(cpu->getStatus()), 1);
}

TEST_F(CpuTwosComplementTest, SBC_SEVEN)
{
  // LDA #ff; = -1                -1 - -128 - 1 (carry is 0) = 126;  C=1, V=0
  // SBC #80; = -128
  unsigned char data[5] = {0xa9, 0xff, 0xe9, 0x80, 0x00};

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getA(), 0x7e); // 126
  EXPECT_EQ(carry(cpu->getStatus()), 1);
  EXPECT_EQ(overflow(cpu->getStatus()), 0);
}

TEST_F(CpuTwosComplementTest, SBC_TEN)
{
  // LDA #01; CMP #00; carry=1
  // LDA #ff; = -1                 -1 - -128 - 0 (carry is 1) = -127;  C=1, V=0
  // SBC #80; = -128
  unsigned char data[9] = {0xa9, 0x01, 0xc9, 0x00, 0xa9, 0xff, 0xe9, 0x80, 0x00};

  // when
  readData(data, 9);

  // then
  EXPECT_EQ(cpu->getA(), 0x7f); // 127
  EXPECT_EQ(carry(cpu->getStatus()), 1);
  EXPECT_EQ(overflow(cpu->getStatus()), 0);
}