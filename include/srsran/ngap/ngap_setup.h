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

#include "ngap_types.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/crit_diagnostics.h"

namespace srsran {
namespace srs_cu_cp {

// enum class ngap_handov_type { intra5gs = 0, fivegs_to_eps, eps_to_5gs, fivegs_to_utran };

struct ngap_broadcast_plmn_item {
  std::string                       plmn_id;
  std::vector<slice_support_item_t> tai_slice_support_list;
};

struct ngap_supported_ta_item {
  uint32_t                              tac;
  std::vector<ngap_broadcast_plmn_item> broadcast_plmn_list;
};

struct ngap_ng_setup_request {
  unsigned                            max_setup_retries = 1;
  cu_cp_global_gnb_id                 global_ran_node_id;
  std::string                         ran_node_name;
  std::vector<ngap_supported_ta_item> supported_ta_list;
  uint16_t                            default_paging_drx;
  // TODO: Add optional ue_retention_info;
  // TODO: Add optional nb_iot_default_paging_drx
  // TODO: Add optional extended_ran_node_name
};

struct ngap_served_guami_item {
  guami_t                    guami;
  std::optional<std::string> backup_amf_name;
};

struct ngap_plmn_support_item {
  std::string                       plmn_id;
  std::vector<slice_support_item_t> slice_support_list;
};

struct ngap_ng_setup_response {
  std::string                         amf_name;
  std::vector<ngap_served_guami_item> served_guami_list;
  uint16_t                            relative_amf_capacity;
  std::vector<ngap_plmn_support_item> plmn_support_list;
  std::optional<crit_diagnostics_t>   crit_diagnostics;
  // TODO: Add optional ue_retention_info;
  // TODO: Add optional iab_supported;
  // TODO: Add optional extended_amf_name;
};

struct ngap_ng_setup_failure {
  ngap_cause_t                      cause;
  std::optional<crit_diagnostics_t> crit_diagnostics;
};

using ngap_ng_setup_result = variant<ngap_ng_setup_response, ngap_ng_setup_failure>;

} // namespace srs_cu_cp
} // namespace srsran
