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

#include "srsran/ran/nr_cgi.h"

namespace srsran {

namespace srs_cu_cp {

// Cell-related configuration used by the RRC.
struct rrc_cell_context {
  nr_cell_global_id_t cgi;
  uint32_t            tac;
};

} // namespace srs_cu_cp

} // namespace srsran
