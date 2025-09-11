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

#include "test_helpers.h"
#include "../rrc/rrc_ue_test_helpers.h"
#include "tests/test_doubles/rrc/rrc_test_messages.h"
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/security/integrity.h"

using namespace srsran;
using namespace srs_cu_cp;

byte_buffer srsran::srs_cu_cp::generate_container_with_cell_group_config()
{
  asn1::unbounded_octstring<true> octet_str;
  octet_str.from_string(
      "5c00b001117aec701061e0007c20408d07810020a2090480ca8000f800000000008370842000088165000048200002069a06aa49880002"
      "00204000400d008013b64b1814400e468acf120000096070820f177e060870000000e25038000040bde802000400000000028201950300"
      "c400");
  return octet_str.to_byte_buffer();
}

byte_buffer srsran::srs_cu_cp::generate_rrc_setup_complete()
{
  asn1::unbounded_octstring<true> octet_str;
  octet_str.from_string(
      "000010c01000082727e01c3ff100c047e004139000bf202f8998000410000000f2e04f070f0707100517e004139000bf202f899800041000"
      "0000f1001032e04f070f0702f1b08010027db00000000080101b669000000000801000001000000005202f8990000011707f070c0401980b"
      "018010174000090530101000000000");
  return octet_str.to_byte_buffer();
}

byte_buffer srsran::srs_cu_cp::generate_rrc_reconfiguration_complete_pdu(unsigned transaction_id, uint8_t count)
{
  byte_buffer pdu_with_count = byte_buffer::create({0x00, count}).value();
  if (!pdu_with_count.append(pack_ul_dcch_msg(create_rrc_reconfiguration_complete(transaction_id)))) {
    return {};
  }

  security::sec_mac mac_exp   = {};
  auto              k_128_int = std::array<uint8_t, 16>{
      0xf3, 0xd5, 0x99, 0x4a, 0x3b, 0x29, 0x06, 0xfb, 0x27, 0x00, 0x4a, 0x44, 0x90, 0x6c, 0x6b, 0xd1};
  byte_buffer_view buf = pdu_with_count;

  security::security_nia2(
      mac_exp, k_128_int, count, srb_id_to_uint(srb_id_t::srb1) - 1, security::security_direction::uplink, buf);

  if (!pdu_with_count.append(mac_exp)) {
    return {};
  }

  return pdu_with_count;
}

rrc_timers_t srsran::srs_cu_cp::get_timers(const asn1::f1ap::f1_setup_request_s& f1_setup_req)
{
  rrc_timers_t timers;

  auto& sib1_container = f1_setup_req->gnb_du_served_cells_list[0]->gnb_du_served_cells_item().gnb_du_sys_info.sib1_msg;

  // Unpack SIB1 to store timers.
  asn1::rrc_nr::sib1_s sib1_msg;
  asn1::cbit_ref       bref2(sib1_container);
  if (sib1_msg.unpack(bref2) != asn1::SRSASN_SUCCESS) {
    report_fatal_error("Failed to unpack SIB1");
  }
  timers.t300 = std::chrono::milliseconds{sib1_msg.ue_timers_and_consts.t300.to_number()};
  timers.t301 = std::chrono::milliseconds{sib1_msg.ue_timers_and_consts.t301.to_number()};
  timers.t310 = std::chrono::milliseconds{sib1_msg.ue_timers_and_consts.t310.to_number()};
  timers.t311 = std::chrono::milliseconds{sib1_msg.ue_timers_and_consts.t311.to_number()};

  return timers;
}
