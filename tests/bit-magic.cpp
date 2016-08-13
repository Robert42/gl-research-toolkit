#include <gtest/gtest.h>

#include <glrt/toolkit/bit-magic.h>

using namespace glrt;

TEST(bit_magic, one_if_zero)
{
  EXPECT_EQ(quint32(1), -quint32(0xffffffff));
  EXPECT_EQ(quint32(0xffffffff), -quint32(1));
  EXPECT_EQ(quint32(0), -quint32(0));
  EXPECT_EQ(quint32(0), quint32(0) | -quint32(0));
  EXPECT_EQ(quint32(0), 0x80000000 & (quint32(0) | -quint32(0)));
  EXPECT_EQ(0x80000000, 0x80000000 & (quint32(42) | -quint32(42)));

  EXPECT_EQ(quint32(0), one_if_not_zero<quint32>(0));
  EXPECT_EQ(quint32(1), one_if_not_zero<quint32>(1));
  EXPECT_EQ(quint32(1), one_if_not_zero<quint32>(2));
  EXPECT_EQ(quint32(1), one_if_not_zero<quint32>(65));
  EXPECT_EQ(quint32(1), one_if_not_zero<quint32>(6846));
  EXPECT_EQ(quint32(1), one_if_not_zero<quint32>(0x80000000));
  EXPECT_EQ(quint32(1), one_if_not_zero<quint32>(0xffff0000));
}

TEST(bit_magic, bitIndexOf)
{
  EXPECT_EQ(quint8(0), bitIndexOf<quint8>(quint8(0)));
  EXPECT_EQ(quint16(0), bitIndexOf<quint16>(quint16(0)));
  EXPECT_EQ(quint32(0), bitIndexOf<quint32>(quint32(0)));
  EXPECT_EQ(quint64(0), bitIndexOf<quint64>(quint64(0)));

  for(quint8 i=0; i<8; ++i)
    EXPECT_EQ(quint8(i), bitIndexOf<quint8>(quint8(1)<<i));
  for(quint16 i=0; i<16; ++i)
    EXPECT_EQ(quint16(i), bitIndexOf<quint16>(quint16(1)<<i));
  for(quint32 i=0; i<32; ++i)
    EXPECT_EQ(quint32(i), bitIndexOf<quint32>(quint32(1)<<i));
  for(quint32 i=0; i<64; ++i)
    EXPECT_EQ(quint64(i), bitIndexOf<quint64>(quint64(1)<<i));
}

