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

namespace srsran {

class task_executor;

/// Upper physical layer executor parameters.
struct upper_phy_executor {
  /// Reference to the executor.
  task_executor* executor = nullptr;
  /// \brief Maximum number of threads that can simultaneously execute tasks.
  ///
  /// Upper physical layer factories use this parameter for calculating the number of concurrent instances necessary for
  /// processing simultaneously certain tasks.
  ///
  /// For example, if the PUCCH processing has a maximum concurrency of two, the upper physical layer factory will
  /// create two instances of PUCCH processors which will be assigned to the two different threads processing PUCCH.
  unsigned max_concurrency = 0;

  /// \brief Determines whether the executor is valid.
  ///
  /// An executor is valid if the executor pointer is not nullptr and the maximum concurrency is not zero.
  bool is_valid() const { return (executor != nullptr) && (max_concurrency != 0); }
};

/// Collects task executors and their maximum concurrency levels.
struct upper_phy_execution_configuration {
  /// PDCCH executor.
  upper_phy_executor pdcch_executor;
  /// PDSCH task executor.
  upper_phy_executor pdsch_executor;
  /// PDSCH asynchronous codeblock task executor.
  upper_phy_executor pdsch_codeblock_executor;
  /// SSB task executor.
  upper_phy_executor ssb_executor;
  /// NZP-CSI-RS task executor.
  upper_phy_executor csi_rs_executor;
  /// PRS task executor.
  upper_phy_executor prs_executor;
  /// Downlink grid pool task executor.
  upper_phy_executor dl_grid_executor;
  /// PUCCH task executor.
  upper_phy_executor pucch_executor;
  /// PUSCH task executor.
  upper_phy_executor pusch_executor;
  /// PUSCH channel estimator executor.
  upper_phy_executor pusch_ch_estimator_executor;
  /// PUSCH decoder task executor.
  upper_phy_executor pusch_decoder_executor;
  /// PRACH task executor.
  upper_phy_executor prach_executor;
  /// SRS task executor.
  upper_phy_executor srs_executor;
};

} // namespace srsran
