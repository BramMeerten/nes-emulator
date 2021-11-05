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

TEST_F(CpuTest, ASL_accumulator)
{
  // given
  unsigned char data[4] = {0xa9, 0b1011'0011, 0x0a, 0x00}; // LDA #b3; ASL A;

  // when
  system.insertDisk(data, 4);

  // then
  EXPECT_EQ(system.cpu.getA(), 0b0110'0110);
  EXPECT_EQ(system.cpu.getStatus() & 0x01, 0x01); // carry = 1
}

TEST_F(CpuTest, ASL_zero_page)
{
  // given
  system.memory.write_8(0x0012, 0b0110'0110);
  unsigned char data[3] = {0x06, 0x12, 0x00}; // ASL $12;

  // when
  system.insertDisk(data, 3);

  // then
  EXPECT_EQ(system.cpu.getA(), 0b1100'1100);
  EXPECT_EQ(system.cpu.getStatus() & 0x01, 0x00); // carry = 0
}

TEST_F(CpuTest, CLC)
{
  // given
  unsigned char data[5] = {0xa9, 0xff, 0x0a, 0x18, 0x00}; // LDA #ff; CLC;

  // when
  system.insertDisk(data, 5);

  // then
  EXPECT_EQ(system.cpu.getStatus() & 0x0000'0001, 0);
}

TEST_F(CpuTest, BCC_forward)
{
  // given
  unsigned char data[10] = {0x18, 0x90, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa9, 0x70, 0x00}; // CLC; BCC *+4; BRK; BRK; BRK; BRK; LDA #70;

  // when
  system.insertDisk(data, 10);

  // then
  EXPECT_EQ(system.cpu.getA(), 0x70);
}

TEST_F(CpuTest, BCC_backward)
{
  // given
  unsigned char data[12] = {
    0x18,       // CLC;
    0x90, 0x04, // BCC *+4;   (Jump to X)
    0x00,       // BRK;
    0xa9, 0xff, // LDA #ff;   (Y)
    0x00,       // BRK;
    0xa9, 0xee, // LDA #ee;   (X)
    0x90, 0xf9, // BCC *-7;   (Jump to Y)
    0x00        // BRK;
    };

  // when
  system.insertDisk(data, 12);

  // then
  EXPECT_EQ(system.cpu.getA(), 0xff);
}

TEST_F(CpuTest, BCC_dont_jump)
{
  // given
  unsigned char data[11] = {0xa9, 0xff, 0x0a, 0x90, 0x03, 0xa9, 0xee, 0x00, 0xa9, 0xdd, 0x00}; // LDA #ff; ASL A; BCC *+3; LDA #ee; BRK; LDA #dd; BRK;

  // when
  system.insertDisk(data, 11);

  // then
  EXPECT_EQ(system.cpu.getA(), 0xee);
}

TEST_F(CpuTest, BCS_forward)
{
  // given
  unsigned char data[10] = {0x38, 0xb0, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa9, 0x70, 0x00}; // SEC; BCS *+4; BRK; BRK; BRK; BRK; LDA #70;

  // when
  system.insertDisk(data, 10);

  // then
  EXPECT_EQ(system.cpu.getA(), 0x70);
}

TEST_F(CpuTest, BCS_backword)
{
  // given
  unsigned char data[12] = {
    0x38,        // SEC; 
    0xb0, 0x04,  // BCS *+4;   (Jump to X)
    0x00,        // BRK;
    0xa9, 0x70,  // LDA #70;   (Y)
    0x00,        // BRK;
    0xa9, 0xff,  // LDA #FF;   (X)
    0xb0, 0xf9,  // BCS *-7;   (Jump to Y)
    0x00
  }; 
  
  // when
  system.insertDisk(data, 12);

  // then
  EXPECT_EQ(system.cpu.getA(), 0x70);
}

TEST_F(CpuTest, BCS_dont_jump)
{
  // given
  unsigned char data[9] = {0x18, 0xb0, 0x03, 0xa9, 0xee, 0x00, 0xa9, 0xdd, 0x00}; // CLC; BCC *+3; LDA #ee; BRK; LDA #dd; BRK;

  // when
  system.insertDisk(data, 9);

  // then
  EXPECT_EQ(system.cpu.getA(), 0xee);
}

TEST_F(CpuTest, BEQ_jump)
{
  // given
  unsigned char data[11] = {0xa9, 0x00, 0xf0, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa9, 0x70, 0x00}; // LDA #00; BEQ *+4; BRK; BRK; BRK; BRK; LDA #70;

  // when
  system.insertDisk(data, 11);

  // then
  EXPECT_EQ(system.cpu.getA(), 0x70);
}


TEST_F(CpuTest, BEQ_dont_jump)
{
  // given
  unsigned char data[11] = {0xa9, 0x01, 0xf0, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa9, 0x70, 0x00}; // LDA #01; BEQ *+4; BRK; BRK; BRK; BRK; LDA #70;

  // when
  system.insertDisk(data, 11);

  // then
  EXPECT_EQ(system.cpu.getA(), 0x01);
}

TEST_F(CpuTest, BIT_is_not_zero)
{
  // given
  system.memory.write_8(0x0012, 0b0011'0110);
  unsigned char data[5] = {
    0xa9, 0x18,     // LDA #0b0001'1000; 
    0x24, 0x12,     // BIT $12
    0x00
  }; 

  // when
  system.insertDisk(data, 5);

  // then
  EXPECT_EQ(system.cpu.getZero(), 0x0);
  EXPECT_EQ(system.cpu.getStatus() & 0b1100'0000, 0x0);
}

TEST_F(CpuTest, BIT_is_zero)
{
  // given
  system.memory.write_8(0x0012, 0b0010'0110);
  unsigned char data[5] = {
    0xa9, 0x18,     // LDA #0b0001'1000; 
    0x24, 0x12,     // BIT $12
    0x00
  }; 

  // when
  system.insertDisk(data, 5);

  // then
  EXPECT_EQ(system.cpu.getZero(), 0x01);
  EXPECT_EQ(system.cpu.getStatus() & 0b1100'0000, 0x0);
}

TEST_F(CpuTest, BIT_set_overflow_flag)
{
  // given
  system.memory.write_8(0x0012, 0b0100'1111);
  unsigned char data[3] = {
    0x24, 0x12,     // BIT $12
    0x00
  }; 

  // when
  system.insertDisk(data, 3);

  // then
  EXPECT_EQ(system.cpu.getStatus() & 0b1100'0000, 0b0100'0000);
}

TEST_F(CpuTest, BIT_set_negative_flag)
{
  // given
  system.memory.write_8(0x0012, 0b1000'1111);
  unsigned char data[3] = {
    0x24, 0x12,     // BIT $12
    0x00
  }; 

  // when
  system.insertDisk(data, 3);

  // then
  EXPECT_EQ(system.cpu.getStatus() & 0b1100'0000, 0b1000'0000);
}

TEST_F(CpuTest, BMI_jump)
{
  // given
  unsigned char data[11] = {0xa9, 0x80, 0x30, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa9, 0x70, 0x00}; // LDA #80; BEQ *+4; BRK; BRK; BRK; BRK; LDA #70;

  // when
  system.insertDisk(data, 11);

  // then
  EXPECT_EQ(system.cpu.getA(), 0x70);
}


TEST_F(CpuTest, BMI_dont_jump)
{
  // given
  unsigned char data[11] = {0xa9, 0x7f, 0x30, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa9, 0x70, 0x00}; // LDA #fe; BEQ *+4; BRK; BRK; BRK; BRK; LDA #70;

  // when
  system.insertDisk(data, 11);

  // then
  EXPECT_EQ(system.cpu.getA(), 0x7f);
}