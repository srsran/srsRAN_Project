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

#include "srsran/du/du_low/du_low_factory.h"
#include "du_low_impl.h"
#include "srsran/du/du_low/du_low_config.h"
#include "srsran/support/error_handling.h"

using namespace srsran;
using namespace srs_du;

std::unique_ptr<du_low> srsran::srs_du::make_du_low(const du_low_config& config, du_low_dependencies&& deps)
{
  unsigned nof_cells = config.cells.size();

  // Create upper PHY factory.
  std::unique_ptr<upper_phy_factory> upper_phy_factory =
      create_upper_phy_factory(config.upper_phy_common_config, deps.upper_phy_common_deps);
  report_fatal_error_if_not(upper_phy_factory, "Invalid upper PHY factory.");

  std::vector<std::unique_ptr<upper_phy>> upper;
  for (unsigned i_cell = 0; i_cell != nof_cells; ++i_cell) {
    upper.push_back(upper_phy_factory->create(config.cells[i_cell].upper_phy_cfg, deps.cells[i_cell].upper_phy_deps));
  }

  return std::make_unique<du_low_impl>(std::move(upper));
}
