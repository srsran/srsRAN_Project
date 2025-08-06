/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
