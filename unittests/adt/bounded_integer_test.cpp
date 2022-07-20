/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/adt/bounded_integer.h"
#include "srsgnb/support/test_utils.h"
#include <cstdint>

using namespace srsgnb;

void test_bounded_integer_default_ctor_is_invalid_number()
{
  bounded_integer<int, 4, 10> val;
  TESTASSERT(not val.is_valid());
}

void test_bounded_integer_valid_check_passes()
{
  bounded_integer<unsigned, 1, 3> val(3);
  TESTASSERT(val.is_valid());
}

void test_bounded_integer_cast_to_integer()
{
  bounded_integer<uint16_t, 2, 6> val = 3;

  uint16_t v = static_cast<uint16_t>(val);
  TESTASSERT_EQ(3, v);
}

void test_bounded_integer_formats_valid_numbers()
{
  bounded_integer<int, 4, 10> val = 6;
  std::string                 s   = fmt::format("{}", val);
  TESTASSERT_EQ("6", s);
}

void test_invalid_bounded_integer_format_is_not_number()
{
  bounded_integer<int, 4, 10> val;
  std::string                 s = fmt::format("{}", val);
  TESTASSERT_EQ("INVALID", s);
}

int main()
{
  test_bounded_integer_default_ctor_is_invalid_number();
  test_bounded_integer_valid_check_passes();
  test_bounded_integer_cast_to_integer();
  test_bounded_integer_formats_valid_numbers();
  test_invalid_bounded_integer_format_is_not_number();
}
