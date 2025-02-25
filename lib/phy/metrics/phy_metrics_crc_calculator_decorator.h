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

#pragma once

#include "srsran/phy/metrics/phy_metrics_notifiers.h"
#include "srsran/phy/metrics/phy_metrics_reports.h"
#include "srsran/phy/upper/channel_coding/crc_calculator.h"
#include "srsran/support/resource_usage/scoped_resource_usage.h"
#include <memory>

namespace srsran {

/// CRC calculator metric decorator.
class phy_metrics_crc_calculator_decorator : public crc_calculator
{
public:
  /// Creates an CRC calculator decorator from a base instance and a metric notifier.
  phy_metrics_crc_calculator_decorator(std::unique_ptr<crc_calculator> base_calculator_,
                                       crc_calculator_metric_notifier& notifier_) :
    base_calculator(std::move(base_calculator_)), notifier(notifier_)
  {
    srsran_assert(base_calculator, "Invalid CRC calculator.");
  }

  // See interface for documentation.
  crc_calculator_checksum_t calculate_byte(span<const uint8_t> data) const override
  {
    crc_calculator_metrics    metrics;
    crc_calculator_checksum_t ret;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      ret = base_calculator->calculate_byte(data);
    }
    // Report metrics.
    metrics.poly     = base_calculator->get_generator_poly();
    metrics.nof_bits = units::bytes(data.size()).to_bits();
    notifier.on_new_metric(metrics);

    return ret;
  }

  // See interface for documentation.
  crc_calculator_checksum_t calculate_bit(span<const uint8_t> data) const override
  {
    crc_calculator_metrics    metrics;
    crc_calculator_checksum_t ret;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      ret = base_calculator->calculate_bit(data);
    }
    // Report metrics.
    metrics.poly     = base_calculator->get_generator_poly();
    metrics.nof_bits = units::bits(data.size());
    notifier.on_new_metric(metrics);

    return ret;
  }

  // See interface for documentation.
  crc_calculator_checksum_t calculate(const bit_buffer& data) const override
  {
    crc_calculator_metrics    metrics;
    crc_calculator_checksum_t ret;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      ret = base_calculator->calculate(data);
    }
    // Report metrics.
    metrics.poly     = base_calculator->get_generator_poly();
    metrics.nof_bits = units::bits(data.size());
    notifier.on_new_metric(metrics);

    return ret;
  }

  // See interface for documentation.
  crc_generator_poly get_generator_poly() const override { return base_calculator->get_generator_poly(); }

private:
  std::unique_ptr<crc_calculator> base_calculator;
  crc_calculator_metric_notifier& notifier;
};

} // namespace srsran
