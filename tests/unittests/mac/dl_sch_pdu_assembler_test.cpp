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

#include "lib/mac/mac_dl/dl_sch_pdu_assembler.h"
#include "mac_test_helpers.h"
#include "srsran/mac/config/mac_config_helpers.h"
#include "srsran/ran/pdsch/pdsch_constants.h"
#include "srsran/support/bit_encoding.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

auto& test_logger = []() -> srslog::basic_logger& {
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::info);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();
  return srslog::fetch_basic_logger("TEST");
}();

TEST(mac_dl_sch_pdu, mac_ce_con_res_id_pack)
{
  // MAC PDU with DL-SCH subheader with UE Contention Resolution Identity MAC CE field (6 B payload + 1 B header)
  // |   |   |   |   |   |   |   |   |
  // | R |F=1|         LCID          |  Octet 1
  // |-------------------------------|
  // |  UE Contention Resolution Id  |  Octet 2
  // |             ...               |  ...
  // |  UE Contention Resolution Id  |  Octet 6

  std::vector<uint8_t> bytes(MAX_DL_PDU_LENGTH);
  dl_sch_pdu           pdu(bytes);
  ue_con_res_id_t      conres = {};
  for (unsigned i = 0; i != UE_CON_RES_ID_LEN; ++i) {
    conres[i] = test_rgen::uniform_int<uint8_t>();
  }
  pdu.add_ue_con_res_id(conres);
  span<const uint8_t> result = pdu.get();

  byte_buffer expected{0b00111110};
  ASSERT_TRUE(expected.append(conres));
  ASSERT_EQ(result, expected);
}

TEST(mac_dl_sch_pdu, mac_sdu_8bit_L_pack)
{
  // MAC PDU with DL-SCH subheader with 8-bit L field and MAC SDU (<256 B payload + 2 B header)
  // |   |   |   |   |   |   |   |   |
  // | R |F=0|         LCID          |  Octet 1
  // |               L               |  Octet 2
  // |-------------------------------|
  // |            Payload            |  Octet 3
  // |             ...               |  ...
  // |            Payload            |  Octet <256 + 2

  std::vector<uint8_t> bytes(MAX_DL_PDU_LENGTH);
  dl_sch_pdu           pdu(bytes);
  unsigned             payload_len = test_rgen::uniform_int<unsigned>(1, 255);
  byte_buffer          payload;
  for (unsigned i = 0; i != payload_len; ++i) {
    ASSERT_TRUE(payload.append(test_rgen::uniform_int<uint8_t>()));
  }
  lcid_t lcid = (lcid_t)test_rgen::uniform_int<unsigned>(0, MAX_NOF_RB_LCIDS);
  pdu.add_sdu(lcid, byte_buffer_chain{payload.copy()});
  span<const uint8_t> result = pdu.get();

  byte_buffer expected;
  bit_encoder enc(expected);
  enc.pack(0b00, 2);        // R | F
  enc.pack(lcid, 6);        // LCID
  enc.pack(payload_len, 8); // L
  enc.pack_bytes(payload);
  ASSERT_EQ(result, expected);
}

TEST(mac_dl_sch_pdu, mac_sdu_16bit_L_pack)
{
  // MAC PDU with DL-SCH subheader with 16-bit L field and MAC SDU (>=256 B payload + 3 B header)
  // |   |   |   |   |   |   |   |   |
  // | R |F=1|         LCID          |  Octet 1
  // |               L               |  Octet 2
  // |               L               |  Octet 3
  // |-------------------------------|
  // |            Payload            |  Octet 4
  // |             ...               |  ...
  // |            Payload            |  Octet >=256 + 3

  static constexpr unsigned HEADER_LEN = 3;
  std::vector<uint8_t>      bytes(MAX_DL_PDU_LENGTH);
  dl_sch_pdu                pdu(bytes);
  unsigned                  payload_len = test_rgen::uniform_int<unsigned>(256, bytes.size() - HEADER_LEN);
  byte_buffer               payload;
  for (unsigned i = 0; i != payload_len; ++i) {
    ASSERT_TRUE(payload.append(test_rgen::uniform_int<uint8_t>()));
  }
  lcid_t lcid = (lcid_t)test_rgen::uniform_int<unsigned>(0, MAX_NOF_RB_LCIDS);
  ASSERT_EQ(pdu.add_sdu(lcid, byte_buffer_chain{payload.copy()}), payload.length() + HEADER_LEN);
  span<const uint8_t> result = pdu.get();

  byte_buffer expected;
  bit_encoder enc(expected);
  enc.pack(0b01, 2);         // R | F
  enc.pack(lcid, 6);         // LCID
  enc.pack(payload_len, 16); // L
  enc.pack_bytes(payload);

  ASSERT_EQ(result, expected);
}

