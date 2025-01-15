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
