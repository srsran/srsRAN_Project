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

#include "srsran/phy/lower/lower_phy_metrics_notifier.h"
#include "srsran/support/math/stats.h"
#include <mutex>
#include <optional>

namespace srsran {

struct ru_generic_sector_metrics;

/// Low PHY sector metrics collector.
class low_phy_sector_metrics_collector : public lower_phy_metrics_notifier
{
public:
  // See interface for documentation.
  void on_new_transmit_metrics(const lower_phy_baseband_metrics& metrics) override;

  // See interface for documentation.
  void on_new_receive_metrics(const lower_phy_baseband_metrics& metrics) override;

  /// Collects the metrics of the low PHY sector and fills the given structure.
  void collect_metrics(ru_generic_sector_metrics& metrics);

  /// Returns the lower PHY metrics notifier of this sector metricd collector.
  lower_phy_metrics_notifier& get_lower_phy_metrics_notifier() { return *this; }

private:
  std::mutex                                   tx_mutex;
  sample_statistics<float>                     tx_avg_power;
  sample_statistics<float>                     tx_peak_power;
  std::optional<std::pair<uint64_t, uint64_t>> tx_clipping;
  std::mutex                                   rx_mutex;
  sample_statistics<float>                     rx_avg_power;
  sample_statistics<float>                     rx_peak_power;
  std::optional<std::pair<uint64_t, uint64_t>> rx_clipping;
};

} // namespace srsran
