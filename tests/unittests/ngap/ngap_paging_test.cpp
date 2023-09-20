/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "ngap_test_helpers.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class ngap_paging_test : public ngap_test
{
protected:
  bool was_minimal_conversion_successful() const
  {
    // check ue paging id
    if (cu_cp_paging_notifier.last_msg.ue_paging_id.amf_set_id != 1) {
      test_logger.error("AMF Set ID mismatch {} != {}", cu_cp_paging_notifier.last_msg.ue_paging_id.amf_set_id, 1);
      return false;
    }
    if (cu_cp_paging_notifier.last_msg.ue_paging_id.amf_pointer != 0) {
      test_logger.error("AMF Pointer mismatch {} != {}", cu_cp_paging_notifier.last_msg.ue_paging_id.amf_pointer, 0);
      return false;
    }
    if (cu_cp_paging_notifier.last_msg.ue_paging_id.five_g_tmsi != 4211117727) {
      test_logger.error(
          "FiveG TMSI mismatch {} != {}", cu_cp_paging_notifier.last_msg.ue_paging_id.five_g_tmsi, 4211117727);
      return false;
    }

    // check tai list for paging
    if (cu_cp_paging_notifier.last_msg.tai_list_for_paging.size() != 1) {
      return false;
    }

    auto& paging_item = cu_cp_paging_notifier.last_msg.tai_list_for_paging.front();
    if (paging_item.tai.plmn_id != "00f110") {
      test_logger.error("PLMN mismatch {} != 00f110", paging_item.tai.plmn_id);
      return false;
    }
    if (paging_item.tai.tac != 7) {
      test_logger.error("TAC mismatch {} != {}", paging_item.tai.tac, 7);
      return false;
    }

    return true;
  }

  bool was_full_conversion_successful() const
  {
    if (!was_minimal_conversion_successful()) {
      return false;
    }

    // check paging drx
    if (!cu_cp_paging_notifier.last_msg.paging_drx.has_value()) {
      return false;
    }
    if (cu_cp_paging_notifier.last_msg.paging_drx.value() != 64) {
      test_logger.error("Paging DRX mismatch {} != {}", cu_cp_paging_notifier.last_msg.paging_drx, 64);
      return false;
    }

    // check paging prio
    if (!cu_cp_paging_notifier.last_msg.paging_prio.has_value()) {
      return false;
    }
    if (cu_cp_paging_notifier.last_msg.paging_prio.value() != 5) {
      test_logger.error("Paging prio mismatch {} != {}", cu_cp_paging_notifier.last_msg.paging_prio.value(), 5);
      return false;
    }

    // check ue radio cap for paging
    if (!cu_cp_paging_notifier.last_msg.ue_radio_cap_for_paging.has_value()) {
      return false;
    }
    if (cu_cp_paging_notifier.last_msg.ue_radio_cap_for_paging.value().ue_radio_cap_for_paging_of_nr !=
        make_byte_buffer("deadbeef")) {
      test_logger.error("UE radio cap for paging mismatch {} != {}",
                        cu_cp_paging_notifier.last_msg.ue_radio_cap_for_paging.value().ue_radio_cap_for_paging_of_nr,
                        make_byte_buffer("deadbeef"));
      return false;
    }

    // check paging origin
    if (!cu_cp_paging_notifier.last_msg.paging_origin.has_value()) {
      return false;
    }
    if (!cu_cp_paging_notifier.last_msg.paging_origin.value()) {
      test_logger.error("Paging origin mismatch");
      return false;
    }

    // check assist data for paging
    if (!cu_cp_paging_notifier.last_msg.assist_data_for_paging.has_value()) {
      return false;
    }

    if (!cu_cp_paging_notifier.last_msg.assist_data_for_paging.value().assist_data_for_recommended_cells.has_value()) {
      return false;
    }
    if (cu_cp_paging_notifier.last_msg.assist_data_for_paging.value()
            .assist_data_for_recommended_cells.value()
            .recommended_cells_for_paging.recommended_cell_list.size() != 1) {
      return false;
    }
    auto& cell_item = cu_cp_paging_notifier.last_msg.assist_data_for_paging.value()
                          .assist_data_for_recommended_cells.value()
                          .recommended_cells_for_paging.recommended_cell_list.front();
    if (cell_item.ngran_cgi.plmn_hex != "00f110") {
      test_logger.error("NR CGI PLMN mismatch {} != 00f110", cell_item.ngran_cgi.plmn_hex);
      return false;
    }
    if (cell_item.ngran_cgi.nci != 6576) {
      test_logger.error("NR CGI NCI mismatch {} != {}", cell_item.ngran_cgi.nci, 6576);
      return false;
    }
    if (cell_item.time_stayed_in_cell.value() != 5) {
      test_logger.error("Time stayed in cell mismatch {} != {}", cell_item.time_stayed_in_cell, 5);
      return false;
    }

    if (!cu_cp_paging_notifier.last_msg.assist_data_for_paging.value().paging_attempt_info.has_value()) {
      return false;
    }
    if (cu_cp_paging_notifier.last_msg.assist_data_for_paging.value()
            .paging_attempt_info.value()
            .paging_attempt_count != 3) {
      test_logger.error("Paging attempt count mismatch {} != {}",
                        cu_cp_paging_notifier.last_msg.assist_data_for_paging.value()
                            .paging_attempt_info.value()
                            .paging_attempt_count,
                        3);
      return false;
    }
    if (cu_cp_paging_notifier.last_msg.assist_data_for_paging.value()
            .paging_attempt_info.value()
            .intended_nof_paging_attempts != 4) {
      test_logger.error("Intended nof paging attempts mismatch {} != {}",
                        cu_cp_paging_notifier.last_msg.assist_data_for_paging.value()
                            .paging_attempt_info.value()
                            .intended_nof_paging_attempts,
                        4);
      return false;
    }
    if (!cu_cp_paging_notifier.last_msg.assist_data_for_paging.value()
             .paging_attempt_info.value()
             .next_paging_area_scope.has_value()) {
      return false;
    }
    if (cu_cp_paging_notifier.last_msg.assist_data_for_paging.value()
            .paging_attempt_info.value()
            .next_paging_area_scope.value() != "changed") {
      test_logger.error("Next paging area mismatch {} != changed",
                        cu_cp_paging_notifier.last_msg.assist_data_for_paging.value()
                            .paging_attempt_info.value()
                            .next_paging_area_scope.value());
      return false;
    }

    return true;
  }
  bool was_minimal_paging_forwarded() const { return was_minimal_conversion_successful(); }

  bool was_full_paging_forwarded() const { return was_full_conversion_successful(); }

  bool was_error_indication_sent() const
  {
    return msg_notifier.last_ngap_msg.pdu.init_msg().value.type() ==
           asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::error_ind;
  }
};

/// Test handling of valid paging message with optional fields
TEST_F(ngap_paging_test, when_valid_paging_message_received_message_is_forwarded)
{
  // Inject paging message
  ngap_message paging_msg = generate_valid_minimal_paging_message();
  ngap->handle_message(paging_msg);

  // check that paging message has been forwarded
  ASSERT_TRUE(was_minimal_paging_forwarded());
}

/// Test handling of valid paging message with optional fields
TEST_F(ngap_paging_test, when_valid_paging_message_with_optional_values_received_message_is_forwarded)
{
  // Inject paging message
  ngap_message paging_msg = generate_valid_paging_message();
  ngap->handle_message(paging_msg);

  // check that paging message has been forwarded
  ASSERT_TRUE(was_full_paging_forwarded());
}

/// Test handling of invalid paging message
TEST_F(ngap_paging_test, when_invalid_paging_message_received_message_is_not_forwarded_and_error_indication_is_sent)
{
  // Inject paging message
  ngap_message paging_msg = generate_invalid_paging_message();
  ngap->handle_message(paging_msg);

  // Check that Error Indication has been sent to AMF
  ASSERT_TRUE(was_error_indication_sent());
}
