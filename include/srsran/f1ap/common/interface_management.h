/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/ran/cause/f1ap_cause.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/pci.h"

namespace srsran {

/// Cell commanded by the CU to be activated via the F1AP interface.
struct f1ap_cell_to_activate {
  nr_cell_global_id_t        cgi;
  std::optional<pci_t>       pci;
  std::vector<plmn_identity> available_plmn_list;
};

/// Cell failed to be activated via the F1AP interface.
struct f1ap_cell_failed_to_activate {
  nr_cell_global_id_t cgi;
  f1ap_cause_t        cause;
};

/// Cell commanded by the CU to be deactivated via the F1AP interface.
struct f1ap_cell_to_deactivate {
  nr_cell_global_id_t cgi;
};

} // namespace srsran
