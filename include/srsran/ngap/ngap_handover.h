/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

struct ngap_handover_preparation_request {
  ue_index_t   ue_index = ue_index_t::invalid;
  unsigned     gnb_id;
  nr_cell_id_t cell_id;
};

struct ngap_handover_preparation_response {
  // Place-holder for possible return values.
  bool success = false;
};

} // namespace srs_cu_cp

} // namespace srsran
