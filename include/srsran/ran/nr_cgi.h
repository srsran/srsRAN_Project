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
