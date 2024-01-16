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

#include "srsran/phy/upper/uplink_processor.h"

namespace srsran {

class uplink_processor_spy : public uplink_processor
{
  bool has_process_prach_method_called = false;
  bool has_process_pusch_method_called = false;
  bool has_process_pucch_method_called = false;

public:
  void process_prach(upper_phy_rx_results_notifier& notifier,
                     const prach_buffer&            buffer,
                     const prach_buffer_context&    context) override
  {
    has_process_prach_method_called = true;
  }

  void process_pusch(span<uint8_t>                      data,
                     unique_rx_buffer                   rm_buffer,
                     upper_phy_rx_results_notifier&     notifier,
                     const resource_grid_reader&        grid,
                     const uplink_processor::pusch_pdu& pdu) override
  {
    has_process_pusch_method_called = true;
  }

  void process_pucch(upper_phy_rx_results_notifier& notifier,
                     const resource_grid_reader&    grid,
                     const pucch_pdu&               config) override
  {
    has_process_pucch_method_called = true;
  }

  bool is_process_prach_method_called() const { return has_process_prach_method_called; }

  bool is_process_pusch_method_called() const { return has_process_pusch_method_called; }

  bool is_process_pucch_method_called() const { return has_process_pucch_method_called; }
};

} // namespace srsran
