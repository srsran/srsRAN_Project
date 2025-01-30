/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
