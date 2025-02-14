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

#include "srsran/adt/slotted_array.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/ran/gnb_du_id.h"

namespace srsran {
namespace srs_du {

struct f1ap_du_cell_context {
  nr_cell_global_id_t nr_cgi;
};

/// DU Context stored in the F1AP-DU. It includes information about the DU serving cells.
struct f1ap_du_context {
  gnb_du_id_t                       du_id = gnb_du_id_t::invalid;
  std::string                       gnb_du_name;
  std::vector<f1ap_du_cell_context> served_cells;
};

} // namespace srs_du
} // namespace srsran
