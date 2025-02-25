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
  std::vector<std::unique_ptr<upper_phy>> upper;
  for (const auto& cell_cfg : config.cells) {
    // Create upper PHY factory.
    std::unique_ptr<upper_phy_factory> upper_phy_factory = create_upper_phy_factory(cell_cfg.dl_proc_cfg);
    report_fatal_error_if_not(upper_phy_factory, "Invalid upper PHY factory.");

    upper.push_back(upper_phy_factory->create(cell_cfg.upper_phy_cfg));
  }

  return std::make_unique<du_low_impl>(std::move(upper));
}
