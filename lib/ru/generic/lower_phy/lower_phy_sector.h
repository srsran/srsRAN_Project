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

#include "low_phy_sector_metrics_collector.h"
#include "low_phy_sector_timing_handler.h"
#include "srsran/phy/lower/lower_phy.h"

namespace srsran {

class lower_phy_cfo_controller;
class lower_phy_center_freq_controller;

/// Lower PHY sector implementation.
class lower_phy_sector : public lower_phy
{
public:
  explicit lower_phy_sector(lower_phy_timing_notifier* timing_notifier) : timing_handler(timing_notifier) {}

  // See interface for documentation.
  lower_phy_uplink_request_handler& get_uplink_request_handler() override;

  // See interface for documentation.
  lower_phy_downlink_handler& get_downlink_handler() override;

  // See interface for documentation.
  lower_phy_controller& get_controller() override;

  // See interface for documentation.
  lower_phy_cfo_controller& get_tx_cfo_control() override;

  // See interface for documentation.
  lower_phy_cfo_controller& get_rx_cfo_control() override;

  // See interface for documentation.
  lower_phy_center_freq_controller& get_tx_center_freq_control() override;

  // See interface for documentation.
  lower_phy_center_freq_controller& get_rx_center_freq_control() override;

  // See interface for documentation.
  lower_phy_tx_time_offset_controller& get_tx_time_offset_control() override;

  /// Sets the lower PHY of this sector to the given one.
  void set_lower_phy(std::unique_ptr<lower_phy> phy)
  {
    srsran_assert(phy, "Invalid lower PHY");
    low_phy = std::move(phy);
  }

  /// Returns the metrics collector of this low PHY sector.
  low_phy_sector_metrics_collector& get_metrics_collector() { return metric_collector; }

  /// Returns the timing notifier of this low PHY sector.
  lower_phy_timing_notifier& get_timing_notifier() { return timing_handler; }

private:
  low_phy_sector_metrics_collector metric_collector;
  low_phy_sector_timing_handler    timing_handler;
  std::unique_ptr<lower_phy>       low_phy;
};

} // namespace srsran
