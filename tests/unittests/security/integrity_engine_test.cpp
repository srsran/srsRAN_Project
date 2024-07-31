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

#include "lib/security/integrity_engine_generic.h"
#include "lib/security/integrity_engine_nia2_cmac.h"
#include "lib/security/integrity_engine_nia2_non_cmac.h"
#include "nia1_test_set.h"
#include "nia2_test_set.h"
#include "nia3_test_set.h"
#include "srsran/security/integrity_engine.h"
#include "srsran/security/security.h"
#include "srsran/srslog/srslog.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srsran::security;

/// Fixture class for integrity engine tests
class fxt_nia_base : public testing::TestWithParam<nia_test_set>
{
protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);
    logger.set_hex_dump_max_size(3000);

    // init SEC logger
    srslog::fetch_basic_logger("SEC", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("SEC", false).set_hex_dump_max_size(-1);

    logger.info("Created fixture for integrity engine test");
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST", false);
};

/// Fixture class for integrity engine tests with NIA1
class fxt_nia1 : public fxt_nia_base
{};

/// Fixture class for integrity engine tests with NIA2
class fxt_nia2 : public fxt_nia_base
{};

/// Fixture class for integrity engine tests with NIA3
class fxt_nia3 : public fxt_nia_base
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

TEST_P(fxt_nia1, integrity_engine_generic_nia1)
{
  nia_test_set param = GetParam();

  // Pack hex strings into srsran types
  sec_128_key key      = make_sec_128_key(param.ik_cstr);
  auto        dir      = static_cast<security_direction>(param.direction);
  byte_buffer message  = make_byte_buffer(param.message_cstr).value();
  byte_buffer mact_buf = make_byte_buffer(param.mact_cstr).value();
  byte_buffer prot_buf = message.deep_copy().value();
  ASSERT_TRUE(prot_buf.append(mact_buf));

  // Create integrity engine
  std::unique_ptr<integrity_engine> nia =
      std::make_unique<integrity_engine_generic>(key, param.bearer, dir, integrity_algorithm::nia1);

  // Apply integrity and compare results
  for (unsigned i = 0; i < 2; i++) {
    security_result result = nia->protect_integrity(message.deep_copy().value(), param.count_i);
    ASSERT_TRUE(result.buf.has_value());
    logger.info(result.buf.value().begin(), result.buf.value().end(), "result:");
    logger.info(prot_buf.begin(), prot_buf.end(), "exp:");
    EXPECT_EQ(result.buf.value(), prot_buf);
    EXPECT_EQ(result.count, param.count_i);
  }
}

#ifdef MBEDTLS_CMAC_C
TEST_P(fxt_nia2, integrity_engine_nia2_cmac)
{
  nia_test_set param = GetParam();

  // Pack hex strings into srsran types
  sec_128_key key      = make_sec_128_key(param.ik_cstr);
  auto        dir      = static_cast<security_direction>(param.direction);
  byte_buffer message  = make_byte_buffer(param.message_cstr).value();
  byte_buffer mact_buf = make_byte_buffer(param.mact_cstr).value();
  byte_buffer prot_buf = message.deep_copy().value();
  ASSERT_TRUE(prot_buf.append(mact_buf));

  // Create integrity engine
  std::unique_ptr<integrity_engine> nia = std::make_unique<integrity_engine_nia2_cmac>(key, param.bearer, dir);

  // Apply integrity and compare results
  for (unsigned i = 0; i < 2; i++) {
    security_result result = nia->protect_integrity(message.deep_copy().value(), param.count_i);
    ASSERT_TRUE(result.buf.has_value());
    logger.info(result.buf.value().begin(), result.buf.value().end(), "result:");
    logger.info(prot_buf.begin(), prot_buf.end(), "exp:");
    EXPECT_EQ(result.buf.value(), prot_buf);
    EXPECT_EQ(result.count, param.count_i);
  }
}
#endif // MBEDTLS_CMAC_C

