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

#include "srsran/phy/support/support_formatters.h"
#include "srsran/phy/upper/signal_processors/srs/formatters.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_configuration.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_result.h"
#include "srsran/phy/upper/unique_rx_buffer.h"

namespace srsran {

/// Logging Sounding Reference Signal channel estimator decorator.
class logging_srs_estimator_decorator : public srs_estimator
{
  template <typename Func>
  static std::chrono::nanoseconds time_execution(Func&& func)
  {
    auto start = std::chrono::steady_clock::now();
    func();
    auto end = std::chrono::steady_clock::now();

    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  }

public:
  /// Creates a logging decorator for SRS channel estimator.
  logging_srs_estimator_decorator(srslog::basic_logger& logger_, std::unique_ptr<srs_estimator> srs_) :
    logger(logger_), srs(std::move(srs_))
  {
    srsran_assert(srs, "Invalid SRS estimator instance.");
  }

  srs_estimator_result estimate(const resource_grid_reader& grid, const srs_estimator_configuration& config) override
  {
    srs_estimator_result     result;
    std::chrono::nanoseconds time_ns = time_execution([&]() { result = srs->estimate(grid, config); });

    if (logger.debug.enabled()) {
      // Detailed log information, including a list of all PDU fields.
      logger.debug(config.slot.sfn(),
                   config.slot.slot_index(),
                   "SRS: {:s} {:s} {}\n  {:n}\n  {:n}",
                   config,
                   result,
                   time_ns,
                   config,
                   result);
    } else {
      // Single line log entry.
      logger.info(config.slot.sfn(), config.slot.slot_index(), "SRS: {:s} {:s} {}", config, result, time_ns);
    }

    return result;
  }

private:
  srslog::basic_logger&          logger;
  std::unique_ptr<srs_estimator> srs;
};

} // namespace srsran
