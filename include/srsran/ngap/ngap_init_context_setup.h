/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ngap/ngap_types.h"
#include "srsran/ran/crit_diagnostics.h"
#include "srsran/security/security.h"

namespace srsran {
namespace srs_cu_cp {

struct ngap_init_context_setup_request {
  ue_index_t                               ue_index = ue_index_t::invalid;
  std::optional<std::string>               old_amf;
  std::optional<ngap_ue_aggr_max_bit_rate> ue_aggr_max_bit_rate;
  // TODO: Add optional core_network_assist_info_for_inactive
  guami_t                                                 guami;
  std::optional<cu_cp_pdu_session_resource_setup_request> pdu_session_res_setup_list_cxt_req;
  std::vector<s_nssai_t>                                  allowed_nssai;
  security::security_context                              security_context;
  // TODO: Add optional trace_activation
  // TODO: Add optional mob_restrict_list
  std::optional<byte_buffer> ue_radio_cap;
  std::optional<uint16_t>    idx_to_rfsp;
  std::optional<uint64_t>    masked_imeisv;
  std::optional<byte_buffer> nas_pdu;
  // TODO: Add optional emergency_fallback_ind
  // TODO: Add optional rrc_inactive_transition_report_request
  std::optional<cu_cp_ue_radio_cap_for_paging> ue_radio_cap_for_paging;
  // TODO: Add optional redirection_voice_fallback
  // TODO: Add optional location_report_request_type
  // TODO: Add optional cn_assisted_ran_tuning
};

struct ngap_init_context_setup_failure {
  ngap_cause_t                                                                 cause;
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_failed_item> pdu_session_res_failed_to_setup_items;
  std::optional<crit_diagnostics_t>                                            crit_diagnostics;
};

struct ngap_init_context_setup_response {
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_response_item> pdu_session_res_setup_response_items;
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_failed_item>   pdu_session_res_failed_to_setup_items;
  std::optional<crit_diagnostics_t>                                              crit_diagnostics;
};

} // namespace srs_cu_cp
} // namespace srsran
