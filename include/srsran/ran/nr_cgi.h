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

#include "srsran/ran/nr_cell_identity.h"
#include "srsran/ran/plmn_identity.h"

namespace srsran {

/// \brief The NR Cell Global Identity (NR-CGI)
struct nr_cell_global_id_t {
  nr_cell_global_id_t() = default;
  nr_cell_global_id_t(plmn_identity plmn_id_, nr_cell_identity nci_) : plmn_id(plmn_id_), nci(nci_) {}

  /// PLMN identity.
  plmn_identity plmn_id = plmn_identity::test_value();
  /// NR cell id.
  nr_cell_identity nci;

  bool operator==(const nr_cell_global_id_t& rhs) const { return plmn_id == rhs.plmn_id and nci == rhs.nci; }
  bool operator!=(const nr_cell_global_id_t& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
