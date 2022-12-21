/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/slotted_array.h"
#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/f1ap/du/f1ap_du.h"

namespace srsgnb {
namespace srs_du {

/// DU Context stored in the F1AP-DU. It includes information about the DU serving cells.
struct f1ap_du_context {
  uint64_t                                            gnb_du_id;
  std::string                                         gnb_du_name;
  std::vector<asn1::f1ap::gnb_du_served_cells_item_s> served_cells;
};

} // namespace srs_du
} // namespace srsgnb
