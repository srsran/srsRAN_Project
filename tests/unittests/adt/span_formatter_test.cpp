/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/support/srsran_test.h"
#include <array>

using namespace srsran;

static void test_int_comma()
{
  std::array<int, 5> data = {0, 1, 2, 3, 4};
  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer), "{:,}", span<int>(data));

  std::string formatted_string = to_string(buffer);
  std::string expected_string  = "0, 1, 2, 3, 4";
  TESTASSERT_EQ(formatted_string, expected_string);
  TESTASSERT_EQ(span<int>(data), span<int>(data));
}

static void test_u8_dec()
{
  std::array<uint8_t, 5> data = {0, 1, 2, 3, 4};
  fmt::memory_buffer     buffer;
  fmt::format_to(std::back_inserter(buffer), "{}", span<uint8_t>(data));

  std::string formatted_string = to_string(buffer);
  std::string expected_string  = "0 1 2 3 4";
  TESTASSERT_EQ(formatted_string, expected_string);
  TESTASSERT_EQ(span<uint8_t>(data), span<uint8_t>(data));
}

static void test_u8_hex()
{
  std::array<uint8_t, 5> data = {0, 1, 2, 3, 4};
  fmt::memory_buffer     buffer;
  fmt::format_to(std::back_inserter(buffer), "[{:0>2x}]", span<uint8_t>(data));

  std::string formatted_string = to_string(buffer);
  std::string expected_string  = "[00 01 02 03 04]";
  TESTASSERT_EQ(formatted_string, expected_string);
  TESTASSERT_EQ(span<uint8_t>(data), span<uint8_t>(data));
}

static void test_cf_long()
{
  using namespace std::complex_literals;

  std::array<cf_t, 5> data = {0.0if, 1.0if, 2.0if, 3.0if, 4.0if};
  fmt::memory_buffer  buffer;
  fmt::format_to(std::back_inserter(buffer), "{}", span<cf_t>(data));

  std::string formatted_string = to_string(buffer);
  std::string expected_string =
      "+0.000000+0.000000j +0.000000+1.000000j +0.000000+2.000000j +0.000000+3.000000j +0.000000+4.000000j";
  TESTASSERT_EQ(formatted_string, expected_string);
  TESTASSERT_EQ(span<cf_t>(data), span<cf_t>(data));
}

static void test_cf_short()
{
  using namespace std::complex_literals;

  std::array<cf_t, 5> data = {0.0if, 1.0if, 2.0if, 3.0if, 4.0if};
  fmt::memory_buffer  buffer;
  fmt::format_to(std::back_inserter(buffer), "{:+.1f}", span<cf_t>(data));

  std::string formatted_string = to_string(buffer);
  std::string expected_string  = "+0.0+0.0j +0.0+1.0j +0.0+2.0j +0.0+3.0j +0.0+4.0j";
  TESTASSERT_EQ(formatted_string, expected_string);
  TESTASSERT_EQ(span<cf_t>(data), span<cf_t>(data));
}

int main()
{
  test_int_comma();
  test_u8_dec();
  test_u8_hex();
  test_cf_long();
  test_cf_short();
  return 0;
}
