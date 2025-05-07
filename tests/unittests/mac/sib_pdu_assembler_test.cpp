/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "lib/mac/mac_dl/sib_pdu_assembler.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

byte_buffer make_random_pdu(unsigned size = test_rgen::uniform_int<unsigned>(10, 200))
{
  return byte_buffer::create(test_rgen::random_vector<uint8_t>(size)).value();
}

std::vector<byte_buffer> make_random_segmented_pdu(unsigned segment_size = test_rgen::uniform_int<unsigned>(10, 200),
                                                   unsigned nof_segments = test_rgen::uniform_int<unsigned>(2, 3))
{
  std::vector<byte_buffer> segmented_pdu;
  for (unsigned i_segment = 0; i_segment != nof_segments; ++i_segment) {
    segmented_pdu.emplace_back(make_random_pdu(segment_size));
  }

  return segmented_pdu;
}

byte_buffer make_pdu_with_padding(const byte_buffer& payload, units::bytes tbs)
{
  byte_buffer          result = payload.deep_copy().value();
  std::vector<uint8_t> zeros(tbs.value() - result.length(), 0);
  report_fatal_error_if_not(result.append(zeros), "Failed appending zeros");
  return result;
}

static sib_information make_sib_pdu(std::optional<unsigned> si_msg_index, si_version_type si_version, units::bytes tbs)
{
  sib_information result{};
  result.si_indicator  = si_msg_index.has_value() ? sib_information::other_si : sib_information::sib1;
  result.si_msg_index  = si_msg_index;
  result.version       = si_version;
  result.is_repetition = false;
  result.pdsch_cfg.codewords.emplace_back();
  result.pdsch_cfg.codewords[0].tb_size_bytes = tbs.value();
  return result;
}

class sib_pdu_assembler_test : public ::testing::Test
{
public:
  sib_pdu_assembler_test() : sys_info_cfg({make_random_pdu()}), assembler(sys_info_cfg) {}

  byte_buffer update_si_pdus(const byte_buffer& sib1, span<const bcch_dl_sch_payload_type> si_msgs = {})
  {
    mac_cell_sys_info_config req;
    auto                     old_pdu = std::move(sys_info_cfg.sib1);
    sys_info_cfg.sib1                = sib1.copy();
    req.sib1                         = sib1.copy();
    req.si_messages.clear();
    for (const auto& si_msg : si_msgs) {
      req.si_messages.push_back(si_msg);
    }
    last_version             = next_version++;
    req.si_sched_cfg.version = last_version.value();
    assembler.handle_si_change_request(req);
    return old_pdu;
  }

  mac_cell_sys_info_config       sys_info_cfg;
  sib_pdu_assembler              assembler;
  si_version_type                next_version = 1;
  std::optional<si_version_type> last_version;

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
  auto old_msg = this->update_si_pdus(new_msg);
  ASSERT_EQ(last_version, 1);

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
  this->update_si_pdus(new_msg);
  ASSERT_EQ(last_version, 1);

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

TEST_F(sib_pdu_assembler_test, when_si_message_is_added_then_encoding_matched_added_si_message)
{
  auto new_msg = make_random_pdu();
  this->update_si_pdus(sys_info_cfg.sib1, std::vector<bcch_dl_sch_payload_type>{{new_msg.copy()}});
  ASSERT_EQ(last_version, 1);

  units::bytes        padding_len{test_rgen::uniform_int<unsigned>(0, 20)};
  units::bytes        tbs         = units::bytes{(unsigned)new_msg.length()} + padding_len;
  sib_information     new_si_info = make_sib_pdu(0, 1, tbs);
  span<const uint8_t> pdu         = assembler.encode_si_pdu(current_slot, new_si_info);

  auto expected = make_pdu_with_padding(new_msg, tbs);
  ASSERT_EQ(expected, pdu) << fmt::format("Incorrect SI-message payload returned.\n> expected=[{}]\n> result = [{}])",
                                          expected,
                                          byte_buffer::create(pdu).value());
}

TEST_F(sib_pdu_assembler_test, when_segmented_si_message_is_added_then_encoding_matched_added_si_message)
{
  // Generate a PDU with multiple segments.
  auto new_msg = make_random_segmented_pdu();

  this->update_si_pdus(sys_info_cfg.sib1, std::vector<bcch_dl_sch_payload_type>{{new_msg}});
  ASSERT_EQ(last_version, 1);

  units::bytes padding_len{test_rgen::uniform_int<unsigned>(0, 20)};
  units::bytes tbs = units::bytes{static_cast<unsigned>(new_msg[0].length())} + padding_len;

  sib_information new_si_info = make_sib_pdu(0, 1, tbs);
  for (unsigned i_segment = 0, nof_segments = new_msg.size(); i_segment != nof_segments; ++i_segment) {
    // Encode the PDU. If it has been transmitted before, and it is the first time it is transmitted within the
    // repetition period, the next SI message segment will be selected.
    new_si_info.is_repetition = false;
    span<const uint8_t> pdu   = assembler.encode_si_pdu(current_slot, new_si_info);

    // Encode the PDU again. Since it is a repetition, the returned segment should stay the same.
    new_si_info.is_repetition          = true;
    span<const uint8_t> pdu_repetition = assembler.encode_si_pdu(current_slot, new_si_info);

    auto expected = make_pdu_with_padding(new_msg[i_segment], tbs);
    ASSERT_EQ(expected, pdu) << fmt::format("Incorrect SI-message payload returned.\n> expected=[{}]\n> result = [{}])",
                                            expected,
                                            byte_buffer::create(pdu).value());
    ASSERT_EQ(pdu, pdu_repetition) << fmt::format(
        "SI-message payload does not match its repetition.\n> expected=[{}]\n> result = [{}])",
        byte_buffer::create(pdu).value(),
        byte_buffer::create(pdu_repetition).value());

    // Increment the number of transmissions of the current payload (required to encode segments other than the first
    // one).
    ++new_si_info.nof_txs;
  }
}
