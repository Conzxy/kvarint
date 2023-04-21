#include "kvarint.h"

#include <gtest/gtest.h>

#define DEF_TEST_FUNC1(bytes_)                                                 \
  kvarint_buf64_t buf64;                                                       \
  uint64_t in = 1;                                                             \
  in <<= (7 * ((bytes_)-1));                                                   \
  in += 123;                                                                   \
  kvarint_encode64(in, &buf64);                                                \
  ASSERT_EQ(buf64.len, (bytes_));                                              \
                                                                               \
  uint64_t out64;                                                              \
  ASSERT_EQ(KVARINT_OK, kvarint_decode64(buf64.buf, buf64.len, &out64));       \
                                                                               \
  ASSERT_EQ(in, out64)

TEST(kvarint, encode_and_decode1)
{
  kvarint_buf32_t buf32;
  kvarint_encode32(123, &buf32);
  ASSERT_EQ(buf32.len, 1);
  ASSERT_EQ(buf32.buf[0], 123);
  uint32_t out32;
  kvarint_decode32(buf32.buf, buf32.len, &out32);

  ASSERT_EQ(out32, 123);
}

TEST(kvarint, encode_and_decode2) { DEF_TEST_FUNC1(2); }
TEST(kvarint, encode_and_decode3) { DEF_TEST_FUNC1(3); }
TEST(kvarint, encode_and_decode4) { DEF_TEST_FUNC1(4); }
TEST(kvarint, encode_and_decode5) { DEF_TEST_FUNC1(5); }
TEST(kvarint, encode_and_decode6) { DEF_TEST_FUNC1(6); }
TEST(kvarint, encode_and_decode7) { DEF_TEST_FUNC1(7); }
TEST(kvarint, encode_and_decode8) { DEF_TEST_FUNC1(8); }
TEST(kvarint, encode_and_decode9) { DEF_TEST_FUNC1(9); }
TEST(kvarint, encode_and_decode10) { DEF_TEST_FUNC1(10); }
