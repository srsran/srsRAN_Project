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

#pragma once

#include "ngap_types.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ngap/ngap_handover.h"
#include "srsran/ran/crit_diagnostics.h"
#include "srsran/security/security.h"

namespace srsran {
namespace srs_cu_cp {

// enum class ngap_handov_type { intra5gs = 0, fivegs_to_eps, eps_to_5gs, fivegs_to_utran };

// struct ngap_ue_source_handover_context {
//   std::map<pdu_session_id_t, std::vector<qos_flow_id_t>> pdu_sessions;
//   /// Storage for the RRCContainer required in SourceNGRANNode-ToTargetNGRANNode-TransparentContainer (see TS 38.413)
//   byte_buffer rrc_container;
// };

struct ngap_init_context_setup_request {
  ue_index_t                          ue_index = ue_index_t::invalid;
  optional<std::string>               old_amf;
  optional<ngap_ue_aggr_max_bit_rate> ue_aggr_max_bit_rate;
  // TODO: Add optional core_network_assist_info_for_inactive
  guami_t                                            guami;
  optional<cu_cp_pdu_session_resource_setup_request> pdu_session_res_setup_list_cxt_req;
  std::vector<s_nssai_t>                             allowed_nssai;
  security::security_context                         security_context;
  // TODO: Add optional trace_activation
  // TODO: Add optional mob_restrict_list
  optional<byte_buffer> ue_radio_cap;
  optional<uint16_t>    idx_to_rfsp;
  optional<uint64_t>    masked_imeisv;
  optional<byte_buffer> nas_pdu;
  // TODO: Add optional emergency_fallback_ind
  // TODO: Add optional rrc_inactive_transition_report_request
  optional<cu_cp_ue_radio_cap_for_paging> ue_radio_cap_for_paging;
  // TODO: Add optional redirection_voice_fallback
  // TODO: Add optional location_report_request_type
  // TODO: Add optional cn_assisted_ran_tuning
};

struct ngap_init_context_setup_failure {
  cause_t                                                                      cause;
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_failed_item> pdu_session_res_failed_to_setup_items;
  optional<crit_diagnostics_t>                                                 crit_diagnostics;
};

struct ngap_init_context_setup_response {
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_response_item> pdu_session_res_setup_response_items;
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_failed_item>   pdu_session_res_failed_to_setup_items;
  optional<crit_diagnostics_t>                                                   crit_diagnostics;
};

} // namespace srs_cu_cp
} // namespace srsran
