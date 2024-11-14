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
#include "srsran/ran/cause/ngap_cause.h"
#include <optional>
#include <variant>

namespace srsran {
namespace srs_cu_cp {

struct cu_cp_ng_reset {
  ngap_cause_t            cause;
  bool                    ng_interface_reset = false;
  std::vector<ue_index_t> ues_to_reset;
};

using ngap_reset_ng_interface = bool;

struct ngap_ue_associated_lc_ng_conn_item {
  std::optional<amf_ue_id_t> amf_ue_id;
  std::optional<ran_ue_id_t> ran_ue_id;
};

using ngap_reset_type_part_of_interface = std::vector<ngap_ue_associated_lc_ng_conn_item>;

using ngap_reset_type_t = std::variant<ngap_reset_ng_interface, ngap_reset_type_part_of_interface>;

struct ngap_ng_reset {
  ngap_cause_t      cause;
  ngap_reset_type_t reset_type;
};

} // namespace srs_cu_cp
} // namespace srsran
