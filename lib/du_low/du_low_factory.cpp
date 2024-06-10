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

#include "srsran/du_low/du_low_factory.h"
#include "du_low_impl.h"
#include "srsran/du_low/du_low_config.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

static std::unique_ptr<upper_phy> create_upper_phy(const upper_phy_config&                     upper_config,
                                                   const downlink_processor_factory_sw_config& dl_fact_config)
{
  // Create downlink processor factory.
  std::shared_ptr<downlink_processor_factory> dl_proc_factory = create_downlink_processor_factory_sw(dl_fact_config);
  report_fatal_error_if_not(dl_proc_factory, "Invalid DL processor factory.");

  // Create channel precoder factory.
  std::shared_ptr<channel_precoder_factory> precoding_factory = create_channel_precoder_factory("auto");
  report_fatal_error_if_not(precoding_factory, "Invalid channel precoder factory.");

  // Create resource grid factory.
  std::shared_ptr<resource_grid_factory> rg_factory = create_resource_grid_factory(precoding_factory);
  report_fatal_error_if_not(rg_factory, "Invalid resource grid factory.");

  // Create upper PHY factory.
  std::unique_ptr<upper_phy_factory> upper_phy_factory = create_upper_phy_factory(dl_proc_factory, rg_factory);
  report_fatal_error_if_not(upper_phy_factory, "Invalid upper PHY factory.");

  // Instantiate upper PHY.
  return upper_phy_factory->create(upper_config);
}

std::unique_ptr<du_low> srsran::make_du_low(const du_low_config& config)
{
  std::vector<std::unique_ptr<upper_phy>> upper;
  for (const auto& cell_cfg : config.cells) {
    upper.push_back(create_upper_phy(cell_cfg.upper_phy_cfg, cell_cfg.dl_proc_cfg));
  }

  return std::make_unique<du_low_impl>(std::move(upper));
}
