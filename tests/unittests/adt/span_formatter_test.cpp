/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
  fmt::format_to(buffer, "{:,}", span<int>(data));

  std::string formatted_string = to_string(buffer);
  std::string expected_string  = "0, 1, 2, 3, 4";
  TESTASSERT_EQ(formatted_string, expected_string);
  TESTASSERT_EQ(span<int>(data), span<int>(data));
}

static void test_u8_dec()
{
  std::array<uint8_t, 5> data = {0, 1, 2, 3, 4};
  fmt::memory_buffer     buffer;
  fmt::format_to(buffer, "{}", span<uint8_t>(data));

  std::string formatted_string = to_string(buffer);
  std::string expected_string  = "0 1 2 3 4";
  TESTASSERT_EQ(formatted_string, expected_string);
  TESTASSERT_EQ(span<uint8_t>(data), span<uint8_t>(data));
}

static void test_u8_hex()
{
  std::array<uint8_t, 5> data = {0, 1, 2, 3, 4};
  fmt::memory_buffer     buffer;
  fmt::format_to(buffer, "[{:0>2x}]", span<uint8_t>(data));

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
  fmt::format_to(buffer, "{}", span<cf_t>(data));

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
  fmt::format_to(buffer, "{:+.1f}", span<cf_t>(data));

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
