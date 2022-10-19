/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/du_manager/converters/mac_cell_configuration_helpers.h"
#include "lib/mac/mac_dl/dl_sch_pdu_assembler.h"
#include "srsgnb/support/bit_encoding.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsgnb;

std::random_device rd;
std::mt19937       g(rd());

unsigned get_random_uint(unsigned min, unsigned max)
{
  return std::uniform_int_distribution<unsigned>{min, max}(g);
}

TEST(mac_dl_sch_pdu, mac_ce_con_res_id_pack)
{
  // MAC PDU with DL-SCH subheader with UE Contention Resolution Identity MAC CE field (6 B payload + 1 B header)
  // |   |   |   |   |   |   |   |   |
  // | R |F=1|         LCID          |  Octet 1
  // |-------------------------------|
  // |  UE Contention Resolution Id  |  Octet 2
  // |             ...               |  ...
  // |  UE Contention Resolution Id  |  Octet 6

  std::vector<uint8_t> bytes(dl_sch_pdu::MAX_PDU_LENGTH);
  dl_sch_pdu           pdu(bytes);
  ue_con_res_id_t      conres = {};
  for (unsigned i = 0; i != UE_CON_RES_ID_LEN; ++i) {
    conres[i] = get_random_uint(0, 255);
  }
  pdu.add_ue_con_res_id(conres);
  span<const uint8_t> result = pdu.get();

  byte_buffer expected{0b00111110};
  expected.append(conres);
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

  std::vector<uint8_t> bytes(dl_sch_pdu::MAX_PDU_LENGTH);
  dl_sch_pdu           pdu(bytes);
  unsigned             payload_len = get_random_uint(0, 255);
  byte_buffer          payload;
  for (unsigned i = 0; i != payload_len; ++i) {
    payload.append(get_random_uint(0, 255));
  }
  lcid_t lcid = (lcid_t)get_random_uint(0, MAX_NOF_RB_LCIDS);
  pdu.add_sdu(lcid, byte_buffer_slice_chain{payload.copy()});
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

  std::vector<uint8_t> bytes(dl_sch_pdu::MAX_PDU_LENGTH);
  dl_sch_pdu           pdu(bytes);
  unsigned             payload_len = get_random_uint(256, dl_sch_pdu::MAX_PDU_LENGTH);
  byte_buffer          payload;
  for (unsigned i = 0; i != payload_len; ++i) {
    payload.append(get_random_uint(0, 255));
  }
  lcid_t lcid = (lcid_t)get_random_uint(0, MAX_NOF_RB_LCIDS);
  pdu.add_sdu(lcid, byte_buffer_slice_chain{payload.copy()});
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
  byte_buffer last_sdu;

  byte_buffer_slice_chain on_new_tx_sdu(unsigned nof_bytes) override
  {
    last_sdu.clear();
    for (unsigned i = 0; i != nof_bytes; ++i) {
      last_sdu.append(get_random_uint(0, 255));
    }
    return byte_buffer_slice_chain{last_sdu.copy()};
  }

  unsigned on_buffer_state_update() override { return 0; }
};

class mac_dl_sch_assembler_tester : public testing::Test
{
public:
  mac_dl_sch_assembler_tester() :
    pdu_pool(dl_sch_pdu::MAX_PDU_LENGTH * MAX_DL_PDUS_PER_SLOT, 1), ue_mng(rnti_table), dl_sch_enc(ue_mng, pdu_pool)
  {
    srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);

    for (unsigned i = 0; i != UE_CON_RES_ID_LEN; ++i) {
      msg3_pdu.append(get_random_uint(0, 255));
    }

    // Create UE.
    req.bearers.resize(1);
    req.bearers[0].lcid      = LCID_SRB0;
    req.bearers[0].dl_bearer = &dl_bearer;
    req.ul_ccch_msg          = &msg3_pdu;
    rnti_table.add_ue(req.crnti, req.ue_index);
    ue_mng.add_ue(req);
  }

  ~mac_dl_sch_assembler_tester() { srslog::flush(); }

protected:
  byte_buffer                   msg3_pdu;
  mac_ue_create_request_message req = test_helpers::make_default_ue_creation_request();
  du_rnti_table                 rnti_table;
  ticking_ring_buffer_pool      pdu_pool;
  mac_dl_ue_manager             ue_mng;
  dummy_dl_bearer               dl_bearer;
  dl_sch_pdu_assembler          dl_sch_enc;
};

TEST_F(mac_dl_sch_assembler_tester, msg4_correctly_assembled)
{
  const unsigned sdu_subheader_size = 2;
  const unsigned sdu_size           = get_random_uint(5, 255);
  const unsigned conres_ce_size     = UE_CON_RES_ID_LEN + 1;
  const unsigned tb_size            = sdu_subheader_size + sdu_size + conres_ce_size + get_random_uint(0, 100);

  dl_msg_tb_info tb_info;
  tb_info.subpdus.push_back(dl_msg_lc_info{lcid_dl_sch_t::UE_CON_RES_ID, conres_ce_size});
  tb_info.subpdus.push_back(dl_msg_lc_info{LCID_SRB0, sdu_size});
  span<const uint8_t> result = this->dl_sch_enc.assemble_pdu(this->req.crnti, tb_info, tb_size);

  byte_buffer expected;
  bit_encoder enc(expected);
  // UE ConRes Id.
  enc.pack(0b00, 2);                         // R | F
  enc.pack(lcid_dl_sch_t::UE_CON_RES_ID, 6); // LCID
  enc.pack_bytes(this->msg3_pdu);            // UE ConRes Id.
  // MAC SDU.
  enc.pack(0b00, 2);                  // R | F
  enc.pack(LCID_SRB0, 6);             // LCID
  enc.pack(sdu_size, 8);              // L
  enc.pack_bytes(dl_bearer.last_sdu); // SDU
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
  ASSERT_EQ(dl_bearer.last_sdu.length(), sdu_size);
  ASSERT_EQ(result, expected);
}
