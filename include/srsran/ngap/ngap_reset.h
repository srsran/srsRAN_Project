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
