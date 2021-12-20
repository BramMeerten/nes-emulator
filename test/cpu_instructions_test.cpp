#include "gtest/gtest.h"

#include "bus.h"
#include "cpu/cpu.h"

class CpuTest : public ::testing::Test // TODO extract to super class
{
public:
  CpuTest() {
    bus = new Bus();
    cpu = new Cpu(bus);
  }

  ~CpuTest() 
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

TEST_F(CpuTest, LdaSetsRegisterA)
{
  // given
  unsigned char data[3] = {0xa9, 0b0100'0000, 0x00}; // LDA #40

  // when
  readData(data, 3);

  // then
  EXPECT_EQ(cpu->getA(), 0x40);
  EXPECT_EQ(cpu->getStatus(), 0);
}

TEST_F(CpuTest, LdaSetsZeroFlag)
{ 
  // given
  unsigned char data[3] = {0xa9, 0x00, 0x00}; // LDA 00

  // when
  readData(data, 3);

  // then
  EXPECT_EQ(cpu->getA(), 0);
  EXPECT_EQ(cpu->getStatus(), 0b0000'0010);
}

TEST_F(CpuTest, LdaSetsNegativeFlag)
{
  // given
  unsigned char data[3] = {0xa9, 0b1100'0000, 0x00}; // LDA c0

  // when
  readData(data, 3);

  // then
  EXPECT_EQ(cpu->getA(), 0xc0);
  EXPECT_EQ(cpu->getStatus(), 0b1000'0000);
}

TEST_F(CpuTest, TaxSetsXRegister)
{
  // given
  unsigned char data[4] = {0xa9, 0x80, 0xaa, 0x00}; // LDA #80; TAX;

  // when
  readData(data, 4);

  // then
  EXPECT_EQ(cpu->getX(), 0x80);
  EXPECT_EQ(cpu->getStatus(), 0b1000'0000);
}

TEST_F(CpuTest, TAY)
{
  // given
  unsigned char data[4] = {0xa9, 0x80, 0xa8, 0x00}; // LDA #80; TAY;

  // when
  readData(data, 4);

  // then
  EXPECT_EQ(cpu->getY(), 0x80);
  EXPECT_EQ(cpu->getStatus(), 0b1000'0000);
}

TEST_F(CpuTest, InxIncreasesTheXRegister)
{
  // given
  unsigned char data[5] = {0xa2, 0x80, 0xe8, 0x00}; // LDX #80; INX

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getX(), 0x81);
  EXPECT_EQ(cpu->getStatus(), 0b1000'0000);
}

TEST_F(CpuTest, InxIncreasesTheYRegister)
{
  // given
  unsigned char data[5] = {0xa0, 0x80, 0xc8, 0x00}; // LDY #80; INY

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getY(), 0x81);
  EXPECT_EQ(cpu->getStatus(), 0b1000'0000);
}

TEST_F(CpuTest, LdxSetsRegisterX)
{
  // given
  unsigned char data[3] = {0xa2, 0x40, 0x00}; // LDX #40

  // when
  readData(data, 3);

  // then
  EXPECT_EQ(cpu->getX(), 0x40);
}

TEST_F(CpuTest, LdySetsRegisterY)
{
  // given
  unsigned char data[3] = {0xa0, 0x40, 0x00}; // LDY #40

  // when
  readData(data, 3);

  // then
  EXPECT_EQ(cpu->getY(), 0x40);
}

TEST_F(CpuTest, And)
{
  // given
  unsigned char data[5] = {0xa9, 0b0011'0011, 0x29, 0b1010'0001, 0x00}; // LDA #33; AND #a1;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getA(), 0b0010'0001);
}

TEST_F(CpuTest, ORA)
{
  // given
  unsigned char data[5] = {0xa9, 0b0011'0011, 0x09, 0b1010'0001, 0x00}; // LDA #33; ORA #a1;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getA(), 0b1011'0011);
}

TEST_F(CpuTest, EOR)
{
  // given
  unsigned char data[5] = {0xa9, 0b0011'0011, 0x49, 0b1010'0001, 0x00}; // LDA #33; ORA #a1;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getA(), 0b1001'0010);
}

TEST_F(CpuTest, Adc)
{
  // given
  unsigned char data[5] = {0xa9, 0x33, 0x69, 0x12, 0x00}; // LDA #33; ADC #12;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getA(), 0x45);
  EXPECT_EQ(cpu->getStatus(), 0b0000'0000); // carry = 0
}

TEST_F(CpuTest, Adc_set_carry)
{
  // given
  unsigned char data[5] = {0xa9, 0xff, 0x69, 0xff, 0x00}; // LDA #ff; ADC #ff;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getA(), 0xfe); // #ff + #ff = #1FE 
  EXPECT_EQ(cpu->getStatus(), 0b1000'0001); // carry = 1
}

TEST_F(CpuTest, Adc_adds_carry)
{
  // given
  unsigned char data[9] = {0xa9, 0xff, 0x69, 0xff, 0xa9, 0xff, 0x69, 0xff, 0x00}; // LDA #ff; ADC #ff; LDA #ff; ADC #ff;

  // when
  readData(data, 9);

  // then
  EXPECT_EQ(cpu->getA(), 0xff); // #ff + #ff + #01 = #1FF 
  EXPECT_EQ(cpu->getStatus(), 0b1000'0001); // carry = 1
}

TEST_F(CpuTest, Sec)
{
  // given
  unsigned char data[2] = {0x38, 0x00}; // SEC;

  // when
  readData(data, 2);

  // then
  EXPECT_EQ(cpu->getStatus(), 0b0000'0001); // carry = 1
}

TEST_F(CpuTest, Sed)
{
  // given
  unsigned char data[2] = {0xf8, 0x00}; // SED;

  // when
  readData(data, 2);

  // then
  EXPECT_EQ(cpu->getStatus(), 0b0000'1000); // decimal = 1
}

TEST_F(CpuTest, Sei)
{
  // given
  unsigned char data[2] = {0x78, 0x00}; // SEI;

  // when
  readData(data, 2);

  // then
  EXPECT_EQ(cpu->getStatus(), 0b0000'0100); // interrupt = 1
}

TEST_F(CpuTest, Cli)
{
  // given
  unsigned char data[3] = {0x78, 0x58, 0x00}; // CLI;

  // when
  readData(data, 3);

  // then
  EXPECT_EQ(cpu->getStatus(), 0b0000'0000);
}

TEST_F(CpuTest, Clv)
{
  // given
  bus->write_8(0x0012, 0b0100'1111);
  unsigned char data[4] = {0x24, 0x12, 0xb8, 0x00}; // BIT $12; CLV;

  // when
  readData(data, 4);

  // then
  EXPECT_EQ(cpu->getStatus(), 0b0000'0010); // V is cleared, zero flag is still set because 0x12 & 0x00 = 0x00
}

TEST_F(CpuTest, ASL_accumulator)
{
  // given
  unsigned char data[4] = {0xa9, 0b1011'0011, 0x0a, 0x00}; // LDA #b3; ASL A;

  // when
  readData(data, 4);

  // then
  EXPECT_EQ(cpu->getA(), 0b0110'0110);
  EXPECT_EQ(cpu->getStatus() & 0x01, 0x01); // carry = 1
}

TEST_F(CpuTest, ASL_zero_page)
{
  // given
  bus->write_8(0x12, 0b0110'0110);
  unsigned char data[3] = {0x06, 0x12, 0x00}; // ASL $12;

  // when
  readData(data, 3);

  // then
  EXPECT_EQ(bus->read(0x12), 0b1100'1100);
  EXPECT_EQ(cpu->getStatus() & 0x01, 0x00); // carry = 0
}

TEST_F(CpuTest, ASL_set_carry_zero)
{
  // given
  unsigned char data[5] = {0x38, 0xa9, 0b0011'0011, 0x0a, 0x00}; // SEC; LDA #b3; ASL A;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getA(), 0b0110'0110);
  EXPECT_EQ(cpu->getStatus() & 0x01, 0x00); // carry = 0
}

TEST_F(CpuTest, LSR_accumulator)
{
  // given
  unsigned char data[4] = {0xa9, 0b0011'0011, 0x4a, 0x00}; // LDA #0011'0011; LSR A;

  // when
  readData(data, 4);

  // then
  EXPECT_EQ(cpu->getA(), 0b0001'1001);
  EXPECT_EQ(cpu->getStatus() & 0x01, 0x01); // carry = 1
}

TEST_F(CpuTest, LSR_zero_page)
{
  // given
  bus->write_8(0x12, 0b0110'0110);
  unsigned char data[4] = {0x38, 0x46, 0x12, 0x00}; // SEC; LSR $12;

  // when
  readData(data, 4);

  // then
  EXPECT_EQ(bus->read(0x12), 0b0011'0011);
  EXPECT_EQ(cpu->getStatus() & 0x01, 0x00); // carry = 0
}

TEST_F(CpuTest, CLC)
{
  // given
  unsigned char data[5] = {0xa9, 0xff, 0x0a, 0x18, 0x00}; // LDA #ff; CLC;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getStatus() & 0x0000'0001, 0);
}

TEST_F(CpuTest, CLD)
{
  // given
  unsigned char data[3] = {0xf8, 0xd8, 0x00}; // SED; CLD;

  // when
  readData(data, 3);

  // then
  EXPECT_EQ(cpu->getStatus() & 0x0000'0000, 0);
}

TEST_F(CpuTest, BCC_forward)
{
  // given
  unsigned char data[10] = {0x18, 0x90, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa9, 0x70, 0x00}; // CLC; BCC *+4; BRK; BRK; BRK; BRK; LDA #70;

  // when
  readData(data, 10);

  // then
  EXPECT_EQ(cpu->getA(), 0x70);
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
  readData(data, 12);

  // then
  EXPECT_EQ(cpu->getA(), 0xff);
}

TEST_F(CpuTest, BCC_dont_jump)
{
  // given
  unsigned char data[11] = {0xa9, 0xff, 0x0a, 0x90, 0x03, 0xa9, 0xee, 0x00, 0xa9, 0xdd, 0x00}; // LDA #ff; ASL A; BCC *+3; LDA #ee; BRK; LDA #dd; BRK;

  // when
  readData(data, 11);

  // then
  EXPECT_EQ(cpu->getA(), 0xee);
}

TEST_F(CpuTest, BCS_forward)
{
  // given
  unsigned char data[10] = {0x38, 0xb0, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa9, 0x70, 0x00}; // SEC; BCS *+4; BRK; BRK; BRK; BRK; LDA #70;

  // when
  readData(data, 10);

  // then
  EXPECT_EQ(cpu->getA(), 0x70);
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
  readData(data, 12);

  // then
  EXPECT_EQ(cpu->getA(), 0x70);
}

TEST_F(CpuTest, BCS_dont_jump)
{
  // given
  unsigned char data[9] = {0x18, 0xb0, 0x03, 0xa9, 0xee, 0x00, 0xa9, 0xdd, 0x00}; // CLC; BCC *+3; LDA #ee; BRK; LDA #dd; BRK;

  // when
  readData(data, 9);

  // then
  EXPECT_EQ(cpu->getA(), 0xee);
}

TEST_F(CpuTest, BEQ_jump)
{
  // given
  unsigned char data[11] = {0xa9, 0x00, 0xf0, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa9, 0x70, 0x00}; // LDA #00; BEQ *+4; BRK; BRK; BRK; BRK; LDA #70;

  // when
  readData(data, 11);

  // then
  EXPECT_EQ(cpu->getA(), 0x70);
}


TEST_F(CpuTest, BEQ_dont_jump)
{
  // given
  unsigned char data[11] = {0xa9, 0x01, 0xf0, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa9, 0x70, 0x00}; // LDA #01; BEQ *+4; BRK; BRK; BRK; BRK; LDA #70;

  // when
  readData(data, 11);

  // then
  EXPECT_EQ(cpu->getA(), 0x01);
}

TEST_F(CpuTest, BIT_is_not_zero)
{
  // given
  bus->write_8(0x0012, 0b0011'0110);
  unsigned char data[5] = {
    0xa9, 0x18,     // LDA #0b0001'1000; 
    0x24, 0x12,     // BIT $12
    0x00
  }; 

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getZero(), 0x0);
  EXPECT_EQ(cpu->getStatus() & 0b1100'0000, 0x0);
}

TEST_F(CpuTest, BIT_is_zero)
{
  // given
  bus->write_8(0x0012, 0b0010'0110);
  unsigned char data[5] = {
    0xa9, 0x18,     // LDA #0b0001'1000; 
    0x24, 0x12,     // BIT $12
    0x00
  }; 

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getZero(), 0x01);
  EXPECT_EQ(cpu->getStatus() & 0b1100'0000, 0x0);
}

TEST_F(CpuTest, BIT_set_overflow_flag)
{
  // given
  bus->write_8(0x0012, 0b0100'1111);
  unsigned char data[3] = {
    0x24, 0x12,     // BIT $12
    0x00
  }; 

  // when
  readData(data, 3);

  // then
  EXPECT_EQ(cpu->getStatus() & 0b1100'0000, 0b0100'0000);
}

TEST_F(CpuTest, BIT_set_negative_flag)
{
  // given
  bus->write_8(0x0012, 0b1000'1111);
  unsigned char data[3] = {
    0x24, 0x12,     // BIT $12
    0x00
  }; 

  // when
  readData(data, 3);

  // then
  EXPECT_EQ(cpu->getStatus() & 0b1100'0000, 0b1000'0000);
}

TEST_F(CpuTest, BMI_jump)
{
  // given
  unsigned char data[11] = {0xa9, 0x80, 0x30, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa9, 0x70, 0x00}; // LDA #80; BMI *+4; BRK; BRK; BRK; BRK; LDA #70;

  // when
  readData(data, 11);

  // then
  EXPECT_EQ(cpu->getA(), 0x70);
}


TEST_F(CpuTest, BMI_dont_jump)
{
  // given
  unsigned char data[11] = {0xa9, 0x7f, 0x30, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa9, 0x70, 0x00}; // LDA #fe; BMI *+4; BRK; BRK; BRK; BRK; LDA #70;

  // when
  readData(data, 11);

  // then
  EXPECT_EQ(cpu->getA(), 0x7f);
}

TEST_F(CpuTest, BNE_dont_jump)
{
  // given
  unsigned char data[11] = {0xa9, 0x00, 0xd0, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa9, 0x70, 0x00}; // LDA #00; BNE *+4; BRK; BRK; BRK; BRK; LDA #70;

  // when
  readData(data, 11);

  // then
  EXPECT_EQ(cpu->getA(), 0x00);
}


TEST_F(CpuTest, BNE_jump)
{
  // given
  unsigned char data[11] = {0xa9, 0x01, 0xd0, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa9, 0x70, 0x00}; // LDA #01; BNE *+4; BRK; BRK; BRK; BRK; LDA #70;

  // when
  readData(data, 11);

  // then
  EXPECT_EQ(cpu->getA(), 0x70);
}

TEST_F(CpuTest, BPL_dont_jump)
{
  // given
  unsigned char data[11] = {0xa9, 0x80, 0x10, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa9, 0x70, 0x00}; // LDA #80; BPL *+4; BRK; BRK; BRK; BRK; LDA #70;

  // when
  readData(data, 11);

  // then
  EXPECT_EQ(cpu->getA(), 0x80);
}


TEST_F(CpuTest, BPL_jump)
{
  // given
  unsigned char data[11] = {0xa9, 0x7f, 0x10, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa9, 0x70, 0x00}; // LDA #fe; BPL *+4; BRK; BRK; BRK; BRK; LDA #70;

  // when
  readData(data, 11);

  // then
  EXPECT_EQ(cpu->getA(), 0x70);
}

TEST_F(CpuTest, BVC_dont_jump)
{
  // given
  bus->write_8(0x0012, 0b0100'1111);
  unsigned char data[11] = {0x24, 0x12, 0x50, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa9, 0x70, 0x00}; // BIT $12; BVC *+4; BRK; BRK; BRK; BRK; LDA #70;

  // when
  readData(data, 11);

  // then
  EXPECT_EQ(cpu->getA(), 0x00);
}


TEST_F(CpuTest, BVC_jump)
{
  // given
  unsigned char data[9] = {0x50, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa9, 0x70, 0x00}; // BVC *+4; BRK; BRK; BRK; BRK; LDA #70;

  // when
  readData(data, 9);

  // then
  EXPECT_EQ(cpu->getA(), 0x70);
}

TEST_F(CpuTest, BVS_jump)
{
  // given
  bus->write_8(0x0012, 0b0100'1111);
  unsigned char data[11] = {0x24, 0x12, 0x70, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa9, 0x70, 0x00}; // BIT $12; BVS *+4; BRK; BRK; BRK; BRK; LDA #70;

  // when
  readData(data, 11);

  // then
  EXPECT_EQ(cpu->getA(), 0x70);
}


TEST_F(CpuTest, BVS_dont_jump)
{
  // given
  unsigned char data[9] = {0x70, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa9, 0x70, 0x00}; // BVS *+4; BRK; BRK; BRK; BRK; LDA #70;

  // when
  readData(data, 9);

  // then
  EXPECT_EQ(cpu->getA(), 0x00);
}

TEST_F(CpuTest, CMP_equals)
{
  // given
  unsigned char data[5] = {0xa9, 0x12, 0xc9, 0x12, 0x00}; // LDA #12; CMP #12; BRK;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getStatus() & 0b1000'0011, 0b0000'0011);
}

TEST_F(CpuTest, CMP_less_then)
{
  // given
  unsigned char data[5] = {0xa9, 0x12, 0xc9, 0x14, 0x00}; // LDA #12; CMP #14; BRK;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getStatus() & 0b1000'0011, 0b1000'0000);
}

TEST_F(CpuTest, CMP_greater)
{
    // given
  unsigned char data[5] = {0xa9, 0x12, 0xc9, 0x09, 0x00}; // LDA #12; CMP #09; BRK;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getStatus() & 0b1000'0011, 0b0000'0001);
}

TEST_F(CpuTest, CPX_equals)
{
  // given
  unsigned char data[5] = {0xa2, 0x12, 0xe0, 0x12, 0x00}; // LDX #12; CPX #12; BRK;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getStatus() & 0b1000'0011, 0b0000'0011);
}

TEST_F(CpuTest, CPX_less_then)
{
  // given
  unsigned char data[5] = {0xa2, 0x12, 0xe0, 0x14, 0x00}; // LDX #12; CPX #14; BRK;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getStatus() & 0b1000'0011, 0b1000'0000);
}

TEST_F(CpuTest, CPX_greater)
{
    // given
  unsigned char data[5] = {0xa2, 0x12, 0xe0, 0x09, 0x00}; // LDX #12; CPX #09; BRK;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getStatus() & 0b1000'0011, 0b0000'0001);
}

TEST_F(CpuTest, CPY_equals)
{
  // given
  unsigned char data[5] = {0xa0, 0x12, 0xc0, 0x12, 0x00}; // LDY #12; CPY #12; BRK;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getStatus() & 0b1000'0011, 0b0000'0011);
}

TEST_F(CpuTest, CPY_less_then)
{
  // given
  unsigned char data[5] = {0xa0, 0x12, 0xc0, 0x14, 0x00}; // LDY #12; CPY #14; BRK;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getStatus() & 0b1000'0011, 0b1000'0000);
}

TEST_F(CpuTest, CPY_greater)
{
    // given
  unsigned char data[5] = {0xa0, 0x12, 0xc0, 0x09, 0x00}; // LDY #12; CPY #09; BRK;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getStatus() & 0b1000'0011, 0b0000'0001);
}

TEST_F(CpuTest, NOP)
{
  // given
  unsigned char data[4] = {0xea, 0xa9, 0x03, 0x00}; // NOP; LDA #03

  // when
  readData(data, 4);

  // then
  EXPECT_EQ(cpu->getA(), 0x03);
}

TEST_F(CpuTest, PHA)
{
  // given
  unsigned char data[7] = {0xa9, 0x15, 0x48, 0xa9, 0xa1, 0x48, 0x00}; // LDA #15; PHA; LDA #a1; PHA

  // when
  readData(data, 7);

  // then
  EXPECT_EQ(bus->read(0x01fd), 0x15);
  EXPECT_EQ(bus->read(0x01fc), 0xa1);
}

TEST_F(CpuTest, PHP)
{
  // given
  unsigned char data[4] = {0x38, 0xf8, 0x08, 0x00}; // SEC; SED; PHP

  // when
  readData(data, 4);

  // then
  EXPECT_EQ(bus->read(0x01fd), 0b0011'1001);
}

TEST_F(CpuTest, PLA)
{
  // given
  unsigned char data[10] = {
    0xa9, 0x15, 0x48,  // LDA #15; PHA; 
    0xa9, 0xa1, 0x48,  // LDA #a1; PHA
    0xa9, 0x10,        // LDA #10;
    0x68, 0x00};       // PLA

  // when
  readData(data, 10);

  // then
  EXPECT_EQ(bus->read(0x01fd), 0x15);
  EXPECT_EQ(cpu->getA(), 0xa1);
}

// Rotate so much data that stack rotates from 0x0100 to 0x01ff
// Pull twice so stack rotates from 0x01ff to 0x0100
TEST_F(CpuTest, STACK_ROTATES)
{
  // given
  unsigned char data[(3 * 255) + 2 + 2 + 1];
  int b = 0;

  // push 0x15 to stack first (addr 0xfd)
  data[b++] = 0xa9; data[b++] = 0x15; data[b++] = 0x48; // LDA #15; PHA; 

  // keep pushing until is about to rotate (minus 1)
  for (int i=0; i<252; i++) {
    data[b++] = 0xa9; data[b++] = 0x22; data[b++] = 0x48; // LDA #22; PHA; 
  }

  // push 0x16 right before rotate
  data[b++] = 0xa9; data[b++] = 0x16; data[b++] = 0x48; // LDA #16; PHA; 

  // push 0x17 after rotate
  data[b++] = 0xa9; data[b++] = 0x17; data[b++] = 0x48; // LDA #17; PHA; 

  // pull twice and write to a register
  data[b++] = 0xa9; data[b++] = 0x01; // LDA #01;
  data[b++] = 0x68; data[b++] = 0x68; // PLA; PLA;
  data[b++] = 0x00;

  // when
  readData(data, (3 * 257) + 2 + 2 + 1);

  // then
  EXPECT_EQ(bus->read(0x01ff), 0x17);
  EXPECT_EQ(bus->read(0x01fd), 0x15);
  EXPECT_EQ(bus->read(0x01fc), 0x22);
  EXPECT_EQ(bus->read(0x0100), 0x16);
  EXPECT_EQ(cpu->getA(), 0x16);
}

TEST_F(CpuTest, PLP)
{
  // given
  unsigned char data[7] = {0x38, 0xf8, 0x08, 0x18, 0xd8, 0x28, 0x00}; // SEC; SED; PHP; CLC; CLD; PLP;

  // when
  readData(data, 7);

  // then
  EXPECT_EQ(cpu->getStatus(), 0b0000'1001);
}

TEST_F(CpuTest, ROL_ACCUMULATOR_NO_CARRY)
{
  // given
  unsigned char data[4] = {0xa9, 0b1001'0101, 0x2a, 0x00}; // LDA #0b1001'0101; ROL A;

  // when
  readData(data, 4);

  // then
  EXPECT_EQ(cpu->getA(), 0b0010'1010);
  EXPECT_EQ(cpu->getStatus() & 0x01, 1); // carry == 1
}

TEST_F(CpuTest, ROL_ACCUMULATOR_CARRY)
{
  // given
  unsigned char data[5] = {0x38, 0xa9, 0b0111'0101, 0x2a, 0x00}; // SEC; LDA #0b0111'0101; ROL A;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getA(), 0b1110'1011);
  EXPECT_EQ(cpu->getStatus() & 0x01, 0); // carry == 0
}

TEST_F(CpuTest, ROL_ZERO_PAGE)
{
  // given
  bus->write_8(0x12, 0b1110'0110);
  unsigned char data[3] = {0x26, 0x12, 0x00}; // ROL $12;

  // when
  readData(data, 3);

  // then
  EXPECT_EQ(bus->read(0x12), 0b1100'1100);
  EXPECT_EQ(cpu->getStatus() & 0x01, 1); // carry == 1
}

TEST_F(CpuTest, ROR_ACCUMULATOR_NO_CARRY)
{
  // given
  unsigned char data[4] = {0xa9, 0b0111'0101, 0x6a, 0x00}; // LDA #0b0111'0101; ROR A;

  // when
  readData(data, 4);

  // then
  EXPECT_EQ(cpu->getA(), 0b0011'1010);
  EXPECT_EQ(cpu->getStatus() & 0x01, 1); // carry == 1
}

TEST_F(CpuTest, ROR_ACCUMULATOR_CARRY)
{
  // given
  unsigned char data[5] = {0x38, 0xa9, 0b0111'0100, 0x6a, 0x00}; // SEC; LDA #0b0111'0101; ROR A;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getA(), 0b1011'1010);
  EXPECT_EQ(cpu->getStatus() & 0x01, 0); // carry == 0
}

TEST_F(CpuTest, ROR_ZERO_PAGE)
{
  // given
  bus->write_8(0x12, 0b1110'0110);
  unsigned char data[3] = {0x66, 0x12, 0x00}; // ROR $12;

  // when
  readData(data, 3);

  // then
  EXPECT_EQ(bus->read(0x12), 0b0111'0011);
  EXPECT_EQ(cpu->getStatus() & 0x01, 0); // carry == 0
}

TEST_F(CpuTest, RTI)
{
  // given
  bus->write_8(0x9012, 0x00);
  unsigned char data[10] = {
    0xa9, 0x90, 0x48, 0xa9, 0x12, 0x48,   // LDA #90; PHA; LDA #12; PHA; --> PC 0x9012
    0xa9, 0x32, 0x48,                     // LDA #32; PHA;               --> 0x32 = 0b0011'0010 -> status = 0b0000'0010;
    0x40                                  // RTI
  };

  // when
  readData(data, 10);

  // then
  EXPECT_EQ(cpu->getPC(), 0x9012);
  EXPECT_EQ(cpu->getStatus(), 0b0000'0010);
}

TEST_F(CpuTest, JSR_RTS)
{
  // given
  unsigned char subroutine[5] = {0xa0, 0x13, 0xa9, 0x88, 0x60}; // LDY #13; LDA #88; RTS;
  bus->write(0x9021, subroutine, 5);
  unsigned char data[8] = {
      0xa2, 0x12, 0x20, 0x21, 0x90, 0xa9, 0x14, 0x00 // LDX #12, JSR $9021, LDA #14;
  };

  // when
  readData(data, 8);

  // then
  EXPECT_EQ(cpu->getX(), 0x12);
  EXPECT_EQ(cpu->getY(), 0x13);
  EXPECT_EQ(cpu->getA(), 0x14);
}

TEST_F(CpuTest, SBC_set_carry)
{
  // given
  unsigned char data[5] = {0xa9, 0x33, 0xe9, 0x12, 0x00}; // LDA #33; SBC #12;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getA(), 0x20);
  EXPECT_EQ(cpu->getStatus(), 0b0000'0001); // carry = 1
}

TEST_F(CpuTest, SBC_clear_carry)
{
  // given
  unsigned char data[5] = {0xa9, 0x12, 0xe9, 0x33, 0x00}; // LDA #12; SBC #33;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(cpu->getA(), 0xde);
  EXPECT_EQ(cpu->getStatus(), 0b1000'0000); // overflow = 0, carry = 0
}

TEST_F(CpuTest, SBC_carry_is_set)
{
  // given
  unsigned char data[6] = {0x38, 0xa9, 0x33, 0xe9, 0x12, 0x00}; // SEC; LDA #33; SBC #12;

  // when
  readData(data, 6);

  // then
  EXPECT_EQ(cpu->getA(), 0x21);
  EXPECT_EQ(cpu->getStatus(), 0b0000'0001); // carry = 1
}

TEST_F(CpuTest, STA)
{
  // given
  unsigned char data[5] = {0xa9, 0x33, 0x85, 0x99, 0x00}; // LDA #33; STA $99;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(bus->read(0x99), 0x33);
}

TEST_F(CpuTest, STX)
{
  // given
  unsigned char data[5] = {0xa2, 0x33, 0x86, 0x99, 0x00}; // LDX #33; STX $99;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(bus->read(0x99), 0x33);
}

TEST_F(CpuTest, STY)
{
  // given
  unsigned char data[5] = {0xa0, 0x33, 0x84, 0x99, 0x00}; // LDY #33; STY $99;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(bus->read(0x99), 0x33);
}

TEST_F(CpuTest, TSX)
{
  // given
  unsigned char data[3] = {0x48, 0xba, 0x00}; // PHA; TSX;

  // when
  readData(data, 3);

  // then
  EXPECT_EQ(cpu->getX(), 0xfc); // sp initial value is #fd, pushed one item to stack --> #fc
  EXPECT_EQ(cpu->getStatus(), 0b1000'0000);
}

TEST_F(CpuTest, TXA)
{
  // given
  unsigned char data[4] = {0xa2, 0x14, 0x8a, 0x00}; // LDX #14; TXA;

  // when
  readData(data, 4);

  // then
  EXPECT_EQ(cpu->getA(), 0x14); 
  EXPECT_EQ(cpu->getStatus(), 0b0000'0000);
}

TEST_F(CpuTest, TXS)
{
  // given
  unsigned char data[4] = {0xa2, 0x33, 0x9a, 0x00}; // LDX #14; TXS;

  // when
  readData(data, 4);

  // then
  EXPECT_EQ(cpu->getSP(), 0x33);
}

TEST_F(CpuTest, TYA)
{
  // given
  unsigned char data[4] = {0xa0, 0xfe, 0x98, 0x00}; // LDY #FE; TYA;

  // when
  readData(data, 4);

  // then
  EXPECT_EQ(cpu->getA(), 0xfe); 
  EXPECT_EQ(cpu->getStatus(), 0b1000'0000);
}

TEST_F(CpuTest, DEC)
{
  // given
  unsigned char data[7] = {0xa9, 0xfd, 0x85, 0x54, 0xc6, 0x54, 0x00}; // LDA #fd; STA $54; DEC $54;

  // when
  readData(data, 7);

  // then
  EXPECT_EQ(bus->read(0x54), 0xfc); 
  EXPECT_EQ(cpu->getStatus(), 0b1000'0000);
}

TEST_F(CpuTest, DEX)
{
  // given
  unsigned char data[4] = {0xa2, 0x14, 0xca, 0x00}; // LDX #14; DEX;

  // when
  readData(data, 4);

  // then
  EXPECT_EQ(cpu->getStatus(), 0b0000'0000);
  EXPECT_EQ(cpu->getX(), 0x13);
}

TEST_F(CpuTest, DEY)
{
  // given
  unsigned char data[4] = {0xa0, 0x14, 0x88, 0x00}; // LDY #14; DEY;

  // when
  readData(data, 4);

  // then
  EXPECT_EQ(cpu->getStatus(), 0b0000'0000);
  EXPECT_EQ(cpu->getY(), 0x13);
}

TEST_F(CpuTest, INC)
{
  // given
  unsigned char data[7] = {0xa9, 0xfd, 0x85, 0x54, 0xe6, 0x54, 0x00}; // LDA #fd; STA $54; INC $54;

  // when
  readData(data, 7);

  // then
  EXPECT_EQ(bus->read(0x54), 0xfe); 
  EXPECT_EQ(cpu->getStatus(), 0b1000'0000);
}

TEST_F(CpuTest, JMP)
{
  // given
  unsigned char jumpInstructions[3] = {0xa9, 0x13, 0x00}; // LDA #13; BRK
  bus->write(0x1234, jumpInstructions, 3);
  unsigned char data[6] = {0x4c, 0x34, 0x12, 0xa9, 0x12, 0x00}; // JMP $1234; LDA #12; BRK;

  // when
  readData(data, 6);

  // then
  EXPECT_EQ(cpu->getA(), 0x13);
}

TEST_F(CpuTest, SLO)
{
  // given
  bus->write_8(0x12, 0b0110'0110);
  unsigned char data[5] = {0xa9, 0b0001'1010, 0x07, 0x12, 0x00}; // LDA #1a; ASL $12;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(bus->read(0x12), 0b1100'1100);
  EXPECT_EQ(cpu->getA(), 0b1101'1110);
  EXPECT_EQ(cpu->getStatus() & 0x01, 0x00); // carry = 0
}

TEST_F(CpuTest, RLA)
{
  // given
  bus->write_8(0x12, 0b1110'0110);
  unsigned char data[5] = {0xa9, 0b0101'1010, 0x27, 0x12, 0x00}; // LDA 0x5a; RLA $12;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(bus->read(0x12), 0b1100'1100);
  EXPECT_EQ(cpu->getA(), 0b0100'1000);
  EXPECT_EQ(cpu->getStatus() & 0x01, 1); // carry == 1
}

TEST_F(CpuTest, SRE)
{
  // given
  bus->write_8(0x12, 0b0110'0110);
  unsigned char data[6] = {0xa9, 0b1010'0011, 0x38, 0x47, 0x12, 0x00}; // LDA 0xa3; SEC; LSR $12;

  // when
  readData(data, 6);

  // then
  EXPECT_EQ(bus->read(0x12), 0b0011'0011);
  EXPECT_EQ(cpu->getA(), 0b1001'0000);
  EXPECT_EQ(cpu->getStatus() & 0x01, 0x00); // carry = 0
}

TEST_F(CpuTest, RRA)
{
  // given
  bus->write_8(0x12, 0b1110'0110);
  unsigned char data[5] = {0xa9, 0x02, 0x67, 0x12, 0x00}; // LDA 0x02; RRA $12;

  // when
  readData(data, 5);

  // then
  EXPECT_EQ(bus->read(0x12), 0b0111'0011);
  EXPECT_EQ(cpu->getA(), 0b0111'0101);
  EXPECT_EQ(cpu->getStatus() & 0x01, 0); // carry == 0
}

TEST_F(CpuTest, SAX)
{
  // given
  unsigned char data[7] = {0xa9, 0b1100'0100, 0xa2, 0b1010'1110, 0x87, 0x12, 0x00}; // LDA #c4; LDX $ae; SAX $12;

  // when
  readData(data, 7);

  // then
  EXPECT_EQ(bus->read(0x12), 0b1000'0100); 
}