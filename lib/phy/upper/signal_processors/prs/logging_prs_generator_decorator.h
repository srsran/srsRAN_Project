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
#include "srsran/phy/upper/signal_processors/prs/formatters.h"
#include "srsran/phy/upper/signal_processors/prs/prs_generator.h"
#include "srsran/phy/upper/signal_processors/prs/prs_generator_configuration.h"

namespace srsran {

template <typename Func>
std::chrono::nanoseconds time_execution(Func&& func)
{
  auto start = std::chrono::steady_clock::now();
  func();
  auto end = std::chrono::steady_clock::now();

  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
}

class logging_prs_generator_decorator : public prs_generator
{
public:
  logging_prs_generator_decorator(srslog::basic_logger& logger_, std::unique_ptr<prs_generator> generator_) :
    logger(logger_), generator(std::move(generator_))
  {
    srsran_assert(generator, "Invalid PRS generator.");
  }

  void generate(resource_grid_writer& grid, const prs_generator_configuration& config) override
  {
    const auto&& func = [&]() { generator->generate(grid, config); };

    std::chrono::nanoseconds time_ns = time_execution(func);

    if (logger.debug.enabled()) {
      // Detailed log information, including a list of all config fields.
      logger.debug(config.slot.sfn(), config.slot.slot_index(), "PRS: {:s} {}\n  {:n}", config, time_ns, config);
      return;
    }
    // Single line log entry.
    logger.info(config.slot.sfn(), config.slot.slot_index(), "PRS: {:s} {}", config, time_ns);
  }

private:
  srslog::basic_logger&          logger;
  std::unique_ptr<prs_generator> generator;
};

} // namespace srsran
