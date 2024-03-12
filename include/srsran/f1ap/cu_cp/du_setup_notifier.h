/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/adt/optional.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/cause/f1ap_cause.h"
#include "srsran/ran/crit_diagnostics.h"
#include "srsran/ran/gnb_du_id.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/pci.h"
#include <cstdint>
#include <string>
#include <vector>

namespace srsran {
namespace srs_cu_cp {

struct du_setup_request {
  gnb_du_id_t                             gnb_du_id;
  std::string                             gnb_du_name;
  std::vector<cu_cp_du_served_cells_item> gnb_du_served_cells_list;
  uint8_t                                 gnb_du_rrc_version;
  // TODO: Add optional fields
};

struct f1ap_cells_to_be_activ_list_item {
  nr_cell_global_id_t nr_cgi;
  optional<pci_t>     nr_pci;
};

/// Result of a DU setup request operation.
struct du_setup_result {
  struct accepted {
    std::string                                   gnb_cu_name;
    std::vector<f1ap_cells_to_be_activ_list_item> cells_to_be_activ_list;
    uint8_t                                       gnb_cu_rrc_version;
  };
  struct rejected {
    f1ap_cause_t cause;
    std::string  cause_str;
  };

  variant<accepted, rejected> result;

  /// Whether the DU setup request was accepted by the CU-CP.
  bool is_accepted() const { return variant_holds_alternative<accepted>(result); }
};

/// \brief Interface used to handle F1AP interface management procedures as defined in TS 38.473 section 8.2.
class du_setup_notifier
{
public:
  virtual ~du_setup_notifier() = default;

  /// \brief Notifies about the reception of a F1 Setup Request message.
  /// \param[in] msg The received F1 Setup Request message.
  /// \return Error with time-to-wait in case the F1 Setup should not be accepted by the DU.
  virtual du_setup_result on_new_du_setup_request(const du_setup_request& msg) = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
