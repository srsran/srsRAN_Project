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

#include "srsran/adt/optional.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/cause.h"
#include "srsran/ran/crit_diagnostics.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/pci.h"
#include <cstdint>
#include <map>
#include <string>
#include <type_traits>
#include <vector>

namespace srsran {
namespace srs_cu_cp {

struct f1ap_f1_setup_request {
  uint64_t                                gnb_du_id;
  optional<std::string>                   gnb_du_name;
  std::vector<cu_cp_du_served_cells_item> gnb_du_served_cells_list;
  uint8_t                                 gnb_du_rrc_version;
  // TODO: Add optional fields
};

struct f1ap_cells_to_be_activ_list_item {
  nr_cell_global_id_t nr_cgi;
  optional<pci_t>     nr_pci;
};

struct f1ap_f1_setup_response {
  bool success = false;
  // F1 Setup Response
  optional<std::string>                         gnb_cu_name;
  std::vector<f1ap_cells_to_be_activ_list_item> cells_to_be_activ_list;
  uint8_t                                       gnb_cu_rrc_version;
  // TODO: Add optional fields

  // F1 Setup Failure
  optional<cause_t>            cause;
  optional<crit_diagnostics_t> crit_diagnostics;
  // TODO: Add optional fields
};

} // namespace srs_cu_cp
} // namespace srsran
