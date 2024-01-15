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
  unsigned                            max_setup_retries = 5;
  cu_cp_global_gnb_id                 global_ran_node_id;
  std::string                         ran_node_name;
  std::vector<ngap_supported_ta_item> supported_ta_list;
  uint16_t                            default_paging_drx;
  // TODO: Add optional ue_retention_info;
  // TODO: Add optional nb_iot_default_paging_drx
  // TODO: Add optional extended_ran_node_name
};

struct ngap_served_guami_item {
  guami_t               guami;
  optional<std::string> backup_amf_name;
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
  optional<crit_diagnostics_t>        crit_diagnostics;
  // TODO: Add optional ue_retention_info;
  // TODO: Add optional iab_supported;
  // TODO: Add optional extended_amf_name;
};

struct ngap_ng_setup_failure {
  cause_t                      cause;
  optional<crit_diagnostics_t> crit_diagnostics;
};

using ngap_ng_setup_result = variant<ngap_ng_setup_response, ngap_ng_setup_failure>;

} // namespace srs_cu_cp
} // namespace srsran
