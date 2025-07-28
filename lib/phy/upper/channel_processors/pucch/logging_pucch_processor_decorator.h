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

#include "srsran/phy/support/support_formatters.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_processor.h"

namespace srsran {

template <typename Func>
inline std::chrono::nanoseconds time_execution(Func&& func)
{
  auto start = std::chrono::steady_clock::now();
  func();
  auto end = std::chrono::steady_clock::now();

  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
}

class logging_pucch_processor_decorator : public pucch_processor
{
  template <typename Config>
  pucch_processor_result process_(const resource_grid_reader& grid, const Config& config)
  {
    pucch_processor_result result;

    std::chrono::nanoseconds time_ns =
        time_execution([this, &result, &grid, &config]() { result = processor->process(grid, config); });
    if (logger.debug.enabled()) {
      // Detailed log information, including a list of all PUCCH configuration and result fields.
      logger.debug(config.slot.sfn(),
                   config.slot.slot_index(),
                   "PUCCH: {:s} {:s} {}\n  {:n}\n  {:n}",
                   config,
                   result,
                   time_ns,
                   config,
                   result);
    } else {
      // Single line log entry.
      logger.info(config.slot.sfn(), config.slot.slot_index(), "PUCCH: {:s} {:s} {}", config, result, time_ns);
    }

    return result;
  }

public:
  logging_pucch_processor_decorator(srslog::basic_logger& logger_, std::unique_ptr<pucch_processor> processor_) :
    logger(logger_), processor(std::move(processor_))
  {
    srsran_assert(processor, "Invalid processor.");
  }

  pucch_processor_result process(const resource_grid_reader& grid, const format0_configuration& config) override
  {
    return process_(grid, config);
  }

  pucch_format1_map<pucch_processor_result> process(const resource_grid_reader&        grid,
                                                    const format1_batch_configuration& config) override
  {
    if (!logger.debug.enabled() && !logger.info.enabled()) {
      return processor->process(grid, config);
    }

    pucch_format1_map<pucch_processor_result> results;

    std::chrono::nanoseconds time_ns =
        time_execution([this, &results, &grid, &config]() { results = processor->process(grid, config); });

    // Iterate each of the UE dedicated configuration.
    for (unsigned initial_cyclic_shift = pucch_constants::format1_initial_cyclic_shift_range.start();
         initial_cyclic_shift != pucch_constants::format1_initial_cyclic_shift_range.stop();
         ++initial_cyclic_shift) {
      for (unsigned time_domain_occ = pucch_constants::format1_time_domain_occ_range.start();
           time_domain_occ != pucch_constants::format1_time_domain_occ_range.stop();
           ++time_domain_occ) {
        // Skip result if it is not available.
        if (!config.entries.contains(initial_cyclic_shift, time_domain_occ) ||
            !results.contains(initial_cyclic_shift, time_domain_occ)) {
          continue;
        }

        // Select configuration and results->
        const auto& common_config = config.common_config;
        const auto& ue_config     = config.entries.get(initial_cyclic_shift, time_domain_occ);
        const auto& result        = results.get(initial_cyclic_shift, time_domain_occ);

        // Build formateable Format 1 configuraton.
        format1_configuration all_config = {.context              = ue_config.context,
                                            .slot                 = common_config.slot,
                                            .bwp_size_rb          = common_config.bwp_size_rb,
                                            .bwp_start_rb         = common_config.bwp_start_rb,
                                            .cp                   = common_config.cp,
                                            .starting_prb         = common_config.starting_prb,
                                            .second_hop_prb       = common_config.second_hop_prb,
                                            .n_id                 = common_config.n_id,
                                            .nof_harq_ack         = ue_config.nof_harq_ack,
                                            .ports                = common_config.ports,
                                            .initial_cyclic_shift = initial_cyclic_shift,
                                            .nof_symbols          = common_config.nof_symbols,
                                            .start_symbol_index   = common_config.start_symbol_index,
                                            .time_domain_occ      = time_domain_occ};

        if (logger.debug.enabled()) {
          // Detailed log information, including a list of all PUCCH configuration and result fields.
          logger.debug(common_config.slot.sfn(),
                       common_config.slot.slot_index(),
                       "PUCCH: {:s} {:s} {}\n  {:n}\n  {:n}",
                       all_config,
                       result,
                       time_ns,
                       all_config,
                       result);
        } else {
          // Single line log entry.
          logger.info(common_config.slot.sfn(),
                      common_config.slot.slot_index(),
                      "PUCCH: {:s} {:s} {}",
                      all_config,
                      result,
                      time_ns);
        }
      }
    }

    return results;
  }
  pucch_processor_result process(const resource_grid_reader& grid, const format2_configuration& config) override
  {
    return process_(grid, config);
  }
  pucch_processor_result process(const resource_grid_reader& grid, const format3_configuration& config) override
  {
    return process_(grid, config);
  }
  pucch_processor_result process(const resource_grid_reader& grid, const format4_configuration& config) override
  {
    return process_(grid, config);
  }

private:
  srslog::basic_logger&            logger;
  std::unique_ptr<pucch_processor> processor;
};

} // namespace srsran