class dummy_dl_bearer : public mac_sdu_tx_builder
{
public:
  std::vector<byte_buffer> last_sdus;
  std::deque<unsigned>     next_rlc_pdu_sizes;

  size_t on_new_tx_sdu(span<uint8_t> mac_sdu_buf) override
  {
    unsigned nof_bytes = mac_sdu_buf.size();
    if (not next_rlc_pdu_sizes.empty()) {
      nof_bytes = std::min(nof_bytes, next_rlc_pdu_sizes.front());
      next_rlc_pdu_sizes.pop_front();
    }
    for (unsigned i = 0; i != nof_bytes; ++i) {
      mac_sdu_buf[i] = test_rgen::uniform_int<uint8_t>();
    }
    last_sdus.emplace_back(mac_sdu_buf.first(nof_bytes));
    return nof_bytes;
  }

  unsigned on_buffer_state_update() override { return 0; }
};

class mac_dl_sch_assembler_tester : public testing::Test
{
public:
  mac_dl_sch_assembler_tester() : ue_mng(rnti_table), dl_bearers(2), dl_sch_enc(ue_mng)
  {
    srslog::fetch_basic_logger("MAC", true).set_level(srslog::basic_levels::debug);
    srslog::init();

    for (unsigned i = 0; i != UE_CON_RES_ID_LEN; ++i) {
      report_fatal_error_if_not(msg3_pdu.append(test_rgen::uniform_int<uint8_t>()), "failed to allocate bytes");
    }

    // Create UE.
    req.bearers.resize(dl_bearers.size());
    for (unsigned i = 0; i != dl_bearers.size(); ++i) {
      req.bearers[i].lcid      = (lcid_t)i;
      req.bearers[i].dl_bearer = &dl_bearers[i];
    }
    req.ul_ccch_msg = &msg3_pdu;

    rnti_table.add_ue(req.crnti, req.ue_index);

    mac_dl_ue_context u{req};
    ue_mng.add_ue(std::move(u));
  }

  ~mac_dl_sch_assembler_tester() { srslog::flush(); }

protected:
  byte_buffer                  msg3_pdu;
  mac_ue_create_request        req = test_helpers::make_default_ue_creation_request();
  du_rnti_table                rnti_table;
  mac_dl_ue_manager            ue_mng;
  std::vector<dummy_dl_bearer> dl_bearers;
  dl_sch_pdu_assembler         dl_sch_enc;
};

TEST_F(mac_dl_sch_assembler_tester, msg4_correctly_assembled)
{
  const unsigned sdu_subheader_size = 2;
  const unsigned sdu_size           = test_rgen::uniform_int<unsigned>(5, 255);
  const unsigned conres_ce_size     = UE_CON_RES_ID_LEN + 1;
  const unsigned tb_size = sdu_subheader_size + sdu_size + conres_ce_size + test_rgen::uniform_int<unsigned>(0, 100);

  dl_msg_tb_info tb_info;
  tb_info.lc_chs_to_sched.push_back(dl_msg_lc_info{lcid_dl_sch_t::UE_CON_RES_ID, conres_ce_size});
  tb_info.lc_chs_to_sched.push_back(dl_msg_lc_info{LCID_SRB0, sdu_size});
  span<const uint8_t> result = this->dl_sch_enc.assemble_newtx_pdu(this->req.crnti, to_harq_id(0), 0, tb_info, tb_size);

  byte_buffer expected;
  bit_encoder enc(expected);
  // UE ConRes Id.
  enc.pack(0b00, 2);                         // R | F
  enc.pack(lcid_dl_sch_t::UE_CON_RES_ID, 6); // LCID
  enc.pack_bytes(this->msg3_pdu);            // UE ConRes Id.
  // MAC SDU.
  enc.pack(0b00, 2);                              // R | F
  enc.pack(LCID_SRB0, 6);                         // LCID
  enc.pack(sdu_size, 8);                          // L
  enc.pack_bytes(dl_bearers[0].last_sdus.back()); // SDU
  ASSERT_GE(tb_size, expected.length());
  if (expected.length() < tb_size) {
    // Padding.
    enc.pack(0b00, 2);                   // R | F
    enc.pack(lcid_dl_sch_t::PADDING, 6); // LCID
    std::vector<uint8_t> zeros(tb_size - expected.length(), 0);
    enc.pack_bytes(zeros);
  }

  ASSERT_EQ(tb_size, expected.length());
  ASSERT_EQ(tb_size, result.size()) << "PDU was not padded correctly";
  ASSERT_EQ(dl_bearers[0].last_sdus.size(), 1);
  ASSERT_EQ(dl_bearers[0].last_sdus.back().length(), sdu_size);
  ASSERT_EQ(result, expected);
}

