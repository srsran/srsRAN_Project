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

#include "srsran/ran/positioning/common.h"
#include <cstdint>
#include <variant>

namespace srsran {

struct semipersistent_srs_t {
  uint8_t srs_res_set_id;
  // The IE exts is optional in the NRPPa but mandatory in the F1AP.
  std::optional<spatial_relation_info_t> srs_spatial_relation;
  std::optional<spatial_relation_info_t> srs_spatial_relation_per_srs_res;
};

struct srs_res_trigger_t {
  std::vector<uint8_t> aperiodic_srs_res_trigger_list;
};

struct aperiodic_srs_t {
  bool                             aperiodic;
  std::optional<srs_res_trigger_t> srs_res_trigger;
};

using srs_type_t = std::variant<semipersistent_srs_t, aperiodic_srs_t>;

} // namespace srsran
