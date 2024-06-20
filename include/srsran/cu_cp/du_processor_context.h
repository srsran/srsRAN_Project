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
#include <string>

namespace srsran {
namespace srs_cu_cp {

/// Basic cell system information provided by DU
struct du_sys_info {
  byte_buffer packed_mib;
  byte_buffer packed_sib1;
};

struct du_cell_context {
  du_cell_index_t      cell_index = du_cell_index_t::invalid; /// CU internal cell ID
  nr_cell_global_id_t  cgi;                                   /// global cell ID
  uint32_t             tac;                                   /// tracking area code
  pci_t                pci;                                   /// Physical cell ID
  std::vector<nr_band> bands;                                 /// NR bands provided/supported by the cell.
  du_sys_info          sys_info;                              /// System information provided by DU
};

} // namespace srs_cu_cp

} // namespace srsran
