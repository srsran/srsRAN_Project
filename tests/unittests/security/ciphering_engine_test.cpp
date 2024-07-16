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

#include "lib/security/ciphering_engine_nea1.h"
#include "lib/security/ciphering_engine_nea2.h"
#include "lib/security/ciphering_engine_nea3.h"
#include "nea1_test_set.h"
#include "nea2_test_set.h"
#include "nea3_test_set.h"
#include "srsran/security/ciphering_engine.h"
#include "srsran/security/security.h"
#include "srsran/srslog/srslog.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srsran::security;

/// Fixture class for ciphering engine tests
class fxt_nea_base : public testing::TestWithParam<nea_test_set>
{
protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init SEC logger
    srslog::fetch_basic_logger("SEC", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("SEC", false).set_hex_dump_max_size(-1);

    logger.info("Created fixture for ciphering engine test");
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST", false);
};

/// Fixture class for ciphering engine tests with NEA1
class fxt_nea1 : public fxt_nea_base
{};

/// Fixture class for ciphering engine tests with NEA2
class fxt_nea2 : public fxt_nea_base
{};

/// Fixture class for ciphering engine tests with NEA3
class fxt_nea3 : public fxt_nea_base
{};

/// Converts a hex string (e.g. 01FA02) to a sec_as_key.
sec_key make_sec_key(const std::string& hex_str)
{
  byte_buffer key_buf = make_byte_buffer(hex_str).value();
  sec_key     key     = {};
  std::copy(key_buf.begin(), key_buf.end(), key.begin());
  return key;
}

/// Converts a hex string (e.g. 01FA02) to a sec_128_as_key.
sec_128_key make_sec_128_key(const std::string& hex_str)
{
  byte_buffer key_buf = make_byte_buffer(hex_str).value();
  sec_128_key key     = {};
  std::copy(key_buf.begin(), key_buf.end(), key.begin());
  return key;
}

/// Compares two byte arrays
int arrcmp(uint8_t const* const a, uint8_t const* const b, uint32_t len)
{
  uint32_t i = 0;

  for (i = 0; i < len; i++) {
    if (a[i] != b[i]) {
      return a[i] - b[i];
    }
  }
  return 0;
}

bool trim_tail_to_bitlength(byte_buffer_view buf, uint32_t bitlength)
{
  if ((bitlength + 7) / 8 != buf.length()) {
    return false;
  }
  uint32_t padding = bitlength % 8;
  if (padding > 0) {
    uint8_t mask = 0xff << (8 - padding);
    buf[buf.length() - 1] &= mask;
  }
  return true;
}

TEST_P(fxt_nea1, ciphering_engine_nea1)
{
  nea_test_set param = GetParam();

  // Pack hex strings into srsran types
  sec_128_key key        = make_sec_128_key(param.key_cstr);
  auto        dir        = static_cast<security_direction>(param.direction);
  byte_buffer plaintext  = make_byte_buffer(param.plaintext_cstr).value();
  byte_buffer ciphertext = make_byte_buffer(param.ciphertext_cstr).value();

  // Create ciphering engine
  std::unique_ptr<ciphering_engine> nea = std::make_unique<ciphering_engine_nea1>(key, param.bearer, dir);

  // Apply ciphering and compare results
  security_result result = nea->apply_ciphering(plaintext.deep_copy().value(), 0, param.count);
  ASSERT_TRUE(result.buf.has_value());
  ASSERT_TRUE(trim_tail_to_bitlength(result.buf.value(), param.length));
  EXPECT_EQ(result.buf.value(), ciphertext);
}

TEST_P(fxt_nea2, ciphering_engine_nea2)
{
  nea_test_set param = GetParam();

  // Pack hex strings into srsran types
  sec_128_key key        = make_sec_128_key(param.key_cstr);
  auto        dir        = static_cast<security_direction>(param.direction);
  byte_buffer plaintext  = make_byte_buffer(param.plaintext_cstr).value();
  byte_buffer ciphertext = make_byte_buffer(param.ciphertext_cstr).value();

  // Create ciphering engine
  std::unique_ptr<ciphering_engine> nea = std::make_unique<ciphering_engine_nea2>(key, param.bearer, dir);

  // Apply ciphering and compare results
  security_result result = nea->apply_ciphering(plaintext.deep_copy().value(), 0, param.count);
  ASSERT_TRUE(result.buf.has_value());
  ASSERT_TRUE(trim_tail_to_bitlength(result.buf.value(), param.length));
  EXPECT_EQ(result.buf.value(), ciphertext);
}

TEST_P(fxt_nea3, ciphering_engine_nea3)
{
  nea_test_set param = GetParam();

  // Pack hex strings into srsran types
  sec_128_key key        = make_sec_128_key(param.key_cstr);
  auto        dir        = static_cast<security_direction>(param.direction);
  byte_buffer plaintext  = make_byte_buffer(param.plaintext_cstr).value();
  byte_buffer ciphertext = make_byte_buffer(param.ciphertext_cstr).value();

  // Create ciphering engine
  std::unique_ptr<ciphering_engine> nea = std::make_unique<ciphering_engine_nea3>(key, param.bearer, dir);

  // Apply ciphering and compare results
  security_result result = nea->apply_ciphering(plaintext.deep_copy().value(), 0, param.count);
  ASSERT_TRUE(result.buf.has_value());
  ASSERT_TRUE(trim_tail_to_bitlength(result.buf.value(), param.length));
  EXPECT_EQ(result.buf.value(), ciphertext);
}

//////////////////////////////////////////////////////////
// Finally, instantiate all testcases for each test set //
//////////////////////////////////////////////////////////
std::string test_param_info_to_string(const ::testing::TestParamInfo<nea_test_set>& info)
{
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "{}", info.param.name);
  return fmt::to_string(buffer);
}

INSTANTIATE_TEST_SUITE_P(nea1,
                         fxt_nea1,
                         ::testing::ValuesIn(nea1_test_set.begin(), nea1_test_set.end()),
                         test_param_info_to_string);

INSTANTIATE_TEST_SUITE_P(nea2,
                         fxt_nea2,
                         ::testing::ValuesIn(nea2_test_set.begin(), nea2_test_set.end()),
                         test_param_info_to_string);

INSTANTIATE_TEST_SUITE_P(nea3,
                         fxt_nea3,
                         ::testing::ValuesIn(nea3_test_set.begin(), nea3_test_set.end()),
                         test_param_info_to_string);

int main(int argc, char** argv)
{
  srslog::init();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