TEST_P(fxt_nia2, integrity_engine_nia2_non_cmac)
{
  nia_test_set param = GetParam();

  // Pack hex strings into srsran types
  sec_128_key key      = make_sec_128_key(param.ik_cstr);
  auto        dir      = static_cast<security_direction>(param.direction);
  byte_buffer message  = make_byte_buffer(param.message_cstr).value();
  byte_buffer mact_buf = make_byte_buffer(param.mact_cstr).value();
  byte_buffer prot_buf = message.deep_copy().value();
  ASSERT_TRUE(prot_buf.append(mact_buf));

  // Create integrity engine
  std::unique_ptr<integrity_engine> nia = std::make_unique<integrity_engine_nia2_non_cmac>(key, param.bearer, dir);

  // Apply integrity and compare results
  for (unsigned i = 0; i < 2; i++) {
    security_result result = nia->protect_integrity(message.deep_copy().value(), param.count_i);
    ASSERT_TRUE(result.buf.has_value());
    logger.info(result.buf.value().begin(), result.buf.value().end(), "result:");
    logger.info(prot_buf.begin(), prot_buf.end(), "exp:");
    EXPECT_EQ(result.buf.value(), prot_buf);
    EXPECT_EQ(result.count, param.count_i);
  }
}

TEST_P(fxt_nia2, integrity_engine_generic_nia2)
{
  nia_test_set param = GetParam();

  // Pack hex strings into srsran types
  sec_128_key key      = make_sec_128_key(param.ik_cstr);
  auto        dir      = static_cast<security_direction>(param.direction);
  byte_buffer message  = make_byte_buffer(param.message_cstr).value();
  byte_buffer mact_buf = make_byte_buffer(param.mact_cstr).value();
  byte_buffer prot_buf = message.deep_copy().value();
  ASSERT_TRUE(prot_buf.append(mact_buf));

  // Create integrity engine
  std::unique_ptr<integrity_engine> nia =
      std::make_unique<integrity_engine_generic>(key, param.bearer, dir, integrity_algorithm::nia2);

  // Apply integrity and compare results
  for (unsigned i = 0; i < 2; i++) {
    security_result result = nia->protect_integrity(message.deep_copy().value(), param.count_i);
    ASSERT_TRUE(result.buf.has_value());
    logger.info(result.buf.value().begin(), result.buf.value().end(), "result:");
    logger.info(prot_buf.begin(), prot_buf.end(), "exp:");
    EXPECT_EQ(result.buf.value(), prot_buf);
    EXPECT_EQ(result.count, param.count_i);
  }
}

TEST_P(fxt_nia3, integrity_engine_generic_nia3)
{
  nia_test_set param = GetParam();

  // Pack hex strings into srsran types
  sec_128_key key      = make_sec_128_key(param.ik_cstr);
  auto        dir      = static_cast<security_direction>(param.direction);
  byte_buffer message  = make_byte_buffer(param.message_cstr).value();
  byte_buffer mact_buf = make_byte_buffer(param.mact_cstr).value();
  byte_buffer prot_buf = message.deep_copy().value();
  ASSERT_TRUE(prot_buf.append(mact_buf));

  // Create integrity engine
  std::unique_ptr<integrity_engine> nia =
      std::make_unique<integrity_engine_generic>(key, param.bearer, dir, integrity_algorithm::nia3);

  // Apply integrity and compare results
  for (unsigned i = 0; i < 2; i++) {
    security_result result = nia->protect_integrity(message.deep_copy().value(), param.count_i);
    ASSERT_TRUE(result.buf.has_value());
    logger.info(result.buf.value().begin(), result.buf.value().end(), "result:");
    logger.info(prot_buf.begin(), prot_buf.end(), "exp:");
    EXPECT_EQ(result.buf.value(), prot_buf);
    EXPECT_EQ(result.count, param.count_i);
  }
}

//////////////////////////////////////////////////////////
// Finally, instantiate all testcases for each test set //
//////////////////////////////////////////////////////////
std::string test_param_info_to_string(const ::testing::TestParamInfo<nia_test_set>& info)
{
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "{}", info.param.name);
  return fmt::to_string(buffer);
}

INSTANTIATE_TEST_SUITE_P(nia1,
                         fxt_nia1,
                         ::testing::ValuesIn(nia1_test_set.begin(), nia1_test_set.end()),
                         test_param_info_to_string);

INSTANTIATE_TEST_SUITE_P(nia2,
                         fxt_nia2,
                         ::testing::ValuesIn(nia2_test_set.begin(), nia2_test_set.end()),
                         test_param_info_to_string);

INSTANTIATE_TEST_SUITE_P(nia3,
                         fxt_nia3,
                         ::testing::ValuesIn(nia3_test_set.begin(), nia3_test_set.end()),
                         test_param_info_to_string);

int main(int argc, char** argv)
{
  srslog::init();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
