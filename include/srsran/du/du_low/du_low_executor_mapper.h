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

#include "srsran/ran/du_types.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/executors/task_executor.h"
#include <chrono>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

namespace srsran {
namespace srs_du {

/// \brief Interface used to access different executors used in the DU-Low.
class du_low_cell_executor_mapper
{
public:
  virtual ~du_low_cell_executor_mapper() = default;

  /// Retrieves the task executor for processing PDCCH.
  virtual task_executor& pdcch_executor() = 0;

  /// Retrieves the task executor for processing PDSCH.
  virtual task_executor& pdsch_executor() = 0;

  /// Retrieves the task executor for processing SSB.
  virtual task_executor& ssb_executor() = 0;

  /// Retrieves the task executor for processing NZP-CSI-RS.
  virtual task_executor& csi_rs_executor() = 0;

  /// Retrieves the task executor for processing PRS.
  virtual task_executor& prs_executor() = 0;

  /// Retrieves the task executor for DL resource grid pool housekeeping.
  virtual task_executor& dl_grid_pool_executor() = 0;

  /// \brief Retrieves the PDSCH asynchronous executor.
  ///
  /// The PDSCH asynchronous executor is in charge of processing PDSCH transmissions concurrently (if configured).
  virtual task_executor& pdsch_codeblock_executor() = 0;

  /// \brief Retrieves the Preamble for Random Access Channel (PRACH) task executor.
  ///
  /// The PRACH task executor is in charge of detecting incoming PRACH transmissions.
  virtual task_executor& prach_executor() = 0;

  /// \brief Retrieves the PUSCH task executor.
  ///
  /// The PUSCH executor is in charge of processing PUSCH channel estimation, equalization, demodulation  mapping and
  /// UL-SCH demultiplexing.
  ///
  /// If the upper physical layer is configured for decoding UL-SCH asynchronously, the UL-SCH decoding is performed by
  /// the PUSCH decoder executor. Otherwise, this executor decodes UL-SCH too.
  virtual task_executor& pusch_executor() = 0;

  /// \brief Retrieves the PUSCH decoder task executor.
  ///
  /// The PUSCH decoder is in charge of decoding UL-SCH codeblocks asynchronously. Note that codeblock decoding can
  /// have a high duration compared with other tasks. The duration is in the order of hundreds of microseconds per
  /// codeblock.
  virtual task_executor& pusch_decoder_executor() = 0;

  /// \brief Retrieves the PUCCH reception task executor.
  ///
  /// The PUCCH reception executor is in charge of receiving all supported formats of PUCCH.
  virtual task_executor& pucch_executor() = 0;

  /// \brief Retrieves the Sounding Reference Signal (SRS) task executor.
  ///
  /// The SRS task executor is in charge of estimating the propagation channel based on SRS.
  virtual task_executor& srs_executor() = 0;
};

/// \brief DU low executor mapper interface.
///
/// Provides access to the different cell executor mappers.
class du_low_executor_mapper
{
public:
  /// Default destructor.
  virtual ~du_low_executor_mapper() = default;

  /// \brief Retrieves the DU low cell executor mapper for a given cell index.
  ///
  /// \remark An assertion is triggered the cell index exceeds the number of executor configurations.
  virtual du_low_cell_executor_mapper& get_cell_mapper(unsigned cell_index) = 0;

  /// Retrieves the DU low cell executor mapper via closed braces operator.
  du_low_cell_executor_mapper& operator[](unsigned cell_index) { return get_cell_mapper(cell_index); }
};

/// \brief Collects the task executor mapper for using a single task executor.
struct du_low_executor_mapper_single_exec_config {
  /// Common executor for all the DU low cell tasks.
  task_executor* common_executor = nullptr;
};

/// \brief Collects the task executor mapper manual mapping.
///
/// All task executors are given.
struct du_low_executor_mapper_manual_exec_config {
  /// High priority executor.
  task_executor* high_priority_executor = nullptr;
  /// Medium priority executor.
  task_executor* medium_priority_executor = nullptr;
  /// Low priority executor.
  task_executor* low_priority_executor = nullptr;
  /// Executor for downlink transmissions.
  task_executor* dl_executor = nullptr;
  /// Executor for PDSCH concurrent processing.
  task_executor* pdsch_executor = nullptr;
  /// Limits the number of threads that can concurrently process PUCCH.
  unsigned max_concurrent_pucch = 2;
  /// Limits the number of threads that can concurrently process PUSCH.
  unsigned max_concurrent_pusch = 2;
  /// Limits the number of threads that can concurrently process SRS.
  unsigned max_concurrent_srs = 2;
  /// Limits the number of threads that can decode PUSCH concurrently.
  unsigned max_concurrent_pusch_decoders = 2;
};

/// Variant of the DU low executor mapping configuration.
using du_low_executor_mapper_exec_config =
    std::variant<du_low_executor_mapper_single_exec_config, du_low_executor_mapper_manual_exec_config>;

/// Collects the DU low executor mapper metrics configuration.
struct du_low_executor_mapper_metric_config {
  /// Period.
  std::chrono::milliseconds period;
  /// Sequential executor for metric processing.
  task_executor& sequential_executor;
  /// Metrics logger.
  srslog::log_channel& logger;
};

/// Configuration of DU-low executor mapper.
struct du_low_executor_mapper_config {
  /// \brief Executor configuration for each cell.
  ///
  /// \remark An assertion is triggered if it is empty.
  std::vector<du_low_executor_mapper_exec_config> cells;
  /// \brief Optional executor metric configuration.
  ///
  /// If it is present, the executor mapper wraps the executors with metric decorators.
  std::optional<du_low_executor_mapper_metric_config> metrics;
};

/// \brief Creates an executor mapper for the DU-low.
std::unique_ptr<du_low_executor_mapper> create_du_low_executor_mapper(const du_low_executor_mapper_config& config);

} // namespace srs_du
} // namespace srsran
