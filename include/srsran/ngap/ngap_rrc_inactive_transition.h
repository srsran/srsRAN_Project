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

#include "srsran/cu_cp/cu_cp_types.h"

namespace srsran {
namespace srs_cu_cp {

struct ngap_rrc_inactive_transition_report {
  enum class ngap_rrc_state { inactive = 0, connected };

  ue_index_t                  ue_index = ue_index_t::invalid;
  ngap_rrc_state              rrc_state;
  cu_cp_user_location_info_nr user_location_info;
};

} // namespace srs_cu_cp
} // namespace srsran
