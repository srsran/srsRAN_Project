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

#include "srsran/du/du_low_config.h"
#include "srsran/phy/upper/upper_phy.h"

namespace srsran {

class du_low_impl final : public upper_phy
{
public:
  du_low_impl(const du_low_configuration& du_low_cfg);

  upper_phy_rx_symbol_handler& get_rx_symbol_handler() override { return phy_up->get_rx_symbol_handler(); }
  upper_phy_timing_handler&    get_timing_handler() override { return phy_up->get_timing_handler(); }
  downlink_processor_pool&     get_downlink_processor_pool() override { return phy_up->get_downlink_processor_pool(); }
  resource_grid_pool& get_downlink_resource_grid_pool() override { return phy_up->get_downlink_resource_grid_pool(); }
  tx_buffer_pool&     get_tx_buffer_pool() override { return phy_up->get_tx_buffer_pool(); }
  resource_grid_pool& get_uplink_resource_grid_pool() override { return phy_up->get_uplink_resource_grid_pool(); }
  uplink_request_processor&   get_uplink_request_processor() override { return phy_up->get_uplink_request_processor(); }
  uplink_slot_pdu_repository& get_uplink_slot_pdu_repository() override
  {
    return phy_up->get_uplink_slot_pdu_repository();
  }
  const downlink_pdu_validator& get_downlink_pdu_validator() const override
  {
    return phy_up->get_downlink_pdu_validator();
  }
  const uplink_pdu_validator& get_uplink_pdu_validator() const override { return phy_up->get_uplink_pdu_validator(); }
  void                        set_timing_notifier(upper_phy_timing_notifier& notifier) override
  {
    return phy_up->set_timing_notifier(notifier);
  }
  void set_rx_results_notifier(upper_phy_rx_results_notifier& notifier) override
  {
    return phy_up->set_rx_results_notifier(notifier);
  }

private:
  srslog::basic_logger& logger;

  std::unique_ptr<upper_phy> phy_up;
};

} // namespace srsran
