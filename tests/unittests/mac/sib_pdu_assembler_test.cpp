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

static sib_information
make_sib_pdu(std::optional<si_message_index_type> si_msg_index, si_version_type si_version, units::bytes tbs)
{
  sib_information result{};
  result.si_indicator = si_msg_index.has_value() ? sib_information::other_si : sib_information::sib1;
  result.si_msg_index = si_msg_index;
  result.version      = si_version;
  result.pdsch_cfg.codewords.emplace_back();
  result.pdsch_cfg.codewords[0].tb_size_bytes = tbs.value();
  return result;
}

class sib_pdu_assembler_test : public ::testing::Test
{
public:
  sib_pdu_assembler_test() : sys_info_cfg({make_random_pdu()}), assembler(sys_info_cfg) {}

  byte_buffer update_sib1_pdu(const byte_buffer& pdu)
  {
    mac_cell_sys_info_config req;
    auto                     old_pdu = std::move(sys_info_cfg.sib1);
    sys_info_cfg.sib1                = pdu.copy();
    req.sib1                         = pdu.copy();
    last_resp                        = assembler.handle_si_change_request(req);
    return old_pdu;
  }

  mac_cell_sys_info_config        sys_info_cfg;
  sib_pdu_assembler               assembler;
  std::optional<si_change_result> last_resp;

  slot_point current_slot{1, 0};
};

TEST_F(sib_pdu_assembler_test, when_sib1_is_scheduled_then_the_correct_payload_is_generated)
{
  units::bytes        padding_len{test_rgen::uniform_int<unsigned>(0, 20)};
  units::bytes        tbs     = units::bytes{(unsigned)sys_info_cfg.sib1.length()} + padding_len;
  sib_information     si_info = make_sib_pdu(std::nullopt, 0, tbs);
  span<const uint8_t> pdu     = assembler.encode_si_pdu(current_slot, si_info);

  byte_buffer expected = make_pdu_with_padding(sys_info_cfg.sib1, tbs);
  ASSERT_EQ(expected, pdu) << fmt::format("Incorrect SIB1 payload returned.\n> expected=[{}]\n> result = [{}])",
                                          expected,
                                          byte_buffer::create(pdu).value());
}

TEST_F(sib_pdu_assembler_test, when_invalid_si_msg_index_is_scheduled_then_a_pdu_of_zeros_is_generated)
{
  units::bytes        padding_len{test_rgen::uniform_int<unsigned>(0, 20)};
  units::bytes        tbs     = units::bytes{(unsigned)sys_info_cfg.sib1.length()} + padding_len;
  sib_information     si_info = make_sib_pdu(2, 0, tbs);
  span<const uint8_t> pdu     = assembler.encode_si_pdu(current_slot, si_info);

  ASSERT_EQ(pdu.size(), tbs.value());
  ASSERT_TRUE(std::all_of(pdu.begin(), pdu.end(), [](uint8_t c) { return c == 0; }));
}

TEST_F(sib_pdu_assembler_test, when_sib1_is_updated_and_old_version_is_scheduled_then_encoding_returns_old_version)
{
  auto new_msg = make_random_pdu();
  auto old_msg = this->update_sib1_pdu(new_msg);
  ASSERT_EQ(last_resp.value().version, 1);

  const unsigned nof_tries = 4;
  for (unsigned i = 0; i != nof_tries; ++i) {
    current_slot++;

    // Old SIB1 version is scheduled, old SIB1 PDU is encoded.
    units::bytes        padding_len{test_rgen::uniform_int<unsigned>(0, 20)};
    units::bytes        tbs         = units::bytes{(unsigned)old_msg.length()} + padding_len;
    sib_information     old_si_info = make_sib_pdu(std::nullopt, 0, tbs);
    span<const uint8_t> pdu         = assembler.encode_si_pdu(current_slot, old_si_info);
    byte_buffer         expected    = make_pdu_with_padding(old_msg, tbs);
    ASSERT_EQ(expected, pdu) << fmt::format("Incorrect SIB1 payload returned.\n> expected=[{}]\n> result = [{}])",
                                            expected,
                                            byte_buffer::create(pdu).value());
  }
}

TEST_F(sib_pdu_assembler_test, when_sib1_is_updated_then_encoding_accounts_for_new_version)
{
  auto new_msg = make_random_pdu();
  this->update_sib1_pdu(new_msg);
  ASSERT_EQ(last_resp.value().version, 1);

  const unsigned nof_tries = 4;
  for (unsigned i = 0; i != nof_tries; ++i) {
    current_slot++;

    // Encoding new PDU.
    units::bytes        padding_len{test_rgen::uniform_int<unsigned>(0, 20)};
    units::bytes        tbs         = units::bytes{(unsigned)new_msg.length()} + padding_len;
    sib_information     new_si_info = make_sib_pdu(std::nullopt, 1, tbs);
    span<const uint8_t> pdu         = assembler.encode_si_pdu(current_slot, new_si_info);

    auto expected = make_pdu_with_padding(new_msg, tbs);

    ASSERT_EQ(expected, pdu) << fmt::format("Incorrect SIB1 payload returned.\n> expected=[{}]\n> result = [{}])",
                                            expected,
                                            byte_buffer::create(pdu).value());
  }
}
