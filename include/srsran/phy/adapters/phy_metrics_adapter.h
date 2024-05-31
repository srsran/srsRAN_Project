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

#include "srsran/phy/lower/lower_phy_baseband_metrics.h"
#include "srsran/phy/lower/lower_phy_metrics_notifier.h"
#include "srsran/support/math_utils.h"
#include "srsran/support/stats.h"
#include <fmt/format.h>
#include <mutex>

namespace srsran {

/// \brief Implements a generic physical layer metrics adapter.
///
/// Currently, the adapter only collects the lower physical layer metrics.
class phy_metrics_adapter : public lower_phy_metrics_notifier
{
public:
  // See interface for documentation.
  void on_new_transmit_metrics(const lower_phy_baseband_metrics& metrics) override
  {
    std::lock_guard<std::mutex> lock(tx_mutex);
    tx_avg_power.update(metrics.avg_power);
    tx_peak_power.update(metrics.peak_power);
    if (!tx_clipping.has_value()) {
      tx_clipping = metrics.clipping;
    } else {
      tx_clipping->first += metrics.clipping->first;
      tx_clipping->second += metrics.clipping->second;
    }
  }

  // See interface for documentation.
  void on_new_receive_metrics(const lower_phy_baseband_metrics& metrics) override
  {
    std::lock_guard<std::mutex> lock(rx_mutex);
    rx_avg_power.update(metrics.avg_power);
    rx_peak_power.update(metrics.peak_power);
    if (metrics.clipping.has_value()) {
      if (!rx_clipping.has_value()) {
        rx_clipping = metrics.clipping;
      } else {
        rx_clipping->first += metrics.clipping->first;
        rx_clipping->second += metrics.clipping->second;
      }
    }
  }

  static void print_header()
  {
    fmt::print("| ------------------- TX ------------------ | ------------------- RX ------------------ |\n");
    fmt::print("| Avg. power | Peak power | PAPR | Clipping | Avg. power | Peak power | PAPR | Clipping |\n");
  }

  void print_metrics()
  {
    float  tx_avg_power_dB;
    float  tx_peak_power_dB;
    float  tx_papr_dB;
    double tx_clipping_prob = std::numeric_limits<double>::quiet_NaN();
    {
      std::lock_guard<std::mutex> lock(tx_mutex);
      tx_avg_power_dB  = convert_power_to_dB(tx_avg_power.get_mean());
      tx_peak_power_dB = convert_power_to_dB(tx_peak_power.get_max());
      tx_papr_dB       = convert_power_to_dB(tx_peak_power.get_max() / tx_avg_power.get_mean());
      if (tx_clipping.has_value()) {
        double num       = tx_clipping.value().first;
        double den       = tx_clipping.value().second;
        tx_clipping_prob = num / den;
      }
      tx_avg_power.reset();
      tx_peak_power.reset();
      tx_clipping.reset();
    }

    float  rx_avg_power_dB;
    float  rx_peak_power_dB;
    float  rx_papr_dB;
    double rx_clipping_prob = std::numeric_limits<double>::quiet_NaN();
    {
      std::lock_guard<std::mutex> lock(rx_mutex);
      rx_avg_power_dB  = convert_power_to_dB(rx_avg_power.get_mean());
      rx_peak_power_dB = convert_power_to_dB(rx_peak_power.get_max());
      rx_papr_dB       = convert_power_to_dB(rx_peak_power.get_max() / rx_avg_power.get_mean());
      if (rx_clipping.has_value()) {
        double num       = rx_clipping.value().first;
        double den       = rx_clipping.value().second;
        rx_clipping_prob = num / den;
      }
      rx_avg_power.reset();
      rx_peak_power.reset();
      rx_clipping.reset();
    }

    fmt::print("| {:>10.1f} | {:>10.1f} | {:>4.1f} | {:>8.1e} | {:>10.1f} | {:>10.1f} | {:>4.1f} | {:>8.1e} |\n",
               tx_avg_power_dB,
               tx_peak_power_dB,
               tx_papr_dB,
               tx_clipping_prob,
               rx_avg_power_dB,
               rx_peak_power_dB,
               rx_papr_dB,
               rx_clipping_prob);
  }

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
