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

#include "srsran/adt/slotted_array.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/band_helper.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/tac.h"
#include <string>

namespace srsran {
namespace srs_cu_cp {

/// Basic cell system information provided by DU
struct du_sys_info {
  byte_buffer packed_mib;
  byte_buffer packed_sib1;
};

struct du_cell_configuration {
  /// CU-CP specific DU cell identifier.
  du_cell_index_t cell_index = du_cell_index_t::invalid;
  /// Global cell ID.
  nr_cell_global_id_t cgi;
  /// Tracking Area Code
  tac_t tac;
  /// Physical cell ID
  pci_t pci;
  /// NR bands provided/supported by the cell.
  std::vector<nr_band> bands;
  /// System Information provided by the DU for this cell.
  du_sys_info sys_info;
};

} // namespace srs_cu_cp
} // namespace srsran
