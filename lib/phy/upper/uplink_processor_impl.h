/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/phy/upper/uplink_processor.h"

namespace srsran {

class task_executor;

/// \brief Uplink processor implementation.
///
/// In this implementation, every task is processed by its channel processor. When a task finishes, the results are
/// notified by the upper_phy_rx_results_notifier interface.
class uplink_processor_impl : public uplink_processor
{
public:
  uplink_processor_impl(std::unique_ptr<prach_detector>  prach_,
                        std::unique_ptr<pusch_processor> pusch_proc_,
                        std::unique_ptr<pucch_processor> pucch_proc_);

  // See interface for documentation.
  void process_prach(upper_phy_rx_results_notifier& notifier,
                     const prach_buffer&            buffer,
                     const prach_buffer_context&    context) override;

  // See interface for documentation.
  void process_pusch(span<uint8_t>                      data,
                     unique_rx_softbuffer               softbuffer,
                     upper_phy_rx_results_notifier&     notifier,
                     const resource_grid_reader&        grid,
                     const uplink_processor::pusch_pdu& pdu) override;

  // See interface for documentation.
  void process_pucch(upper_phy_rx_results_notifier& notifier,
                     const resource_grid_reader&    grid,
                     const pucch_pdu&               pdu) override;

private:
  /// PRACH detector.
  std::unique_ptr<prach_detector> prach;
  /// PUSCH processor.
  std::unique_ptr<pusch_processor> pusch_proc;
  /// PUCCH processor.
  std::unique_ptr<pucch_processor> pucch_proc;
};

} // namespace srsran
