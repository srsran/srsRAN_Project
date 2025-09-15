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

#include "srsran/ru/ofh/ru_ofh_executor_mapper.h"
#include "srsran/adt/mpmc_queue.h"
#include "srsran/ru/ofh/ru_ofh_executor_mapper_factory.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/executors/strand_executor.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/srsran_assert.h"
#include <algorithm>
#include <cmath>

using namespace srsran;

namespace {

/// Open Fronthaul sector executor mapper implementation.
class ru_ofh_sector_executor_mapper_impl : public ru_ofh_sector_executor_mapper
{
public:
  ru_ofh_sector_executor_mapper_impl(task_executor&                 txrx_exec_,
                                     task_executor&                 downlink_exec_,
                                     std::unique_ptr<task_executor> uplink_exec_) :
    txrx_exec(txrx_exec_), downlink_exec(downlink_exec_), uplink_exec(std::move(uplink_exec_))
  {
    srsran_assert(uplink_exec, "Invalid OFH uplink executor");
  }

  // See interface for documentation.
  task_executor& txrx_executor() override { return txrx_exec; }

  // See interface for documentation.
  task_executor& downlink_executor() override { return downlink_exec; }

  // See interface for documentation.
  task_executor& uplink_executor() override { return *uplink_exec; }

private:
  task_executor&                 txrx_exec;
  task_executor&                 downlink_exec;
  std::unique_ptr<task_executor> uplink_exec;
};

/// Open Fronthaul RU executor mapper implementation managing executor mappers of the configured sectors.
class ru_ofh_executor_mapper_impl : public ru_ofh_executor_mapper
{
  /// Default queue size.
  static constexpr unsigned default_queue_size = 2048;

public:
  explicit ru_ofh_executor_mapper_impl(const ru_ofh_executor_mapper_config& config) :
    timing_exec(config.timing_executor)
  {
    report_error_if_not(config.downlink_executor, "Invalid Downlink executor");
    report_error_if_not(config.uplink_executor, "Invalid Uplink executor");
    report_error_if_not(config.timing_executor, "Invalid Timing executor");

    report_error_if_not(!config.txrx_executors.empty(), "TXRX executors for OFH must not be empty");
    report_error_if_not(std::all_of(config.txrx_executors.begin(),
                                    config.txrx_executors.end(),
                                    [](auto& exec) { return exec != nullptr; }),
                        "TXRX executors are not initialized properly");

    // Number of OFH sectors served by a single executor for transmitter and receiver tasks.
    unsigned nof_txrx_threads = config.txrx_executors.size();
    unsigned nof_sectors_per_txrx_thread =
        (config.nof_sectors > nof_txrx_threads)
            ? static_cast<unsigned>(std::ceil(config.nof_sectors / float(nof_txrx_threads)))
            : 1;

    for (unsigned i = 0; i != config.nof_sectors; ++i) {
      sector_mappers.emplace_back(
          *config.txrx_executors[i / nof_sectors_per_txrx_thread],
          *config.downlink_executor,
          make_task_strand_ptr<concurrent_queue_policy::lockfree_mpmc>(*config.uplink_executor, default_queue_size));
    }
  }

  // See interface for documentation.
  ru_ofh_sector_executor_mapper& get_sector_mapper(unsigned cell_index) override
  {
    srsran_assert(cell_index < sector_mappers.size(),
                  "The cell index {} exceeds the number of cells {}",
                  cell_index,
                  sector_mappers.size());
    return sector_mappers[cell_index];
  }

  // See interface for documentation.
  task_executor& timing_executor() override { return *timing_exec; }

private:
  task_executor*                                  timing_exec;
  std::vector<ru_ofh_sector_executor_mapper_impl> sector_mappers;
};

} // namespace

std::unique_ptr<ru_ofh_executor_mapper>
srsran::create_ofh_ru_executor_mapper(const ru_ofh_executor_mapper_config& config)
{
  return std::make_unique<ru_ofh_executor_mapper_impl>(config);
}
