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

#include "low_phy_sector_metrics_collector.h"
#include "srsran/phy/lower/lower_phy_baseband_metrics.h"
#include "srsran/ru/generic/ru_generic_metrics.h"
#include "srsran/support/math/math_utils.h"

using namespace srsran;

void low_phy_sector_metrics_collector::on_new_transmit_metrics(const lower_phy_baseband_metrics& metrics)
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

void low_phy_sector_metrics_collector::on_new_receive_metrics(const lower_phy_baseband_metrics& metrics)
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

void low_phy_sector_metrics_collector::collect_metrics(ru_generic_sector_metrics& metrics)
{
  {
    std::lock_guard<std::mutex> lock(tx_mutex);
    metrics.tx_avg_power_dB  = convert_power_to_dB(tx_avg_power.get_mean());
    metrics.tx_peak_power_dB = convert_power_to_dB(tx_peak_power.get_max());
    metrics.tx_papr_dB       = convert_power_to_dB(tx_peak_power.get_max() / tx_avg_power.get_mean());
    if (tx_clipping.has_value()) {
      double num               = tx_clipping.value().first;
      double den               = tx_clipping.value().second;
      metrics.tx_clipping_prob = num / den;
    }
    tx_avg_power.reset();
    tx_peak_power.reset();
    tx_clipping.reset();
  }

  {
    std::lock_guard<std::mutex> lock(rx_mutex);
    metrics.rx_avg_power_dB  = convert_power_to_dB(rx_avg_power.get_mean());
    metrics.rx_peak_power_dB = convert_power_to_dB(rx_peak_power.get_max());
    metrics.rx_papr_dB       = convert_power_to_dB(rx_peak_power.get_max() / rx_avg_power.get_mean());
    if (rx_clipping.has_value()) {
      double num               = rx_clipping.value().first;
      double den               = rx_clipping.value().second;
      metrics.rx_clipping_prob = num / den;
    }
    rx_avg_power.reset();
    rx_peak_power.reset();
    rx_clipping.reset();
  }
}
