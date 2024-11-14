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

#include "srsran/adt/byte_buffer.h"
#include "srsran/ran/band_helper.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/pci.h"
#include <cstdint>

namespace srsran {

namespace srs_cu_cp {

// Cell-related configuration used by the RRC.
struct rrc_cell_context {
  nr_cell_global_id_t  cgi;
  uint32_t             tac;
  pci_t                pci;
  unsigned             ssb_arfcn; ///< Absolute SSB position.
  std::vector<nr_band> bands;     ///< Required for capability band filter.
};

} // namespace srs_cu_cp

} // namespace srsran
