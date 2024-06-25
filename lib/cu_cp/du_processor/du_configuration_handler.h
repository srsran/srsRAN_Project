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

#include "srsran/cu_cp/du_processor_context.h"
#include "srsran/f1ap/cu_cp/du_setup_notifier.h"
#include "srsran/ran/gnb_du_id.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {
namespace srs_cu_cp {

/// Current configuration of the DU being managed by the CU-CP.
struct du_configuration_context {
  /// gNB-DU ID reported during F1 setup, as per TS 38.473.
  gnb_du_id_t id;
  /// gNB-DU name reported during F1 setup, as per TS 38.473.
  std::string name;
  uint8_t     rrc_version = 2;
  /// Served cells for this DU.
  std::vector<du_cell_configuration> served_cells;

  const du_cell_configuration* find_cell(pci_t pci) const
  {
    auto it = std::find_if(served_cells.begin(), served_cells.end(), [&pci](const auto& c) { return c.pci == pci; });
    return it != served_cells.end() ? &(*it) : nullptr;
  }
  const du_cell_configuration* find_cell(nr_cell_global_id_t cgi) const
  {
    auto it = std::find_if(served_cells.begin(), served_cells.end(), [&cgi](const auto& c) { return c.cgi == cgi; });
    return it != served_cells.end() ? &(*it) : nullptr;
  }
};

class du_configuration_handler
{
public:
  virtual ~du_configuration_handler() = default;

  /// \brief Whether the DU already shared its configuration with the CU-CP.
  bool has_context() const { return ctxt != nullptr; }

  /// Getter for the current DU configuration.
  const du_configuration_context& get_context() const
  {
    srsran_assert(ctxt != nullptr, "bad access to DU configuration context");
    return *ctxt;
  }

  /// Add a new DU configuration the CU-CP.
  virtual error_type<du_setup_result::rejected> handle_new_du_config(const du_setup_request& req) = 0;

  /// Update the configuration of an existing DU managed by the CU-CP.
  virtual error_type<du_setup_result::rejected> handle_du_config_update(const du_config_update_request& req) = 0;

protected:
  const du_configuration_context* ctxt = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran