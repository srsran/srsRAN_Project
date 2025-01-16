/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/mac/mac_dl/sib_pdu_assembler.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

byte_buffer make_random_pdu(unsigned size = test_rgen::uniform_int<unsigned>(10, 200))
{
  return byte_buffer::create(test_rgen::random_vector<uint8_t>(size)).value();
}

byte_buffer make_pdu_with_padding(const byte_buffer& payload, units::bytes tbs)
{
  byte_buffer          result = payload.deep_copy().value();
  std::vector<uint8_t> zeros(tbs.value() - result.length(), 0);
  report_fatal_error_if_not(result.append(zeros), "Failed appending zeros");
  return result;
}

class sib_pdu_assembler_test : public ::testing::Test
{
public:
  sib_pdu_assembler_test() :
    bcch_dl_msgs({make_random_pdu()}), versions(bcch_dl_msgs.size(), 0), assembler(bcch_dl_msgs)
  {
  }

  byte_buffer update_sib1_pdu(byte_buffer pdu)
  {
    byte_buffer old_pdu = std::move(bcch_dl_msgs[0]);
    bcch_dl_msgs[0]     = std::move(pdu);
    versions[0]         = assembler.handle_new_sib1_payload(bcch_dl_msgs[0].copy());
    return old_pdu;
  }

  std::vector<byte_buffer> bcch_dl_msgs;
  std::vector<unsigned>    versions;
  sib_pdu_assembler        assembler;
};

TEST_F(sib_pdu_assembler_test, when_sib1_is_scheduled_then_the_correct_payload_is_generated)
{
  units::bytes        tbs{(unsigned)bcch_dl_msgs[0].length() + test_rgen::uniform_int<unsigned>(0, 20)};
  span<const uint8_t> pdu = assembler.encode_sib1_pdu(versions[0], tbs);

  byte_buffer expected = make_pdu_with_padding(bcch_dl_msgs[0], tbs);
  ASSERT_EQ(expected, pdu) << fmt::format("Incorrect SIB1 payload returned.\n> expected=[{}]\n> result = [{}])",
                                          expected,
                                          byte_buffer::create(pdu).value());
}

TEST_F(sib_pdu_assembler_test, when_sib1_is_scheduled_with_invalid_version_then_a_pdu_of_zeros_is_generated)
{
  units::bytes        tbs{(unsigned)bcch_dl_msgs[0].length() + test_rgen::uniform_int<unsigned>(0, 20)};
  span<const uint8_t> pdu = assembler.encode_sib1_pdu(versions[0] + 1, tbs);

  ASSERT_EQ(pdu.size(), tbs.value());
  ASSERT_TRUE(std::all_of(pdu.begin(), pdu.end(), [](uint8_t c) { return c == 0; }));
}

TEST_F(sib_pdu_assembler_test, when_sib1_is_updated_then_encoding_accounts_for_new_version)
{
  this->update_sib1_pdu(make_random_pdu());
  ASSERT_EQ(versions[0], 1);

  units::bytes        tbs{(unsigned)bcch_dl_msgs[0].length() + test_rgen::uniform_int<unsigned>(0, 20)};
  span<const uint8_t> pdu = assembler.encode_sib1_pdu(versions[0], tbs);

  byte_buffer expected = make_pdu_with_padding(bcch_dl_msgs[0], tbs);

  ASSERT_EQ(expected, pdu) << fmt::format("Incorrect SIB1 payload returned.\n> expected=[{}]\n> result = [{}])",
                                          expected,
                                          byte_buffer::create(pdu).value());
}

TEST_F(sib_pdu_assembler_test, when_sib1_is_updated_and_old_version_is_scheduled_then_encoding_returns_old_version)
{
  auto old_pdu = this->update_sib1_pdu(make_random_pdu());
  ASSERT_EQ(versions[0], 1);

  units::bytes        tbs{(unsigned)old_pdu.length() + test_rgen::uniform_int<unsigned>(0, 20)};
  span<const uint8_t> pdu = assembler.encode_sib1_pdu(versions[0] - 1, tbs);

  byte_buffer expected = make_pdu_with_padding(old_pdu, tbs);

  ASSERT_EQ(expected, pdu) << fmt::format("Incorrect SIB1 payload returned.\n> expected=[{}]\n> result = [{}])",
                                          expected,
                                          byte_buffer::create(pdu).value());
}
