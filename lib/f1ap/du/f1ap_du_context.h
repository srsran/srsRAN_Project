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
#include "srsran/du_manager/du_manager.h"
#include "srsran/f1ap/du/f1ap_du.h"

namespace srsran {
namespace srs_du {

struct f1ap_du_cell_context {
  nr_cell_global_id_t nr_cgi;
};

/// DU Context stored in the F1AP-DU. It includes information about the DU serving cells.
struct f1ap_du_context {
  uint64_t                          gnb_du_id;
  std::string                       gnb_du_name;
  std::vector<f1ap_du_cell_context> served_cells;
};

} // namespace srs_du
} // namespace srsran
