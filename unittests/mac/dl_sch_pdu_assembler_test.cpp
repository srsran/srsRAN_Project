/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

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

  dl_sch_pdu      pdu;
  ue_con_res_id_t conres = {};
  for (unsigned i = 0; i != UE_CON_RES_ID_LEN; ++i) {
    conres[i] = get_random_uint(0, 255);
  }
  pdu.add_ue_con_res_id(conres);
  byte_buffer_slice_chain result = pdu.pop();

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

  dl_sch_pdu  pdu;
  unsigned    payload_len = get_random_uint(0, 255);
  byte_buffer payload;
  for (unsigned i = 0; i != payload_len; ++i) {
    payload.append(get_random_uint(0, 255));
  }
  lcid_t lcid = (lcid_t)get_random_uint(0, MAX_NOF_RB_LCIDS);
  pdu.add_sdu(lcid, byte_buffer_slice_chain{payload.copy()});
  byte_buffer_slice_chain result = pdu.pop();

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

  dl_sch_pdu  pdu;
  unsigned    payload_len = get_random_uint(256, 100000);
  byte_buffer payload;
  for (unsigned i = 0; i != payload_len; ++i) {
    payload.append(get_random_uint(0, 255));
  }
  lcid_t lcid = (lcid_t)get_random_uint(0, MAX_NOF_RB_LCIDS);
  pdu.add_sdu(lcid, byte_buffer_slice_chain{payload.copy()});
  byte_buffer_slice_chain result = pdu.pop();

  byte_buffer expected;
  bit_encoder enc(expected);
  enc.pack(0b01, 2);         // R | F
  enc.pack(lcid, 6);         // LCID
  enc.pack(payload_len, 16); // L
  enc.pack_bytes(payload);
  ASSERT_EQ(result, expected);
}
