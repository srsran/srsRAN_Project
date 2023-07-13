/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_low_impl.h"

using namespace srsran;

du_low_impl::du_low_impl(const du_low_configuration& du_low_cfg) : logger(*du_low_cfg.logger)
{
  // Create downlink processor factory.
  std::shared_ptr<downlink_processor_factory> dl_proc_factory =
      create_downlink_processor_factory_sw(du_low_cfg.dl_proc_cfg);
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
  phy_up = upper_phy_factory->create(du_low_cfg.upper_phy[0]);
  report_error_if_not(phy_up != nullptr, "Unable to create upper PHY.");
  logger.debug("Upper PHY created successfully");

  logger.debug("DU-low created successfully");
}
