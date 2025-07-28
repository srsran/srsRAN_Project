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

#include "f1ap_cu_test_helpers.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class f1ap_paging_test : public f1ap_cu_test
{
protected:
  /// \brief Generate a dummy PAGING message.
  static cu_cp_paging_message generate_paging_message()
  {
    cu_cp_paging_message paging_msg;

    // add ue paging id
    bounded_bitset<48> five_g_s_tmsi(48);
    five_g_s_tmsi.from_uint64(((uint64_t)1U << 38U) + ((uint64_t)0U << 32U) + 4211117727);
    paging_msg.ue_paging_id = cu_cp_five_g_s_tmsi{five_g_s_tmsi};

    // add paging drx
    paging_msg.paging_drx = 64;

    // add tai list for paging
    cu_cp_tai_list_for_paging_item tai_item;
    tai_item.tai.plmn_id = plmn_identity::test_value();
    tai_item.tai.tac     = 7;
    paging_msg.tai_list_for_paging.push_back(tai_item);

    // add paging prio
    paging_msg.paging_prio = 5;

    // add ue radio cap for paging
    cu_cp_ue_radio_cap_for_paging ue_radio_cap_for_paging;
    ue_radio_cap_for_paging.ue_radio_cap_for_paging_of_nr = make_byte_buffer("deadbeef").value();
    paging_msg.ue_radio_cap_for_paging                    = ue_radio_cap_for_paging;

    // add paging origin
    paging_msg.paging_origin = true;

    // add assist data for paging
    cu_cp_assist_data_for_paging assist_data_for_paging;

    // add assist data for recommended cells
    cu_cp_assist_data_for_recommended_cells assist_data_for_recommended_cells;

    cu_cp_recommended_cell_item recommended_cell_item;

    // add ngran cgi
    recommended_cell_item.ngran_cgi.nci     = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value();
    recommended_cell_item.ngran_cgi.plmn_id = plmn_identity::test_value();

    // add time stayed in cell
    recommended_cell_item.time_stayed_in_cell = 5;

    assist_data_for_recommended_cells.recommended_cells_for_paging.recommended_cell_list.push_back(
        recommended_cell_item);

    assist_data_for_paging.assist_data_for_recommended_cells = assist_data_for_recommended_cells;

    // add paging attempt info
    cu_cp_paging_attempt_info paging_attempt_info;

    paging_attempt_info.paging_attempt_count         = 3;
    paging_attempt_info.intended_nof_paging_attempts = 4;
    paging_attempt_info.next_paging_area_scope       = "changed";

    assist_data_for_paging.paging_attempt_info = paging_attempt_info;

    paging_msg.assist_data_for_paging = assist_data_for_paging;

    return paging_msg;
  }

  bool was_conversion_successful() const
  {
    auto& paging_msg = f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.paging();

    // check ue id idx value
    if (paging_msg->ue_id_idx_value.idx_len10().to_number() != (279089024671 % 1024)) {
      test_logger.error("UE ID idx value mismatch {} != {}",
                        paging_msg->ue_id_idx_value.idx_len10().to_number(),
                        (279089024671 % 1024));
      return false;
    }

    // check paging id
    if (paging_msg->paging_id.cn_ue_paging_id().five_g_s_tmsi().to_number() != 279089024671) {
      test_logger.error("Paging ID mismatch {} != {}",
                        paging_msg->paging_id.cn_ue_paging_id().five_g_s_tmsi().to_number(),
                        279089024671);
      return false;
    }

    // check paging drx
    if (!paging_msg->paging_drx_present) {
      return false;
    }
    if (paging_msg->paging_drx.to_number() != 64) {
      test_logger.error("Paging DRX mismatch {} != {}", paging_msg->paging_drx.to_number(), 64);
      return false;
    }

    // check paging prio
    if (!paging_msg->paging_prio_present) {
      return false;
    }
    if (paging_msg->paging_prio.to_number() != 5) {
      test_logger.error("Paging prio mismatch {} != {}", paging_msg->paging_prio.to_number(), 5);
      return false;
    }

    // check paging cell list
    if (paging_msg->paging_cell_list.size() != 1) {
      return false;
    }
    auto&            paging_cell_item = paging_msg->paging_cell_list[0].value().paging_cell_item();
    nr_cell_identity nci              = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value();
    if (paging_cell_item.nr_cgi.nr_cell_id.to_number() != nci.value()) {
      test_logger.error("NR CGI NCI mismatch {} != {}}", paging_cell_item.nr_cgi.nr_cell_id.to_number(), nci);
      return false;
    }
    if (paging_cell_item.nr_cgi.plmn_id.to_string() != "00f110") {
      test_logger.error("NR CGI PLMN mismatch {} != 00f110", paging_cell_item.nr_cgi.plmn_id.to_string());
      return false;
    }

    // check paging origin
    if (!paging_msg->paging_origin_present) {
      return false;
    }
    if ((std::string)paging_msg->paging_origin.to_string() != "non-3gpp") {
      test_logger.error("Paging origin mismatch {} != non-3gpp", paging_msg->paging_origin.to_string());
      return false;
    }

    return true;
  }

  bool was_paging_forwarded() const { return was_conversion_successful(); }
};

/// Test paging message handling
TEST_F(f1ap_paging_test, when_paging_message_received_message_is_forwarded)
{
  // Inject paging message
  cu_cp_paging_message paging_msg = generate_paging_message();
  f1ap->handle_paging(paging_msg);

  // check that paging message has been forwarded
  ASSERT_TRUE(was_paging_forwarded());
}
