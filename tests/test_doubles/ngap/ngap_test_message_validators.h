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

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/ran/cu_types.h"

namespace srsran {

namespace srs_cu_cp {

struct ngap_message;

} // namespace srs_cu_cp

namespace test_helpers {

bool is_valid_init_ue_message(const srs_cu_cp::ngap_message& msg);

bool is_valid_initial_context_setup_response(const srs_cu_cp::ngap_message& msg);

bool is_valid_initial_context_setup_failure(const srs_cu_cp::ngap_message& msg);

bool is_valid_ue_context_release_request(const srs_cu_cp::ngap_message& msg);

bool is_valid_ue_radio_capability_info_indication(const srs_cu_cp::ngap_message& msg);

bool is_valid_ue_context_release_complete(const srs_cu_cp::ngap_message& msg);

bool is_valid_pdu_session_resource_setup_response(const srs_cu_cp::ngap_message& msg);

bool is_valid_pdu_session_resource_release_response(const srs_cu_cp::ngap_message& msg);

bool is_valid_pdu_session_resource_modify_response(const srs_cu_cp::ngap_message& msg);

bool is_valid_error_indication(const srs_cu_cp::ngap_message& msg);

bool is_valid_handover_request_ack(const srs_cu_cp::ngap_message& msg);

bool is_valid_handover_notify(const srs_cu_cp::ngap_message& msg);

bool is_valid_handover_required(const srs_cu_cp::ngap_message& msg);

bool is_valid_handover_cancel(const srs_cu_cp::ngap_message& msg);

bool is_valid_ul_ran_status_transfer(const srs_cu_cp::ngap_message& msg);

bool is_valid_ng_reset(const srs_cu_cp::ngap_message& msg);

bool is_valid_ul_ue_associated_nrppa_transport(const srs_cu_cp::ngap_message& msg);

bool is_valid_ul_non_ue_associated_nrppa_transport(const srs_cu_cp::ngap_message& msg);

// Check if the NGAP PDU contains the expected PDU session setup response.
bool is_expected_pdu_session_resource_setup_response(
    const srs_cu_cp::ngap_message&       ngap_pdu,
    const std::vector<pdu_session_id_t>& expected_pdu_sessions_to_setup,
    const std::vector<pdu_session_id_t>& expected_pdu_sessions_failed_to_setup);

byte_buffer get_rrc_container(const srs_cu_cp::ngap_message& msg);

} // namespace test_helpers
} // namespace srsran