TEST_F(mac_dl_sch_assembler_tester, pack_multiple_sdus_of_same_lcid)
{
  const unsigned nof_sdus                = test_rgen::uniform_int<unsigned>(2, 6);
  const unsigned MIN_LC_SCHED_BYTES_SIZE = 3; // Need to account MAC subheader.

  unsigned              tb_size = 0;
  std::vector<unsigned> sdu_payload_sizes(nof_sdus), sdu_req_sizes(nof_sdus);
  for (unsigned i = 0; i != nof_sdus; ++i) {
    // Generate SDU size.
    sdu_payload_sizes[i] = test_rgen::uniform_int<unsigned>(MIN_LC_SCHED_BYTES_SIZE, 1000);
    // Slighlty oversize the TB size.
    sdu_req_sizes[i] = MAX_MAC_SDU_SUBHEADER_SIZE + sdu_payload_sizes[i];
    if (i == nof_sdus - 1 and tb_size + sdu_req_sizes[i] == 258) {
      // avoid special case with impossible subheader + SDU size.
      sdu_payload_sizes[i]++;
      sdu_req_sizes[i] = MAX_MAC_SDU_SUBHEADER_SIZE + sdu_payload_sizes[i];
    }
    tb_size += sdu_req_sizes[i];

    // Add pending SDU in MAC Tx SDU dummy notifier.
    this->dl_bearers[1].next_rlc_pdu_sizes.push_back(sdu_payload_sizes[i]);
  }

  // MAC schedules one TB with one LCID, and size to fill with upper layer data.
  const unsigned lcid_sched_bytes = get_mac_sdu_payload_size(tb_size);
  dl_msg_tb_info tb_info;
  tb_info.lc_chs_to_sched.push_back(dl_msg_lc_info{LCID_SRB1, lcid_sched_bytes});

  span<const uint8_t> result = this->dl_sch_enc.assemble_newtx_pdu(this->req.crnti, to_harq_id(0), 0, tb_info, tb_size);
  ASSERT_EQ(result.size(), tb_size);
  ASSERT_EQ(this->dl_bearers[1].last_sdus.size(), nof_sdus);
  for (unsigned i = 0; i != nof_sdus; ++i) {
    ASSERT_EQ(this->dl_bearers[1].last_sdus[i].length(), sdu_payload_sizes[i])
        << fmt::format("SDU size mismatch for SDU {}/{}. First SDU len={}", i + 1, nof_sdus, sdu_payload_sizes[0]);
  }

  // Check if MAC PDU contains the MAC SDUs composed by a MAC subheader and the data passed by the upper layer.
  byte_buffer expected_result;
  bit_encoder enc(expected_result);
  for (unsigned i = 0; i != nof_sdus; ++i) {
    unsigned mac_expected_sdu_size = get_mac_sdu_payload_size(tb_size - expected_result.length());
    enc.pack(0b0, 1);                                                                            // R
    enc.pack(mac_expected_sdu_size >= MAC_SDU_SUBHEADER_LENGTH_THRES, 1);                        // F
    enc.pack(LCID_SRB1, 6);                                                                      // LCID
    enc.pack(sdu_payload_sizes[i], 8 * (get_mac_sdu_subheader_size(mac_expected_sdu_size) - 1)); // L
    enc.pack_bytes(dl_bearers[1].last_sdus[i]);                                                  // SDU payload.
  }
  if (expected_result.length() < tb_size) {
    // Padding.
    enc.pack(0b00, 2);                   // R | F
    enc.pack(lcid_dl_sch_t::PADDING, 6); // LCID
    std::vector<uint8_t> padding_bits(result.begin() + expected_result.length(), result.end());
    enc.pack_bytes(padding_bits);
  }
  ASSERT_EQ(expected_result, result);
}
